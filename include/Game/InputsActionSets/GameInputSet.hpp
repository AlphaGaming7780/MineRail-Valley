#pragma once

#include <PallasEngine/PallasEngine.hpp>

enum class GameActions : uint8_t {
    MoveLeft,
    MoveRight,
    Jump,
    Shoot,
    COUNT
};

struct GameInputs : pallas::InputActionSet<GameActions> {
    static inline std::array<Mapping, (size_t)GameActions::COUNT> DefaultMap = { {
        { Enum::MoveLeft,  sf::Keyboard::Scan::A },
        { Enum::MoveRight, sf::Keyboard::Scan::D },
        { Enum::Jump,      sf::Keyboard::Scan::Space },
        { Enum::Shoot,     sf::Mouse::Button::Left }
    } };
};
