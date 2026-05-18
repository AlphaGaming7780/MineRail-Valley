#pragma once

namespace pallas {

    enum class UpdatePhase : int {
        Begin,
        Update,
        Physics,
        LateUpdate,
        UI,
        Rendering,
        End
    };
}