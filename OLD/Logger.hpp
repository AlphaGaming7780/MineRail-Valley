// Logger.hpp
#pragma once

#include <string>
#include "LogManager.hpp"

namespace Game::Logging {

    // Logger class
    class Logger {
    public:
        explicit Logger(std::string id)
            : id_(std::move(id))
        {
            if (id_.empty()) id_ = "default";
        }

        // copyable (shares same id)
        Logger(const Logger&) = default;
        Logger& operator=(const Logger&) = default;

        void Info(const std::string& msg) const {
            LogManager::instance().append(id_, LogLevel::Info, msg);
        }
        void Warn(const std::string& msg) const {
            LogManager::instance().append(id_, LogLevel::Warn, msg);
        }
        void Error(const std::string& msg) const {
            LogManager::instance().append(id_, LogLevel::Error, msg);
        }

        // formatted versions (variadic simple wrapper)
        template<typename... Args>
        void InfoF(const char* fmt, Args... args) const { Info(format(fmt, args...)); }
        template<typename... Args>
        void WarnF(const char* fmt, Args... args) const { Warn(format(fmt, args...)); }
        template<typename... Args>
        void ErrorF(const char* fmt, Args... args) const { Error(format(fmt, args...)); }

        const std::string& id() const { return id_; }

    private:
        std::string id_;

        // simple sprintf-like helper (not the most safe but convenient)
        template<typename... Args>
        static std::string format(const char* fmt, Args... args) {
            int size_s = std::snprintf(nullptr, 0, fmt, args...) + 1;
            if (size_s <= 0) return "";
            auto size = static_cast<size_t>(size_s);
            std::unique_ptr<char[]> buf(new char[size]);
            std::snprintf(buf.get(), size, fmt, args...);
            return std::string(buf.get(), buf.get() + size - 1);
        }
    };
}