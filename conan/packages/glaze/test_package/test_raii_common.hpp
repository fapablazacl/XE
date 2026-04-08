// Shared RAII test logic for the glaze C++ smart-pointer wrappers.
//
// This header is included by per-API thin wrappers (test_gl_raii.cpp,
// test_gles1_raii.cpp, ...). Each wrapper defines GLAZE_API_NS to the
// target namespace before including this header. The tests exercise
// Unique / Shared / Weak with a fake Handle + Deleter so no real GL
// context is required: nothing here calls into the C function pointers.

#ifndef GLAZE_API_NS
#error "GLAZE_API_NS must be defined before including test_raii_common.hpp"
#endif

#include <utility>

#define TEST_REQUIRE(cond)                                                                                                                                                         \
    do {                                                                                                                                                                           \
        if (!(cond)) {                                                                                                                                                             \
            return __LINE__;                                                                                                                                                       \
        }                                                                                                                                                                          \
    } while (0)

namespace test_raii_detail {

inline int &destroy_count() {
    static int n = 0;
    return n;
}

// Minimal stand-in for a glaze handle: trivially copyable, default-null,
// explicit bool conversion, and equality. Matches the contract that
// Unique / Shared / Weak rely on.
struct FakeHandle {
    unsigned int id;
    FakeHandle() : id(0) {}
    explicit FakeHandle(unsigned int v) : id(v) {}
    explicit operator bool() const { return id != 0; }
    bool operator==(FakeHandle o) const { return id == o.id; }
    bool operator!=(FakeHandle o) const { return id != o.id; }
};

// Stateless deleter that just bumps a counter — lets us verify that the
// underlying resource is released exactly once.
struct FakeDeleter {
    void operator()(FakeHandle h) const {
        (void)h;
        ++destroy_count();
    }
};

} // namespace test_raii_detail

