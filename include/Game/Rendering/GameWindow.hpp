#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include <PallasEngine/Logging/PerformanceLogger.hpp>

#include <Game/Events.h>


namespace Game {

    class GameWindow final : public IEventObserver<sf::Event::Closed>
    {
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
        );

        // --- Accès brut ---
        sf::RenderWindow& Get();
        const sf::RenderWindow& Get() const;
        bool IsOpen() const;
        void Close();

        // --- Propriétés & réglages ---
        void SetTitle(const std::string& title);

        void SetIcon(const sf::Image& icon);

        void SetIcon(unsigned w, unsigned h, const uint8_t* rgbaPixels);

        void SetVSync(bool enabled);

        void SetFramerateLimit(unsigned limit);

        // --- State (Windowed / Fullscreen) ---
        void SetState(sf::State s);

        void ToggleFullscreen();

        sf::State GetState() const;

        // --- Style (fenêtré uniquement) ---
        void SetStyleFlags(uint32_t styleFlagsWindowed);

        // Acitive/désactive le style borderless
        void SetBorderless(bool enable);

        bool IsWindowedBorderless() const;

        // Active le mode windwed (windowed, style None, taille desktop, pos 0,0)
        void SetWindowed(bool borderless = false);

        // --- Utilitaires rendu / boucle ---
        void Clear(const sf::Color& c = sf::Color::Black);
        void Display();

        std::optional<sf::Event> PollEvent();

        template <typename TDrawable, typename... TArgs>
        void Draw(const TDrawable& d, TArgs&&... args) {
            m_window.draw(d, std::forward<TArgs>(args)...);
        }

        // --- Taille / position (fenêtré) ---
        sf::Vector2u GetSize() const;

        void SetSize(unsigned w, unsigned h);

        sf::Vector2i GetPosition() const;

        void SetPosition(int x, int y);

        void OnEvent(const sf::Event::Closed& event);

    private:
        GameWindow();
        ~GameWindow() = default;

        // Choix d’un bon mode fullscreen (souvent le 1er = meilleure définition/fréquence)
        static sf::VideoMode PickBestFullscreenMode();

        void Recreate(const sf::VideoMode& mode, uint32_t style);

        void CenterOnPrimary();

        void SaveWindowedState();

        void RestoreWindowedStateOrCenter();

        bool IsBorderlessWindowed() const;

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
