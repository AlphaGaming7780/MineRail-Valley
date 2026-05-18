#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace pallas {

    enum class LogLevel { Info, Warn, Error };

    struct LogMessage {
        LogLevel level;
        std::string message;
        std::chrono::system_clock::time_point timestamp;
    };

    class LogManager {
    public:
        // Singleton
        static LogManager& instance();

        // Append a log line
        void append(const std::string& id, LogLevel level, const std::string& message);
        void append(const std::string& id, const LogMessage& logMsg);

        // Flush synchronously (optionnel)
        void flushNow();

        // Shutdown propre du manager
        void shutdown();

        // Deleted copy
        LogManager(const LogManager&) = delete;
        LogManager& operator=(const LogManager&) = delete;

        ~LogManager();

    private:
        LogManager();

        // Worker thread
        void workerLoop();
        void notify_flush();
        bool WriteLogs(const std::string& id, const std::vector<LogMessage>& lines);

        // Helpers
        std::string formatLine(const std::string& id, LogLevel level, const std::string_view msg, bool console = false);
        std::string levelToString(LogLevel l);
        std::string currentTimestamp();
        std::string getTextColor(LogLevel l);

        // Members
        std::mutex mutex_;
        std::condition_variable cv_;
        std::unordered_map<std::string, std::vector<LogMessage>> buffers_;

        std::mutex fileSetMutex_;
        std::unordered_set<std::string> firstWriteDone_;

        std::mutex flushWaitMutex_;
        std::condition_variable flushWaitCv_;

        std::mutex ioMutex_;

        std::thread worker_;
        bool stopping_;

        // Config
        int flushIntervalMs_;
        size_t flushThreshold_;
    };

} // namespace Game::Logging
