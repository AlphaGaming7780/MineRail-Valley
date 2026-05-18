// LogManager.hpp
#pragma once

#include <chrono>
#include <condition_variable>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Game::Logging {

    enum class LogLevel { Info, Warn, Error };

    struct LogMessage
    {
        LogLevel level;        // Niveau du message
        std::string message;   // Contenu du message
        std::chrono::system_clock::time_point timestamp; // (facultatif) pour l'horodatage
    };

    // LogManager singleton
    class LogManager {
    public:
        // récupère l'instance (thread-safe C++11 static init)
        static LogManager& instance() {
            static LogManager inst;
            return inst;
        }

        // Append a line to the buffer for given logger id
        void append(const std::string& id, LogLevel level, const std::string& message) {

            LogMessage logMsg{
                level,
                message,
                std::chrono::system_clock::now()
            };
            append(id, logMsg);
        }

        void append(const std::string& id, const LogMessage logMsg) {
            {
                std::lock_guard<std::mutex> lk(mutex_);
                buffers_[id].push_back(logMsg);
            }
            cv_.notify_one();
        }

        // Force flush synchronously (optionnel)
        void flushNow() {
            notify_flush();
            // wait for flush to finish: acquire fileMutex to ensure background thread finished writing
            // Not perfect but allows a simple barrier: we wait briefly for background thread to pick up.
            std::unique_lock<std::mutex> lk(flushWaitMutex_);
            flushWaitCv_.wait_for(lk, std::chrono::milliseconds(200));
        }

        // Proper shutdown (call once when program exits)
        void shutdown() {
            {
                std::lock_guard<std::mutex> lk(mutex_);
                stopping_ = true;
            }
            cv_.notify_one();
            if (worker_.joinable()) worker_.join();
        }

        // Deleted copy
        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;

        ~LogManager() {
            shutdown();
        }

    private:

        LogManager()
            : flushIntervalMs_(200), // flush every 200 ms if no notify
            flushThreshold_(50),   // optional: flush if buffer >= 50 lines per id
            stopping_(false)
        {
            worker_ = std::thread(&LogManager::workerLoop, this);
        }

        std::string getTextColor(LogLevel l) {
            switch (l) {
            case LogLevel::Info: return "\033[0m";
            case LogLevel::Warn: return "\033[33m";
            case LogLevel::Error: return "\033[91m";
            default: return "\033[0m";
            }
        }

        std::string formatLine(const std::string& id, LogLevel level, const std::string& msg, const bool console = false) {
            std::ostringstream oss;
            oss << "[" << currentTimestamp() << "] ";
            if (console) oss << "[" << id << "] ";
            oss << "[" << levelToString(level) << "] ";
            oss << msg;
            return oss.str();
        }

        inline std::string levelToString(LogLevel l) {
            switch (l) {
            case LogLevel::Info: return  "INFO";
            case LogLevel::Warn: return  "WARN";
            case LogLevel::Error: return "ERROR";
            default: return "UNK";
            }
        }

        inline std::string currentTimestamp() {
            using namespace std::chrono;
            auto now = system_clock::now();
            std::time_t t = system_clock::to_time_t(now);
            std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
            localtime_s(&tm, &t);
#else
            localtime_r(&t, &tm);
#endif
            char buf[32];
            std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
            return std::string(buf);
        }

        void notify_flush() {
            cv_.notify_one();
        }

        void workerLoop() {
            using namespace std::chrono;
            namespace fs = std::filesystem;

            while (true) {
                //std::unordered_map<std::string, std::vector<std::string>> localCopy;
                std::unordered_map<std::string, std::vector<LogMessage>> localCopy;

                // wait for either notification or timeout
                {
                    std::unique_lock<std::mutex> lk(mutex_);
                    cv_.wait_for(lk, milliseconds(flushIntervalMs_), [this]() {
                        if (stopping_) return true;
                        // also wake up if any buffer reached threshold
                        for (const auto& p : buffers_) {
                            if (!p.second.empty() && p.second.size() >= flushThreshold_) return true;
                        }
                        return false;
                        });

                    if (buffers_.empty() && stopping_) {
                        // Nothing to flush, exit
                        break;
                    }

                    // Swap buffers_ with localCopy to minimize time holding the mutex.
                    localCopy.swap(buffers_);
                    // buffers_ is now empty; other threads may append concurrently.
                }

                // Write localCopy to files (outside mutex)
                for (auto& kv : localCopy) {
                    const std::string& id = kv.first;
                    auto& lines = kv.second;
                    if (lines.empty()) continue;

                    WriteLogs(id, lines);

                }

                // notify any flushNow waiter (best effort)
                {
                    std::lock_guard<std::mutex> lk(flushWaitMutex_);
                    flushWaitCv_.notify_all();
                }

                // exit if stopping and no more buffers (loop will check at top)
                {
                    std::lock_guard<std::mutex> lk(mutex_);
                    if (stopping_ && buffers_.empty()) break;
                }
            }

            // before exiting attempt final flush of any leftover buffers
            //std::unordered_map<std::string, std::vector<std::string>> finalCopy;
            std::unordered_map<std::string, std::vector<LogMessage>> finalCopy;
            {
                std::lock_guard<std::mutex> lk(mutex_);
                finalCopy.swap(buffers_);
            }
            for (auto& kv : finalCopy) {
                const std::string& id = kv.first;
                auto& lines = kv.second;
                if (lines.empty()) continue;

                WriteLogs(id, lines);
            }
        }

        bool WriteLogs(const std::string& id, const std::vector<LogMessage>& lines)
        {
#if DEBUG
            for (auto& logMsg : lines) {
                std::string line = formatLine(id, logMsg.level, logMsg.message, true);
                std::cout << getTextColor(logMsg.level) << line << std::endl;
            }
            std::cout.flush();
#endif // DEBUG

            namespace fs = std::filesystem;
            // Determine mode: truncate on first write this run, then append
            std::ios_base::openmode mode = std::ios::out;
            {
                std::lock_guard<std::mutex> lk(fileSetMutex_);
                if (firstWriteDone_.find(id) == firstWriteDone_.end()) {
                    mode |= std::ios::trunc; // overwrite previous run logs
                    firstWriteDone_.insert(id);
                }
                else {
                    mode |= std::ios::app;
                }
            }

            std::string filename = id + ".log";

            fs::path execPath = fs::current_path();

            fs::path logDir = execPath / "Logs";

            if (!fs::exists(logDir)) {
                fs::create_directories(logDir);
            }

            fs::path logFile = logDir / filename;

            std::ofstream ofs(logFile, mode);
            if (!ofs) {
                // write to cerr if file open fails (do not throw)
                std::lock_guard<std::mutex> lk(ioMutex_);
                std::cerr << "LogManager: failed to open log file: " << filename << "\n";
                return false;
            }

            for (auto& logMsg : lines) {
                std::string line = formatLine(id, logMsg.level, logMsg.message);
                ofs << line << '\n';
            }
            // flush to ensure durability
            ofs.flush();
            return true;
        }

        // Members
        std::mutex mutex_;
        std::condition_variable cv_;
        std::unordered_map<std::string, std::vector<LogMessage>> buffers_;
        //std::unordered_map<std::string, std::vector<std::string>> buffers_;

        // For file state
        std::mutex fileSetMutex_;
        std::unordered_set<std::string> firstWriteDone_;

        // For notifying flushNow
        std::mutex flushWaitMutex_;
        std::condition_variable flushWaitCv_;

        // For reporting file open errors
        std::mutex ioMutex_;

        std::thread worker_;
        bool stopping_;

        // Config
        int flushIntervalMs_;
        size_t flushThreshold_;
    };

}