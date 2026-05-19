#pragma once

#include <sstream>
#include <string>
#include <memory>
#include <PallasEngine/Logging/LogManager.hpp>
#include <Game/Utils/Vector2.hpp>

namespace pallas {

    class Logger {
    public:
        explicit Logger(std::string id);

        // copyable (shares same id)
        Logger(const Logger&) = default;
        Logger& operator=(const Logger&) = default;

        // Log levels
        void Info(const std::string& msg) const;
        void Warn(const std::string& msg) const;
        void Error(const std::string& msg) const;

        // Formatted versions
        //template<typename... Args>
        //void InfoF(const char* fmt, Args... args) const;

        template<typename... Args>
        void InfoO(Args&&... args) const;

        template<typename... Args>
        void WarnO(Args&&... args) const;

        template<typename... Args>
        void ErrorO(Args&&... args) const;

        template<typename... Args>
        void InfoF(const char* fmt, Args... args) const;

        template<typename... Args>
        void WarnF(const char* fmt, Args... args) const;

        template<typename... Args>
        void ErrorF(const char* fmt, Args... args) const;

        const std::string& id() const { return id_; }

    private:
        std::string id_;

        // simple sprintf-like helper
        template<typename... Args>
        static std::string format(const char* fmt, Args... args);
    };

    /// @brief Log une série d'arguments en les concaténant via operator<<.
    /// @tparam Args Types des arguments à concaténer.
    /// @param args Les valeurs à écrire dans le flux.
    /// 
    /// Exemple :
    ///     logger.InfoO("HP=", hp, " / Max=", maxHp);
    template<typename... Args>
    void Logger::InfoO(Args&&... args) const {
        std::ostringstream oss;
        (void)(oss << ... << args);
        Info(oss.str());
    }

    /// @brief Log un avertissement en concaténant les arguments via operator<<.
    /// @tparam Args Types des arguments à concaténer.
    /// @param args Les valeurs à écrire dans le flux.
    template<typename... Args>
    void Logger::WarnO(Args&&... args) const {
        std::ostringstream oss;
        (void)(oss << ... << args);
        Warn(oss.str());
    }


    /// @brief Log une erreur en concaténant les arguments via operator<<.
    /// @tparam Args Types des arguments à concaténer.
    /// @param args Les valeurs à écrire dans le flux.
    template<typename... Args>
    void Logger::ErrorO(Args&&... args) const {
        std::ostringstream oss;
        (void)(oss << ... << args);
        Error(oss.str());
    }

    // --- Template definitions must stay in the header ---

    /// @brief Log une information formatée avec une chaîne de format.
    /// @tparam Args Types des arguments utilisés dans le format.
    /// @param fmt Chaîne de format (style printf).
    /// @param args Arguments à injecter dans le format.
    template<typename... Args>
    void Logger::InfoF(const char* fmt, Args... args) const { Info(format(fmt, args...)); }

    /// @brief Log un avertissement formaté.
    /// @tparam Args Types des arguments utilisés dans le format.
    /// @param fmt Chaîne de format.
    /// @param args Arguments à injecter dans le format.
    template<typename... Args>
    void Logger::WarnF(const char* fmt, Args... args) const { Warn(format(fmt, args...)); }

    /// @brief Log une erreur formatée.
    /// @tparam Args Types des arguments utilisés dans le format.
    /// @param fmt Chaîne de format.
    /// @param args Arguments à injecter dans le format.
    template<typename... Args>
    void Logger::ErrorF(const char* fmt, Args... args) const { Error(format(fmt, args...)); }

    template<typename... Args>
    std::string Logger::format(const char* fmt, Args... args) {
        int size_s = std::snprintf(nullptr, 0, fmt, args...) + 1;
        if (size_s <= 0) return "";
        auto size = static_cast<size_t>(size_s);
        auto buf = std::make_unique<char[]>(size);
        std::snprintf(buf.get(), size, fmt, args...);
        return std::string(buf.get(), buf.get() + size - 1);
    }

} // namespace Game::Logging
