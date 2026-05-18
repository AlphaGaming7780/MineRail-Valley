#pragma once

#include <PallasEngine/PallasEngine.hpp>

enum class CameraAction : uint8_t {
	MoveUp,
	MoveDown,
    MoveLeft,
    MoveRight,
    Zoom,
    COUNT
};

struct CameraInputs : pallas::InputActionSet<CameraAction> {
    static inline std::array<Mapping, (size_t)CameraAction::COUNT> DefaultMap = { {
        { Enum::MoveUp,  sf::Keyboard::Scan::W },
        { Enum::MoveDown, sf::Keyboard::Scan::S },
        { Enum::MoveLeft,     sf::Keyboard::Scan::A },
        { Enum::MoveRight,    sf::Keyboard::Scan::D },
        { Enum::Zoom,         sf::Mouse::Wheel::Vertical },
    } };
};
