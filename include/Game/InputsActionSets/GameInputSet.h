#pragma once

#include <magic_enum/magic_enum.hpp>

#include <SFML/Window.hpp>
#include <Game/Events/InputActionSet.hpp>

enum class GameAction : uint8_t {
    Pause,
    ESC,
    COUNT
};

struct GameInputs : Game::InputActionSet<GameAction> {
    static inline std::array<Mapping, (size_t)GameAction::COUNT> DefaultMap = { {
        { Enum::Pause,  sf::Keyboard::Scan::Space },
        { Enum::ESC,    sf::Keyboard::Scan::Escape }
    } };
};
