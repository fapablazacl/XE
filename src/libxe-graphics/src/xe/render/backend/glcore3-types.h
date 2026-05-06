
#pragma once

#include <cassert>
#include <utility>
#include <vector>

#include "glcore3-api.h"

namespace xe {
    inline RenderDeviceBackendContextGL *glctx(RenderDeviceBackendContext *ctx) {
        assert(ctx);
        return static_cast<RenderDeviceBackendContextGL *>(ctx);
    }

    // 16-bit index field in the handle bit layout; caps every pool at 65536 entries.
    constexpr uint32_t kHandleIndexLimit = 0x10000u;
    // 8-bit gen field in the handle bit layout; rolls over every 256 reuses per slot.
    constexpr uint32_t kHandleGenMask = 0xFFu;

    /**
     * @brief True if the pool has room for one more acquire (either unused capacity or a freed slot).
     *
     * Called by the public factories before acquireSlot; converts the hard 16-bit index cap into
     * a soft tl::expected error (BackendErrorCode::HandlePoolExhausted). Scan cost is O(n) only on
     * the overflow path, which is unreachable under normal usage.
     *
     * Slot-shape agnostic: works for any slot type exposing an `obj` field contextually convertible
     * to bool (Slot<T> via glaze::Unique, OptSlot<T> via std::optional).
     *
     * @tparam SlotT the concrete slot struct (Slot<T> or OptSlot<T>)
     * @param pool the pool to inspect; not mutated
     */
    template <class SlotT> bool poolHasCapacity(const std::vector<SlotT> &pool) {
        if (pool.size() < kHandleIndexLimit)
            return true;
        for (auto const &s : pool) {
            if (!s.obj)
                return true;
        }
        return false;
    }

    /**
     * @brief Reuse an empty slot (or append a new one) and return (index, gen).
     *
     * Linear-scans pool looking for a slot whose holder is empty - those are free slots awaiting
     * reuse. On hit, moves obj into that slot and bumps its gen counter. On miss, appends a new
     * slot. Scan cost is O(n); resource creation is not on the frame hot path so this is
     * intentional.
     *
     * Callers must ensure poolHasCapacity(pool) is true before calling; the tail assert is a
     * defensive invariant, not a user-facing failure path.
     *
     * Slot-shape agnostic: accepts any slot type whose `obj` field is assignable from ObjT and
     * contextually convertible to bool (Slot<T> holding glaze::Unique<T>, or OptSlot<T> holding
     * std::optional<T>).
     *
     * @tparam SlotT the concrete slot struct (Slot<T> or OptSlot<T>)
     * @tparam ObjT type of the resource holder being installed (deduced)
     * @param pool the pool vector to acquire a slot in; grows if no free slot is available
     * @param obj resource holder to install at the acquired slot
     * @return {index, gen} pair ready to be passed to HandleT::make
     */
    template <class SlotT, class ObjT> std::pair<uint32_t, uint8_t> acquireSlot(std::vector<SlotT> &pool, ObjT &&obj) {
        for (uint32_t i = 0; i < pool.size(); ++i) {
            if (!pool[i].obj) {
                pool[i].obj = std::forward<ObjT>(obj);
                pool[i].gen = static_cast<uint8_t>((pool[i].gen + 1) & kHandleGenMask);
                return {i, pool[i].gen};
            }
        }
        assert(pool.size() < kHandleIndexLimit && "acquireSlot: pool exhausted - caller skipped poolHasCapacity");
        pool.push_back({std::forward<ObjT>(obj), 0});
        return {static_cast<uint32_t>(pool.size() - 1), 0};
    }
} // namespace xe
