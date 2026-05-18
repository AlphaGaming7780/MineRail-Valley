#include <PallasEngine/Logging/LogManager.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace pallas {

    // --- Singleton ---
    LogManager& LogManager::instance() {
        static LogManager inst;
        return inst;
    }

    // --- Constructeur ---
    LogManager::LogManager()
        : flushIntervalMs_(200),
        flushThreshold_(50),
        stopping_(false)
    {
        worker_ = std::thread(&LogManager::workerLoop, this);
    }

    // --- Destructeur ---
    LogManager::~LogManager() {
        shutdown();
    }

    // --- Append ---
    void LogManager::append(const std::string& id, LogLevel level, const std::string& message) {
        LogMessage logMsg{ level, message, std::chrono::system_clock::now() };
        append(id, logMsg);
    }

    void LogManager::append(const std::string& id, const LogMessage& logMsg) {
#if DEBUG
        std::cout << getTextColor(logMsg.level) << formatLine(id, logMsg.level, logMsg.message, true) << "\033[0m" << std::endl;
#endif
        {
            std::lock_guard<std::mutex> lk(mutex_);
            buffers_[id].push_back(logMsg);
        }
        cv_.notify_one();
    }

    // --- Flush synchronously ---
    void LogManager::flushNow() {
        notify_flush();
        std::unique_lock<std::mutex> lk(flushWaitMutex_);
        flushWaitCv_.wait_for(lk, std::chrono::milliseconds(200));
    }

    // --- Shutdown ---
    void LogManager::shutdown() {
        {
            std::lock_guard<std::mutex> lk(mutex_);
            stopping_ = true;
        }
        cv_.notify_one();
        if (worker_.joinable()) worker_.join();
    }

    // --- Worker thread loop ---
    void LogManager::workerLoop() {
        namespace fs = std::filesystem;

        while (true) {
            std::unordered_map<std::string, std::vector<LogMessage>> localCopy;

            {
                std::unique_lock<std::mutex> lk(mutex_);
                cv_.wait_for(lk, std::chrono::milliseconds(flushIntervalMs_), [this]() {
                    if (stopping_) return true;
                    for (const auto& p : buffers_)
                        if (!p.second.empty() && p.second.size() >= flushThreshold_) return true;
                    return false;
                });

                if (buffers_.empty() && stopping_) break;
                localCopy.swap(buffers_);
            }

            for (auto& kv : localCopy)
                if (!kv.second.empty()) WriteLogs(kv.first, kv.second);

            {
                std::lock_guard<std::mutex> lk(flushWaitMutex_);
                flushWaitCv_.notify_all();
            }

            {
                std::lock_guard<std::mutex> lk(mutex_);
                if (stopping_ && buffers_.empty()) break;
            }
        }

        // Flush final
        std::unordered_map<std::string, std::vector<LogMessage>> finalCopy;
        {
            std::lock_guard<std::mutex> lk(mutex_);
            finalCopy.swap(buffers_);
        }
        for (auto& kv : finalCopy)
            if (!kv.second.empty()) WriteLogs(kv.first, kv.second);
    }

    // --- Write logs to file and console ---
    bool LogManager::WriteLogs(const std::string& id, const std::vector<LogMessage>& lines) {

        // Peut être pas ouf d'écrire les logs dans la console ici, car ils sont donc décalés et pas vraiment en temps réel.
        // Fix : Je l'ai déplacer dans la fonction append, les log sont donc écrit en sync dans la console, mais en temp réel dcp.
//#if DEBUG
//        for (auto& logMsg : lines) {
//            std::cout << getTextColor(logMsg.level) << formatLine(id, logMsg.level, logMsg.message, true) << std::endl;
//        }
//#endif

        namespace fs = std::filesystem;
        std::ios_base::openmode mode = std::ios::out;
        {
            std::lock_guard<std::mutex> lk(fileSetMutex_);
            if (firstWriteDone_.find(id) == firstWriteDone_.end()) {
                mode |= std::ios::trunc;
                firstWriteDone_.insert(id);
            }
            else mode |= std::ios::app;
        }

        fs::path logDir = fs::current_path() / "Logs";
        if (!fs::exists(logDir)) fs::create_directories(logDir);

        std::ofstream ofs(logDir / (id + ".log"), mode);
        if (!ofs) {
            std::lock_guard<std::mutex> lk(ioMutex_);
            std::cerr << "LogManager: failed to open log file: " << id << "\n";
            return false;
        }

        for (auto& logMsg : lines)
            ofs << formatLine(id, logMsg.level, logMsg.message) << '\n';

        ofs.flush();
        return true;
    }

    // --- Helpers ---
    void LogManager::notify_flush() { cv_.notify_one(); }

    std::string LogManager::formatLine(const std::string& id, LogLevel level, const std::string_view msg, bool console) {
        std::ostringstream oss;
        oss << "[" << currentTimestamp() << "] ";
        if (console) oss << "[" << id << "] ";
        oss << "[" << levelToString(level) << "]\t" << msg;
        return oss.str();
    }

    std::string LogManager::levelToString(LogLevel l) {
        switch (l) {
        case LogLevel::Info:  return "INFO";
        case LogLevel::Warn:  return "WARN";
        case LogLevel::Error: return "ERROR";
        default: return "UNK";
        }
    }

    std::string LogManager::currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        std::time_t t = std::chrono::system_clock::to_time_t(now);
        std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
        return buf;
    }

    std::string LogManager::getTextColor(LogLevel l) {
        switch (l) {
        case LogLevel::Info:  return "\033[0m";
        case LogLevel::Warn:  return "\033[33m";
        case LogLevel::Error: return "\033[91m";
        default: return "\033[0m";
        }
    }

} // namespace Game::Logging
