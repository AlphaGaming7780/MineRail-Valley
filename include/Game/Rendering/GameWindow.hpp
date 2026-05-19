#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include <PallasEngine/Logging/PerformanceLogger.hpp>

/*

    NEED REWORK and improvment, maybe add a Camera concept instead of sf::view


*/


namespace Game {

    class GameWindow final {
    public:
        static GameWindow& Instance() {
            static GameWindow inst;
            return inst;
        }

        GameWindow(const GameWindow&) = delete;
        GameWindow& operator=(const GameWindow&) = delete;

        // --- Création ---
        bool Create(
            const std::string& title,
            sf::State state = sf::State::Windowed,
            uint32_t styleFlagsWindowed = sf::Style::Default,
            const sf::ContextSettings& settings = sf::ContextSettings()
        )
        {
            m_title = title;
            m_settings = settings;
            m_state = state;
            m_styleWindowed = styleFlagsWindowed;

            // Init des modes
            m_windowedMode = sf::VideoMode::getDesktopMode();
            m_fullscreenMode = pickBestFullscreenMode();

            if (m_state == sf::State::Fullscreen) 
            {
                recreate(m_fullscreenMode, sf::Style::Default);
            }
            else {
                recreate(m_windowedMode, m_styleWindowed);
                centerOnPrimary();
            }
            return m_window.setActive(true);
        }

        // --- Accès brut ---
        sf::RenderWindow& Get() { return m_window; }
        const sf::RenderWindow& Get() const { return m_window; }
        bool IsOpen() const { return m_window.isOpen(); }
        void Close() { m_window.close(); }

        // --- Propriétés & réglages ---
        void SetTitle(const std::string& title) {
            m_title = title;
            if (m_window.isOpen()) m_window.setTitle(m_title);
        }

        void SetIcon(const sf::Image& icon)
        {
            if (m_window.isOpen()) m_window.setIcon(icon);
        }

        void SetIcon(unsigned w, unsigned h, const uint8_t* rgbaPixels) {
            if (m_window.isOpen()) m_window.setIcon({ w, h }, rgbaPixels);
        }

        void SetVSync(bool enabled) {
            m_vsync = enabled;
            if (m_window.isOpen()) {
                m_window.setVerticalSyncEnabled(m_vsync);
                if (m_vsync) m_window.setFramerateLimit(0);
                else if (m_fpsLimit > 0) m_window.setFramerateLimit(m_fpsLimit);
            }
        }

        void SetFramerateLimit(unsigned limit) {
            m_fpsLimit = limit;
            if (m_window.isOpen() && !m_vsync) m_window.setFramerateLimit(m_fpsLimit);
        }

        // --- State (Windowed / Fullscreen) ---
        void SetState(sf::State s) {
            if (s == m_state) return;

            if (s == sf::State::Fullscreen) {
                saveWindowedState();
                m_state = sf::State::Fullscreen;
                recreate(m_fullscreenMode, sf::Style::Default);
            }
            else {
                m_state = sf::State::Windowed;
                recreate(m_windowedMode, m_styleWindowed);
                restoreWindowedStateOrCenter();
            }
        }

        void ToggleFullscreen() {
            SetState(m_state == sf::State::Windowed ? sf::State::Fullscreen : sf::State::Windowed);
        }

        sf::State GetState() const { return m_state; }

        // --- Style (fenêtré uniquement) ---
        void SetStyleFlags(uint32_t styleFlagsWindowed) {
            m_styleWindowed = styleFlagsWindowed;
            if (m_state == sf::State::Windowed) {
                recreate(m_windowedMode, m_styleWindowed);
            }
        }

        // Acitive/désactive le style borderless
        void SetBorderless(bool enable) {
            if (enable) {
                SetStyleFlags(sf::Style::None);
            }
            else {
                SetStyleFlags(sf::Style::Default);
            }
        }

        bool IsWindowedBorderless() const {
            return (m_state == sf::State::Windowed) && (m_styleWindowed == sf::Style::None);
        }

        // Active le mode windwed (windowed, style None, taille desktop, pos 0,0)
        void SetWindowed(bool borderless = false) {
            m_state = sf::State::Windowed;
            m_styleWindowed = borderless ? sf::Style::None : sf::Style::Default;
            m_windowedMode = sf::VideoMode::getDesktopMode();
            recreate(m_windowedMode, m_styleWindowed);
        }

