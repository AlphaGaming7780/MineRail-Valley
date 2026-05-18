#pragma once
#include <tuple>

namespace pallas {

    // AFTER
    template<typename... T>
    struct AfterDependencies {
        using types = std::tuple<T...>;
    };

    // BEFORE
    template<typename... T>
    struct BeforeDependencies {
        using types = std::tuple<T...>;
    };

}

// Macros façon C# attributes
#define UPDATE_AFTER(...) \
    using After = pallas::AfterDependencies<__VA_ARGS__>;

#define UPDATE_BEFORE(...) \
    using Before = pallas::BeforeDependencies<__VA_ARGS__>;