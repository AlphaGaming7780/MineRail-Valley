#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include <unordered_map>
#include <typeindex>
#include <vector>
#include <string>
#include <cassert>

#include <Game/Rendering/GameWindow.hpp>
#include <Game/Events/EventManager.hpp>
#include <PallasEngine/Logging/Logger.hpp>
#include <Game/UI/UIManager.hpp>

// A Séparé en un fichier HPP et CPP

namespace Game {

    static constexpr sf::Mouse::Button UnknownMouse =
        static_cast<sf::Mouse::Button>(-1);

    static constexpr sf::Mouse::Wheel UnknownWheel =
		static_cast<sf::Mouse::Wheel>(-1);

    struct InputBindingState {
        bool pressed = false;
        bool justPressed = false;
        bool justReleased = false;

        sf::Vector2i screenPos = { 0,0 };

        // Wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets. 
		float wheelDelta = 0.f;

        // Pour affichage UI / rebinding
        sf::Keyboard::Scan scan = sf::Keyboard::Scan::Unknown;
        sf::Mouse::Button mouse = UnknownMouse; // "null"
        sf::Mouse::Wheel  wheel = UnknownWheel; // "null"
    };

    struct ActionBlock {
        std::vector<InputBindingState> states;
        bool enabled = false;
        std::string enumName; // pour export/import 
    };