        // --- Utilitaires rendu / boucle ---
        void Clear(const sf::Color& c = sf::Color::Black) { m_window.clear(c); }
        void Display() 
        {
#ifdef  DEBUG
            PerformanceLogger::Scoped perf("GameWindow.Display()");
#endif //  DEBUG
            m_window.display(); 
        }

        std::optional<sf::Event> PollEvent() { return m_window.pollEvent(); }

        template <typename TDrawable, typename... TArgs>
        void Draw(const TDrawable& d, TArgs&&... args) {
            m_window.draw(d, std::forward<TArgs>(args)...);
        }

        // --- Taille / position (fenêtré) ---
        sf::Vector2u GetSize() const { return m_window.getSize(); }

        void SetSize(unsigned w, unsigned h) {
            if (m_state == sf::State::Windowed && !isBorderlessWindowed()) {
                m_windowedMode.size = { w, h };
                recreate(m_windowedMode, m_styleWindowed);
                centerOnPrimary();
            }
        }

        sf::Vector2i GetPosition() const { return m_window.getPosition(); }

        void SetPosition(int x, int y) {
            if (m_state == sf::State::Windowed && !isBorderlessWindowed()) {
                m_window.setPosition({ x, y });
                m_windowedPos = { x, y };
            }
        }

    private:
        GameWindow() = default;
        ~GameWindow() = default;

        // Choix d’un bon mode fullscreen (souvent le 1er = meilleure définition/fréquence)
        static sf::VideoMode pickBestFullscreenMode() {
            const auto& modes = sf::VideoMode::getFullscreenModes();
            if (!modes.empty()) return modes.front();
            return sf::VideoMode::getDesktopMode();
        }

        void recreate(const sf::VideoMode& mode, uint32_t style) {
            if (m_window.isOpen()) m_window.close();
            m_window.create(mode, m_title.empty() ? "Game" : m_title, style, m_state, m_settings);
            // Réappliquer vsync/fps
            m_window.setVerticalSyncEnabled(m_vsync);
            if (!m_vsync && m_fpsLimit > 0) m_window.setFramerateLimit(m_fpsLimit);
        }

        void centerOnPrimary() {
            auto desktop = sf::VideoMode::getDesktopMode();
            auto size = m_window.getSize();
            int x = static_cast<int>(desktop.size.x / 2 - size.x / 2);
            int y = static_cast<int>(desktop.size.y / 2 - size.y / 2);
            m_window.setPosition({ x, y });
            m_windowedPos = { x, y };
        }

        void saveWindowedState() {
            if (m_state != sf::State::Windowed) return;
            auto sz = m_window.getSize();
            if (sz.x > 0 && sz.y > 0) m_windowedMode.size = sz;
            m_windowedPos = m_window.getPosition();
        }

        void restoreWindowedStateOrCenter() {
            if (m_windowedPos.has_value()) {
                m_window.setPosition(*m_windowedPos);
            }
            else {
                centerOnPrimary();
            }
        }

        //static uint32_t normalizeWindowedStyle(uint32_t flags) {
        //    // En fenêtré, si on te passe 0, on considère borderless (None).
        //    // Sinon on laisse les combinaisons SFML classiques (Titlebar|Resize|Close).
        //    if (flags == 0) return sf::Style::None;
        //    return flags;
        //}

        bool isBorderlessWindowed() const {
            return (m_state == sf::State::Windowed) && (m_styleWindowed == sf::Style::None);
        }

    private:
        sf::RenderWindow            m_window;
        std::string                 m_title;
        sf::ContextSettings         m_settings{};

        // Timing
        bool                        m_vsync = false;
        unsigned                    m_fpsLimit = 0;

        // État et styles
        sf::State                   m_state = sf::State::Windowed;
        uint32_t                    m_styleWindowed = sf::Style::Default;

        // Modes
        sf::VideoMode               m_windowedMode{};
        sf::VideoMode               m_fullscreenMode{};

        // Position fenêtrée à restaurer
        std::optional<sf::Vector2i> m_windowedPos;
    };

} // namespace Windowing