inline int run_raii_tests() {
    using test_raii_detail::FakeDeleter;
    using test_raii_detail::FakeHandle;
    using test_raii_detail::destroy_count;

    using U = ::GLAZE_API_NS::raii::Unique<FakeHandle, FakeDeleter>;
    using S = ::GLAZE_API_NS::raii::Shared<FakeHandle, FakeDeleter>;
    using W = ::GLAZE_API_NS::raii::Weak<FakeHandle, FakeDeleter>;

    // ── Unique ──────────────────────────────────────────────────────────────

    // Default-constructed Unique is empty and never invokes the deleter.
    destroy_count() = 0;
    {
        U u;
        TEST_REQUIRE(!u);
        TEST_REQUIRE(u.get().id == 0);
    }
    TEST_REQUIRE(destroy_count() == 0);

    // Owning Unique releases its handle once on destruction.
    destroy_count() = 0;
    {
        U u(FakeHandle(42));
        TEST_REQUIRE(static_cast<bool>(u));
        TEST_REQUIRE(u.get().id == 42);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Move construction transfers ownership and leaves the source empty.
    destroy_count() = 0;
    {
        U a(FakeHandle(7));
        U b(std::move(a));
        TEST_REQUIRE(!a);
        TEST_REQUIRE(static_cast<bool>(b));
        TEST_REQUIRE(b.get().id == 7);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Move assignment destroys the previously-held handle first.
    destroy_count() = 0;
    {
        U a(FakeHandle(1));
        U b(FakeHandle(2));
        a = std::move(b);
        TEST_REQUIRE(destroy_count() == 1);
        TEST_REQUIRE(a.get().id == 2);
        TEST_REQUIRE(!b);
    }
    TEST_REQUIRE(destroy_count() == 2);

    // release() yields the raw handle without invoking the deleter.
    destroy_count() = 0;
    {
        U u(FakeHandle(99));
        FakeHandle raw = u.release();
        TEST_REQUIRE(raw.id == 99);
        TEST_REQUIRE(!u);
    }
    TEST_REQUIRE(destroy_count() == 0);

    // reset() destroys the current handle and adopts the new one (if any).
    destroy_count() = 0;
    {
        U u(FakeHandle(10));
        u.reset(FakeHandle(20));
        TEST_REQUIRE(destroy_count() == 1);
        TEST_REQUIRE(u.get().id == 20);
        u.reset();
        TEST_REQUIRE(destroy_count() == 2);
        TEST_REQUIRE(!u);
    }
    TEST_REQUIRE(destroy_count() == 2);

    // swap() exchanges ownership without invoking the deleter.
    destroy_count() = 0;
    {
        U a(FakeHandle(1));
        U b(FakeHandle(2));
        a.swap(b);
        TEST_REQUIRE(a.get().id == 2);
        TEST_REQUIRE(b.get().id == 1);
        TEST_REQUIRE(destroy_count() == 0);
    }
    TEST_REQUIRE(destroy_count() == 2);

    // Self move-assignment is a no-op.
    destroy_count() = 0;
    {
        U u(FakeHandle(5));
        U &ref = u;
        u = std::move(ref);
        TEST_REQUIRE(static_cast<bool>(u));
        TEST_REQUIRE(u.get().id == 5);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // ── Shared ──────────────────────────────────────────────────────────────

    // Default-constructed Shared is empty and allocates no control block.
    destroy_count() = 0;
    {
        S s;
        TEST_REQUIRE(!s);
        TEST_REQUIRE(s.use_count() == 0);
    }
    TEST_REQUIRE(destroy_count() == 0);

    // Constructing from a null handle should not allocate or destroy.
    destroy_count() = 0;
    {
        S s{FakeHandle()};
        TEST_REQUIRE(!s);
        TEST_REQUIRE(s.use_count() == 0);
    }
    TEST_REQUIRE(destroy_count() == 0);

    // Copy construction shares the resource; deletion happens on the last copy.
    destroy_count() = 0;
    {
        S a(FakeHandle(5));
        TEST_REQUIRE(a.use_count() == 1);
        {
            S b = a;
            TEST_REQUIRE(a.use_count() == 2);
            TEST_REQUIRE(b.use_count() == 2);
            TEST_REQUIRE(b.get().id == 5);
        }
        TEST_REQUIRE(a.use_count() == 1);
        TEST_REQUIRE(destroy_count() == 0);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Move construction transfers the control block.
    destroy_count() = 0;
    {
        S a(FakeHandle(8));
        S b(std::move(a));
        TEST_REQUIRE(!a);
        TEST_REQUIRE(b.use_count() == 1);
        TEST_REQUIRE(b.get().id == 8);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Copy assignment releases the previous resource before adopting the new one.
    destroy_count() = 0;
    {
        S a(FakeHandle(1));
        S b(FakeHandle(2));
        a = b;
        TEST_REQUIRE(destroy_count() == 1);
        TEST_REQUIRE(a.get().id == 2);
        TEST_REQUIRE(a.use_count() == 2);
    }
    TEST_REQUIRE(destroy_count() == 2);

    // reset() drops the strong reference and triggers deletion when last.
    destroy_count() = 0;
    {
        S a(FakeHandle(3));
        a.reset();
        TEST_REQUIRE(destroy_count() == 1);
        TEST_REQUIRE(!a);
        TEST_REQUIRE(a.use_count() == 0);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // swap() exchanges the control blocks of two Shared instances.
    destroy_count() = 0;
    {
        S a(FakeHandle(1));
        S b(FakeHandle(2));
        a.swap(b);
        TEST_REQUIRE(a.get().id == 2);
        TEST_REQUIRE(b.get().id == 1);
    }
    TEST_REQUIRE(destroy_count() == 2);

    // ── Weak ────────────────────────────────────────────────────────────────

    // Default-constructed Weak is expired and locks to an empty Shared.
    {
        W w;
        TEST_REQUIRE(w.expired());
        TEST_REQUIRE(w.use_count() == 0);
        S locked = w.lock();
        TEST_REQUIRE(!locked);
    }

    // Weak constructed from a live Shared can promote back via lock().
    destroy_count() = 0;
    {
        S s(FakeHandle(11));
        W w(s);
        TEST_REQUIRE(!w.expired());
        TEST_REQUIRE(w.use_count() == 1);
        S locked = w.lock();
        TEST_REQUIRE(static_cast<bool>(locked));
        TEST_REQUIRE(locked.get().id == 11);
        TEST_REQUIRE(s.use_count() == 2);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Weak outlives the underlying resource: control block survives, the
    // resource is released, and lock() returns empty afterwards.
    destroy_count() = 0;
    {
        W w;
        {
            S s(FakeHandle(99));
            w = s;
            TEST_REQUIRE(!w.expired());
        }
        TEST_REQUIRE(destroy_count() == 1);
        TEST_REQUIRE(w.expired());
        S locked = w.lock();
        TEST_REQUIRE(!locked);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Copying a Weak observer does not affect the strong count.
    destroy_count() = 0;
    {
        S s(FakeHandle(4));
        W w1(s);
        W w2 = w1;
        TEST_REQUIRE(!w1.expired());
        TEST_REQUIRE(!w2.expired());
        TEST_REQUIRE(w2.use_count() == 1);
        TEST_REQUIRE(s.use_count() == 1);
    }
    TEST_REQUIRE(destroy_count() == 1);

    // Moving a Weak observer empties the source.
    destroy_count() = 0;
    {
        S s(FakeHandle(4));
        W w1(s);
        W w2 = std::move(w1);
        TEST_REQUIRE(w1.expired());
        TEST_REQUIRE(!w2.expired());
    }
    TEST_REQUIRE(destroy_count() == 1);

    // swap() between Weak observers exchanges their control blocks.
    destroy_count() = 0;
    {
        S s1(FakeHandle(1));
        S s2(FakeHandle(2));
        W w1(s1);
        W w2(s2);
        w1.swap(w2);
        TEST_REQUIRE(w1.lock().get().id == 2);
        TEST_REQUIRE(w2.lock().get().id == 1);
    }
    TEST_REQUIRE(destroy_count() == 2);

    return 0;
}

#undef TEST_REQUIRE
