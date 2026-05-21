#pragma once

#include <magic_enum/magic_enum.hpp>

#include <SFML/Window.hpp>
#include <Game/Events/InputActionSet.hpp>

enum class TrackToolAction : uint8_t {
    Apply,
    Cancel,
    COUNT
};

struct TrackInputs : Game::InputActionSet<TrackToolAction> {
    static inline std::array<Mapping, (size_t)TrackToolAction::COUNT> DefaultMap = { {
        { Enum::Apply,  sf::Mouse::Button::Left },
        { Enum::Cancel, sf::Mouse::Button::Right }
    } };
};
