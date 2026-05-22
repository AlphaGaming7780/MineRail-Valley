#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>

#include <PallasEngine/Logging/PerformanceLogger.hpp>
#include <Game/Rendering/GameWindow.hpp>

namespace Game {

    GameWindow::GameWindow()
    {
    }

    // --- Création ---
    bool GameWindow::Create(
        const std::string& title,
        sf::State state,
        uint32_t styleFlagsWindowed,
        const sf::ContextSettings& settings
    )
    {
        m_title = title;
        m_settings = settings;
        m_state = state;
        m_styleWindowed = styleFlagsWindowed;

        // Init des modes
        m_windowedMode = sf::VideoMode::getDesktopMode();
        m_fullscreenMode = PickBestFullscreenMode();

        if (m_state == sf::State::Fullscreen) 
        {
            Recreate(m_fullscreenMode, sf::Style::Default);
        }
        else {
            Recreate(m_windowedMode, m_styleWindowed);
            CenterOnPrimary();
        }

        EventManager::Instance().Register(this);

        return m_window.setActive(true);
    }

    // --- Accès brut ---
    sf::RenderWindow& GameWindow::Get() { return m_window; }
    const sf::RenderWindow& GameWindow::Get() const { return m_window; }
    bool GameWindow::IsOpen() const { return m_window.isOpen(); }
    void GameWindow::Close() { m_window.close(); }

    // --- Propriétés & réglages ---
    void GameWindow::SetTitle(const std::string& title) {
        m_title = title;
        if (m_window.isOpen()) m_window.setTitle(m_title);
    }

    void GameWindow::SetIcon(const sf::Image& icon)
    {
        if (m_window.isOpen()) m_window.setIcon(icon);
    }

    void GameWindow::SetIcon(unsigned w, unsigned h, const uint8_t* rgbaPixels) {
        if (m_window.isOpen()) m_window.setIcon({ w, h }, rgbaPixels);
    }

    void GameWindow::SetVSync(bool enabled) {
        m_vsync = enabled;
        if (m_window.isOpen()) {
            m_window.setVerticalSyncEnabled(m_vsync);
            if (m_vsync) m_window.setFramerateLimit(0);
            else if (m_fpsLimit > 0) m_window.setFramerateLimit(m_fpsLimit);
        }
    }

    bool GameWindow::IsVSync() const {
        return m_vsync;
    }

    void GameWindow::SetFramerateLimit(unsigned limit) {
        m_fpsLimit = limit;
        if (m_window.isOpen() && !m_vsync) m_window.setFramerateLimit(m_fpsLimit);
    }

    // --- State (Windowed / Fullscreen) ---
    void GameWindow::SetState(sf::State s) {
        if (s == m_state) return;

        if (s == sf::State::Fullscreen) {
            SaveWindowedState();
            m_state = sf::State::Fullscreen;
            Recreate(m_fullscreenMode, sf::Style::Default);
        }
        else {
            m_state = sf::State::Windowed;
            Recreate(m_windowedMode, m_styleWindowed);
            RestoreWindowedStateOrCenter();
        }
    }

    void GameWindow::ToggleFullscreen() {
        SetState(m_state == sf::State::Windowed ? sf::State::Fullscreen : sf::State::Windowed);
    }

    void GameWindow::ToggleFullscreen(bool fullscreen)
    {
        if ((m_state == sf::State::Fullscreen && fullscreen) ||
            (m_state == sf::State::Windowed && !fullscreen)) return;
        SetState(fullscreen ? sf::State::Fullscreen : sf::State::Windowed);
    }

    bool GameWindow::IsFullscreen() const
    {
        return m_state == sf::State::Fullscreen;
    }

    sf::State GameWindow::GetState() const { return m_state; }

    // --- Style (fenêtré uniquement) ---
    void GameWindow::SetStyleFlags(uint32_t styleFlagsWindowed) {
        m_styleWindowed = styleFlagsWindowed;
        if (m_state == sf::State::Windowed) {
            Recreate(m_windowedMode, m_styleWindowed);
        }
    }

    // Acitive/désactive le style borderless
    void GameWindow::SetBorderless(bool enable) {
        if (enable) {
            SetStyleFlags(sf::Style::None);
        }
        else {
            SetStyleFlags(sf::Style::Default);
        }
    }