    class InputManager : 
        public IEventObserver<sf::Event::KeyPressed>,
        public IEventObserver<sf::Event::KeyReleased>,
        public IEventObserver<sf::Event::MouseMoved>,
        public IEventObserver<sf::Event::MouseButtonPressed>,
        public IEventObserver<sf::Event::MouseButtonReleased>,
        public IEventObserver<sf::Event::MouseWheelScrolled>
    {
    public:
        static InputManager& Instance() {
            static InputManager inst;
            return inst;
        }

        ~InputManager();

        InputManager(const InputManager&) = delete;
        InputManager& operator=(const InputManager&) = delete;

        // ------------------------------------------------------------
        // 1) Enregistrement d’un enum d’actions
        // ------------------------------------------------------------
        template<typename TActionSet>
        void Register()
        {
            using Enum = typename TActionSet::Enum;

            static_assert(std::is_enum_v<Enum>, "TActionSet::Enum must be an enum");

            std::type_index key = typeid(Enum);

            // Déjà enregistré ?
            if (blocks.contains(key))
            {
                m_Logger.WarnO("ActionSet already registered: ", TActionSet::GetName());
                return;
            }

            // 1) Créer le bloc
            ActionBlock block;
            block.enabled = false;
            block.enumName = std::string(TActionSet::GetName());

            // 2) Allouer les states
            block.states.resize(TActionSet::Count());

            // 3) Appliquer le mapping par défaut
            for (const auto& entry : TActionSet::DefaultMap)
            {
                uint32_t index = static_cast<uint32_t>(entry.action);
                block.states[index].scan = entry.scan;
                block.states[index].mouse = entry.mouse;
				block.states[index].wheel = entry.wheel;
            }

            // 4) Stocker
            blocks.emplace(key, std::move(block));
        }


        // ------------------------------------------------------------
        // 2) Activation et désactivation des bindings
        // ------------------------------------------------------------
        template<typename TEnum>
        void EnableBlock(bool enable = true)
        {
            static_assert(std::is_enum_v<TEnum>, "TEnum must be an enum");

            std::type_index key = typeid(TEnum);

            auto it = blocks.find(typeid(TEnum));
            assert(it != blocks.end() && "Enum not registered");

            auto& block = it->second;

			if (block.enabled == enable)
                return;

            block.enabled = enable;

            for (auto& st : block.states) {
                st.pressed = false;
                st.justPressed = false;
                st.justReleased = false;
            }
        }

        template<typename TEnum>
        bool IsBlockEnabled() const {
            auto it = blocks.find(typeid(TEnum));
            assert(it != blocks.end());
            return it->second.enabled;
        }

        void DisableAllBlock();
        void EnableKeyboardInputs(bool enable = true);
        void EnableMouseInputs(bool enable = true);


        // ------------------------------------------------------------
        // 3) Accès direct à un binding (référence stable)
        // ------------------------------------------------------------
        template<typename TEnum>
        const InputBindingState& GetBinding(TEnum action) const {
            static_assert(std::is_enum_v<TEnum>, "TEnum must be an enum");

            auto it = blocks.find(typeid(TEnum));
            assert(it != blocks.end() && "Enum not registered");

            const auto& block = it->second;
            uint32_t index = static_cast<uint32_t>(action);

            assert(index < block.states.size() && "Invalid action index");

            return block.states[index];
        }

        template<typename TEnum>
        InputBindingState& GetBinding(TEnum action) {
            return const_cast<InputBindingState&>(
                static_cast<const InputManager*>(this)->GetBinding(action)
                );
        }

        // ------------------------------------------------------------
        // 4) Rebinding
        // ------------------------------------------------------------
        template<typename TEnum>
        void BindKey(TEnum action, sf::Keyboard::Scan scan) {
            InputBindingState& st = GetBinding(action);
            st.scan = scan;
        }

        template<typename TEnum>
        void BindMouse(TEnum action, sf::Mouse::Button btn) {
            InputBindingState& st = GetBinding(action);
            st.mouse = btn;
        }

        sf::Vector2i GetMouseScreenPos() const { return m_MousePosScreen; }
        sf::Vector2f GetMouseWorldPos() const { return m_MousePosWorld; }
        sf::Vector2f GetMouseUIPos() const { return m_MousePosUI; }


        //
        // Event
        //

        void OnEvent(const sf::Event::KeyPressed& ev)           { onKeyPressed(ev.scancode); }
        void OnEvent(const sf::Event::KeyReleased& ev)          { onKeyReleased(ev.scancode); }
        void OnEvent(const sf::Event::MouseMoved& ev)           { onMouseMoved(ev); }
        void OnEvent(const sf::Event::MouseButtonPressed& ev)   { onMousePressed(ev); }
        void OnEvent(const sf::Event::MouseButtonReleased& ev)  { onMouseReleased(ev); }
        void OnEvent(const sf::Event::MouseWheelScrolled& ev)   { onMouseWheelMoved(ev); }


    private:
        InputManager() 
        {
            GameWindow::Instance().Get().setKeyRepeatEnabled(false);

            auto& em = EventManager::Instance();
            em.Register<sf::Event::KeyPressed>(this);
            em.Register<sf::Event::KeyReleased>(this);
            em.Register<sf::Event::MouseMoved>(this);
            em.Register<sf::Event::MouseButtonPressed>(this);
            em.Register<sf::Event::MouseButtonReleased>(this);
            em.Register<sf::Event::MouseWheelScrolled>(this);
        }


        // ------------------------------------------------------------
        // 5) Reset des edges (à appeler 1x par frame)
        // ------------------------------------------------------------
        friend class GameManager;
        void ResetEdges();
        void Update();
        void ForceMouseRelease(sf::Vector2i pos);

        // ------------------------------------------------------------
        // 6) Mise à jour par EventManager
        // ------------------------------------------------------------
        void onMouseMoved(const sf::Event::MouseMoved& ev);
        void onKeyPressed(const sf::Keyboard::Scan key);
        void onKeyReleased(const sf::Keyboard::Scan key);
        void onMousePressed(const sf::Event::MouseButtonPressed ev);
        void onMouseReleased(const sf::Event::MouseButtonReleased ev);
		void onMouseWheelMoved(const sf::Event::MouseWheelScrolled ev);

        pallas::Logger m_Logger = pallas::Logger("InputManager");

        bool m_EnableKeyboardInputs = true;
        bool m_EnableMouseInputs = true;

        sf::Vector2i m_MousePosScreen = { 0,0 };
        sf::Vector2f m_MousePosWorld = { 0,0 };
        sf::Vector2f m_MousePosUI = { 0,0 };

        std::unordered_map<std::type_index, ActionBlock> blocks;
        
    };

} // namespace pallas