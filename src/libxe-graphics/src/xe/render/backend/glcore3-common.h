
#pragma once

#include <optional>
#include <glaze/gl.hpp>
#include <glaze/raii.hpp>

namespace xe {
    /**
     * @brief A single entry in a backend resource pool.
     *
     * Pairs the RAII holder for the underlying GL object with an 8-bit generation counter.
     * A slot is considered free iff its RAII holder evaluates to false (the GL object has been
     * reset()); no separate liveness flag is needed. The generation is bumped by acquireSlot
     * when the slot is reused, so any stale handle that still references this index will fail
     * the gen() == slot.gen check enforced by the backend.
     *
     * @tparam T the underlying GL resource type (gl::BufferId, gl::Texture, gl::Program, ...)
     */
    template <class T> struct Slot {
        //! RAII-owned GL object. Empty (operator bool -> false) when the slot is free for reuse.
        glaze::Unique<T> obj;

        //! Authoritative generation counter. Bumped by acquireSlot when this slot is reused.
        uint8_t gen = 0;
    };

    /**
     * @brief A pool slot for non-GL resources (e.g. VertexLayoutGL) that live as plain C++ values.
     *
     * Shares the contract used by Slot<T>: the `obj` field evaluates to false iff the slot is free,
     * and the 8-bit generation counter is bumped on reuse so stale handles fail the gen check. Uses
     * std::optional as the liveness wrapper because glaze::Unique is specific to GL handle types.
     *
     * @tparam T the plain (non-GL) resource type held by the pool
     */
    template <class T> struct OptSlot {
        //! Engaged when the slot is live; empty after destroy, awaiting reuse.
        std::optional<T> obj;

        //! Authoritative generation counter. Bumped by acquireSlot when this slot is reused.
        uint8_t gen = 0;
    };

}
