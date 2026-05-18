#include <PallasEngine/Logging/Logger.hpp>

namespace pallas {

    // --- Constructor ---
    Logger::Logger(std::string id)
        : id_(std::move(id))
    {
        if (id_.empty()) id_ = "default";
    }

    // --- Logging ---
    void Logger::Info(const std::string& msg) const {
        LogManager::instance().append(id_, LogLevel::Info, msg);
    }

    void Logger::Warn(const std::string& msg) const {
        LogManager::instance().append(id_, LogLevel::Warn, msg);
    }

    void Logger::Error(const std::string& msg) const {
        LogManager::instance().append(id_, LogLevel::Error, msg);
    }

} // namespace Game::Logging
