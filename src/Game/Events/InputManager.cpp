#include <Game/Events/InputManager.hpp>

namespace Game
{
	InputManager::~InputManager()
    {
        auto& em = EventManager::Instance();
        em.UnRegister<sf::Event::KeyPressed>(this);
        em.UnRegister<sf::Event::KeyReleased>(this);
        em.UnRegister<sf::Event::MouseMoved>(this);
        em.UnRegister<sf::Event::MouseButtonPressed>(this);
        em.UnRegister<sf::Event::MouseButtonReleased>(this);
        em.UnRegister<sf::Event::MouseWheelScrolled>(this);
    }

    void InputManager::DisableAllBlock()
    {
        for (auto& [type, block] : blocks)
        {
            block.enabled = false;

            for (auto& st : block.states)
            {
                st.pressed = false;
                st.justPressed = false;
                st.justReleased = false;
            }
        }
    }

    void InputManager::EnableKeyboardInputs(bool enable)
    {
        m_EnableKeyboardInputs = enable;
        if (!enable)
        {
            for (auto& [type, block] : blocks)
            {
                if (!block.enabled) continue;

                for (auto& st : block.states)
                {
                    if (st.scan != sf::Keyboard::Scan::Unknown && st.pressed)
                    {
                        st.pressed = false;
                        st.justReleased = true;
                    }
                }
            }
        }
    }

    void InputManager::EnableMouseInputs(bool enable)
    {
        m_EnableMouseInputs = enable;
        if (!enable)
        {
            for (auto& [type, block] : blocks)
            {
                if (!block.enabled) continue;

                for (auto& st : block.states)
                {
                    if (st.mouse != UnknownMouse && st.pressed)
                    {
                        st.pressed = false;
                        st.justReleased = true;
                    }
                }
            }
        }
    }

    void InputManager::ResetEdges()
    {
        for (auto& [type, block] : blocks) {
            if (!block.enabled) continue;
            for (auto& st : block.states) {
                st.justPressed = false;
                st.justReleased = false;
				st.wheelDelta = 0.f;
            }
        }
    }

    void InputManager::Update()
    {
        // Conversion world
        auto& window = GameWindow::Instance().Get();
        m_MousePosWorld = window.mapPixelToCoords(m_MousePosScreen);

        // Conversion UI
        m_MousePosUI = UIManager::Instance().MapPixelToCoords(m_MousePosScreen);

        if (UIManager::Instance().IsMouseOverUI(m_MousePosUI))
        {
            InputManager::Instance().ForceMouseRelease(m_MousePosScreen);
        }
    }

    void InputManager::ForceMouseRelease(sf::Vector2i pos)
    {
        for (auto& [type, block] : blocks)
        {
            if (!block.enabled) continue;

            for (auto& st : block.states)
            {
                if (st.mouse != UnknownMouse && st.pressed)
                {
                    st.pressed = false;
                    st.justReleased = true;
                    st.screenPos = pos;
                }
            }
        }
    }

    void InputManager::onMouseMoved(const sf::Event::MouseMoved& ev)
    {
        m_MousePosScreen = ev.position;
    }

    void InputManager::onKeyPressed(const sf::Keyboard::Scan key)
    {
        if (!m_EnableKeyboardInputs) return;
        for (auto& [type, block] : blocks) {
            if (!block.enabled) continue;
            for (auto& st : block.states) {
                if (st.scan == key) {
                    if (!st.pressed)
                        st.justPressed = true;
                    st.pressed = true;
                }
            }
        }
    }

    void InputManager::onKeyReleased(const sf::Keyboard::Scan key)
    {
        if (!m_EnableKeyboardInputs) return;
        for (auto& [type, block] : blocks) {
            if (!block.enabled) continue;
            for (auto& st : block.states) {
                if (st.scan == key) {
                    st.pressed = false;
                    st.justReleased = true;
                }
            }
        }
    }

    void InputManager::onMousePressed(const sf::Event::MouseButtonPressed ev)
    {
        if (UIManager::Instance().IsMouseOverUI(ev.position) || !m_EnableMouseInputs) return;
        for (auto& [type, block] : blocks) {
            if (!block.enabled) continue;
            for (auto& st : block.states) {
                if (st.mouse == ev.button) {
                    if (!st.pressed)
                        st.justPressed = true;
                    st.pressed = true;
                    st.screenPos = ev.position;
                }
            }
        }
    }

    void InputManager::onMouseReleased(const sf::Event::MouseButtonReleased ev)
    {
        if (UIManager::Instance().IsMouseOverUI(ev.position) || !m_EnableMouseInputs) return;
        for (auto& [type, block] : blocks) {
            if (!block.enabled) continue;
            for (auto& st : block.states) {
                if (st.mouse == ev.button && st.pressed) {
                    st.pressed = false;
                    st.justReleased = true;
                    st.screenPos = ev.position;
                }
            }
        }
    }

    void InputManager::onMouseWheelMoved(const sf::Event::MouseWheelScrolled ev)
    {
        if (UIManager::Instance().IsMouseOverUI(ev.position) || !m_EnableMouseInputs) return;
        for (auto& [type, block] : blocks) {
            if (!block.enabled) continue;
            for (auto& st : block.states) {
                if (st.wheel == ev.wheel) {
					st.wheelDelta = ev.delta;
                    st.screenPos = ev.position;
                }
            }
        }
    }

}
