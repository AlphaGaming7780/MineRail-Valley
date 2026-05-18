#pragma once
#include <atomic>

namespace pallas
{
    // --------------------------------------------
    // Token partagé, read-only (copiable)
    // --------------------------------------------
    class CancellationToken
    {
    public:
        bool IsCancellationRequested() const noexcept {
            return cancelled.load(std::memory_order_acquire);
        }

    private:
        friend class CancellationSource;
        std::atomic<bool> cancelled{ false };
    };

    // --------------------------------------------
    // Emetteur : permet d'annuler un token
    // --------------------------------------------
    class CancellationSource
    {
    public:
        CancellationToken GetToken() const noexcept { return token; }

        void Cancel() noexcept {
            token.cancelled.store(true, std::memory_order_release);
        }

    private:
        CancellationToken token;
    };
}