    bool GameWindow::IsWindowedBorderless() const {
        return (m_state == sf::State::Windowed) && (m_styleWindowed == sf::Style::None);
    }

    // Active le mode windwed (windowed, style None, taille desktop, pos 0,0)
    void GameWindow::SetWindowed(bool borderless) {
        m_state = sf::State::Windowed;
        m_styleWindowed = borderless ? sf::Style::None : sf::Style::Default;
        m_windowedMode = sf::VideoMode::getDesktopMode();
        Recreate(m_windowedMode, m_styleWindowed);
    }

    // --- Utilitaires rendu / boucle ---
    void GameWindow::Clear(const sf::Color& c) { m_window.clear(c); }
    void GameWindow::Display()
    {
#ifdef  DEBUG
        pallas::PerformanceLogger::Scoped perf("GameWindow.Display()");
#endif //  DEBUG
        m_window.display();
    }

    std::optional<sf::Event> GameWindow::PollEvent() { return m_window.pollEvent(); }

    // --- Taille / position (fenêtré) ---
    sf::Vector2u GameWindow::GetSize() const { return m_window.getSize(); }

    void GameWindow::SetSize(unsigned w, unsigned h) {
        if (m_state == sf::State::Windowed && !IsBorderlessWindowed()) {
            m_windowedMode.size = { w, h };
            Recreate(m_windowedMode, m_styleWindowed);
            CenterOnPrimary();
        }
    }

    sf::Vector2i GameWindow::GetPosition() const { return m_window.getPosition(); }

    void GameWindow::SetPosition(int x, int y) {
        if (m_state == sf::State::Windowed && !IsBorderlessWindowed()) {
            m_window.setPosition({ x, y });
            m_windowedPos = { x, y };
        }
    }

    void GameWindow::OnEvent(const sf::Event::Closed& event)
    {
        Close();
    }

    // Choix d’un bon mode fullscreen (souvent le 1er = meilleure définition/fréquence)
    sf::VideoMode GameWindow::PickBestFullscreenMode() {
        const auto& modes = sf::VideoMode::getFullscreenModes();
        if (!modes.empty()) return modes.front();
        return sf::VideoMode::getDesktopMode();
    }

    void GameWindow::Recreate(const sf::VideoMode& mode, uint32_t style) {
        if (m_window.isOpen()) m_window.close();
        m_window.create(mode, m_title.empty() ? "Game" : m_title, style, m_state, m_settings);
        // Réappliquer vsync/fps
        m_window.setVerticalSyncEnabled(m_vsync);
        if (!m_vsync && m_fpsLimit > 0) m_window.setFramerateLimit(m_fpsLimit);
    }

    void GameWindow::CenterOnPrimary() {
        auto desktop = sf::VideoMode::getDesktopMode();
        auto size = m_window.getSize();
        int x = static_cast<int>(desktop.size.x / 2 - size.x / 2);
        int y = static_cast<int>(desktop.size.y / 2 - size.y / 2);
        m_window.setPosition({ x, y });
        m_windowedPos = { x, y };
    }

    void GameWindow::SaveWindowedState() {
        if (m_state != sf::State::Windowed) return;
        auto sz = m_window.getSize();
        if (sz.x > 0 && sz.y > 0) m_windowedMode.size = sz;
        m_windowedPos = m_window.getPosition();
    }

    void GameWindow::RestoreWindowedStateOrCenter() {
        if (m_windowedPos.has_value()) {
            m_window.setPosition(*m_windowedPos);
        }
        else {
            CenterOnPrimary();
        }
    }

    //static uint32_t normalizeWindowedStyle(uint32_t flags) {
    //    // En fenêtré, si on te passe 0, on considère borderless (None).
    //    // Sinon on laisse les combinaisons SFML classiques (Titlebar|Resize|Close).
    //    if (flags == 0) return sf::Style::None;
    //    return flags;
    //}

    bool GameWindow::IsBorderlessWindowed() const {
        return (m_state == sf::State::Windowed) && (m_styleWindowed == sf::Style::None);
    }

} // namespace Windowing
