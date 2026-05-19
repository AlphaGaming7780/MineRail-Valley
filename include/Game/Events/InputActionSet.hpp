#pragma once
#include <magic_enum/magic_enum.hpp>
#include <string_view>
#include <vector>

#include <Game/Events/InputManager.hpp>

namespace Game {

    template<typename TEnum>
    struct InputActionSet {
        using Enum = TEnum;

        static constexpr std::string_view GetName() {
            return magic_enum::enum_type_name<TEnum>();
        }

        // ------------------------------------------------------------
        // 1. Récupérer le nom d'une action (enum -> string)
        // ------------------------------------------------------------
        static constexpr std::string_view GetActionName(TEnum action) {
            return magic_enum::enum_name(action);
        }

        // ------------------------------------------------------------
        // 2. Récupérer l'action depuis son nom (string -> enum)
        // ------------------------------------------------------------
        static constexpr std::optional<TEnum> GetActionFromName(std::string_view name) {
            return magic_enum::enum_cast<TEnum>(name);
        }

        // ------------------------------------------------------------
        // 3. Récupérer toutes les actions (tableau constexpr)
        // ------------------------------------------------------------
        static constexpr auto GetActions() {
            return magic_enum::enum_values<TEnum>();
        }

        // ------------------------------------------------------------
        // 4. Récupérer tous les noms (tableau constexpr)
        // ------------------------------------------------------------
        static constexpr auto GetActionNames() {
            return magic_enum::enum_names<TEnum>();
        }

        // ------------------------------------------------------------
        // 5. Nombre d'actions
        // ------------------------------------------------------------
        static constexpr std::size_t Count() {
            return magic_enum::enum_count<TEnum>();
        }

        // ------------------------------------------------------------
        // 6. Mapping par défaut (à redéfinir dans les classes dérivées)
        // ------------------------------------------------------------
        struct Mapping {

            TEnum action;
            sf::Keyboard::Scan scan = sf::Keyboard::Scan::Unknown;
            sf::Mouse::Button mouse = UnknownMouse;
			sf::Mouse::Wheel  wheel = UnknownWheel;

            // 0) Constructeur par défaut
            //constexpr Mapping() = default;

            // 1) Action + Scan
            constexpr Mapping(Enum a, sf::Keyboard::Scan s)
                : action(a), scan(s) {
            }

            // 2) Action + Mouse
            constexpr Mapping(Enum a, sf::Mouse::Button m)
                : action(a), mouse(m) {
            }

            // 3) Action + Mouse
            constexpr Mapping(Enum a, sf::Mouse::Wheel w)
                : action(a), wheel(w) {
            }

            // 3) Action + Scan + Mouse (optionnel)
            //constexpr Mapping(Enum a, sf::Keyboard::Scan s, sf::Mouse::Button m)
            //    : action(a), scan(s), mouse(m) {
            //}
        };

        //static constexpr std::string_view Name = "UnnamedActionSet";

        static inline std::array<Mapping, 0> DefaultMap = {};
    };

} // namespace pallas