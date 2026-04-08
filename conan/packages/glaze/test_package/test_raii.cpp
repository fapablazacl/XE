// Smoke test for the static glaze/raii.hpp smart-pointer templates.
//
// This test is api-agnostic: it uses a FakeWrapper value type with a custom
// glaze::Traits specialization, so it doesn't need a real GL context. It
// exercises Unique / Shared / Weak lifetime, ref-counting, operator->, and
// makeUnique / makeShared.
//
// Failures return the source line number so Release builds (where assert is
// a no-op) still produce useful exit codes.

#include <glaze/raii.hpp>

#include <utility>

namespace {
int g_destroyed = 0;

// A value type that mimics gl::dsa::Buffer: cheap, default-null, has methods.
struct FakeWrapper {
    unsigned int id;
    FakeWrapper() : id(0) {}
    explicit FakeWrapper(unsigned int v) : id(v) {}
    int poke() const { return static_cast<int>(id) * 2; }
};
} // namespace

namespace glaze {
template<> struct Traits<FakeWrapper> {
    static FakeWrapper create() { return FakeWrapper(1); }
    static void destroy(FakeWrapper &w) {
        ++g_destroyed;
        w.id = 0;
    }
    static bool valid(const FakeWrapper &w) { return w.id != 0; }
};
} // namespace glaze

#define TEST_REQUIRE(c)                                                                                                                                                            \
    do {                                                                                                                                                                           \
        if (!(c)) {                                                                                                                                                                \
            return __LINE__;                                                                                                                                                       \
        }                                                                                                                                                                          \
    } while (0)

int main() {
    using U = glaze::Unique<FakeWrapper>;
    using S = glaze::Shared<FakeWrapper>;
    using W = glaze::Weak<FakeWrapper>;

    // ── Unique ──────────────────────────────────────────────────────────────

    // Default Unique is empty, never destroys.
    g_destroyed = 0;
    {
        U u;
        TEST_REQUIRE(!u);
        TEST_REQUIRE(u.get().id == 0);
    }
    TEST_REQUIRE(g_destroyed == 0);

    // Owning Unique destroys exactly once; operator-> hits the value.
    g_destroyed = 0;
    {
        U u(FakeWrapper(7));
        TEST_REQUIRE(static_cast<bool>(u));
        TEST_REQUIRE(u->poke() == 14);
        TEST_REQUIRE((*u).id == 7);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Move construction transfers ownership and empties the source.
    g_destroyed = 0;
    {
        U a(FakeWrapper(7));
        U b(std::move(a));
        TEST_REQUIRE(!a);
        TEST_REQUIRE(static_cast<bool>(b));
        TEST_REQUIRE(b->poke() == 14);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Move assignment destroys the previously-held value first.
    g_destroyed = 0;
    {
        U a(FakeWrapper(1));
        U b(FakeWrapper(2));
        a = std::move(b);
        TEST_REQUIRE(g_destroyed == 1);
        TEST_REQUIRE(a->id == 2);
        TEST_REQUIRE(!b);
    }
    TEST_REQUIRE(g_destroyed == 2);

    // release() yields the value without invoking the deleter.
    g_destroyed = 0;
    {
        U u(FakeWrapper(99));
        FakeWrapper raw = u.release();
        TEST_REQUIRE(raw.id == 99);
        TEST_REQUIRE(!u);
    }
    TEST_REQUIRE(g_destroyed == 0);

    // reset() destroys the current value and adopts the new one (if any).
    g_destroyed = 0;
    {
        U u(FakeWrapper(10));
        u.reset(FakeWrapper(20));
        TEST_REQUIRE(g_destroyed == 1);
        TEST_REQUIRE(u->id == 20);
        u.reset();
        TEST_REQUIRE(g_destroyed == 2);
        TEST_REQUIRE(!u);
    }
    TEST_REQUIRE(g_destroyed == 2);

    // swap() exchanges values without invoking the deleter.
    g_destroyed = 0;
    {
        U a(FakeWrapper(1));
        U b(FakeWrapper(2));
        a.swap(b);
        TEST_REQUIRE(a->id == 2);
        TEST_REQUIRE(b->id == 1);
        TEST_REQUIRE(g_destroyed == 0);
    }
    TEST_REQUIRE(g_destroyed == 2);

    // makeUnique<T>() invokes Traits<T>::create().
    g_destroyed = 0;
    {
        auto u = glaze::makeUnique<FakeWrapper>();
        TEST_REQUIRE(static_cast<bool>(u));
        TEST_REQUIRE(u->poke() == 2); // create() returned id=1, *2 = 2
    }
    TEST_REQUIRE(g_destroyed == 1);

    // ── Shared ──────────────────────────────────────────────────────────────

    // Default Shared is empty, allocates nothing.
    g_destroyed = 0;
    {
        S s;
        TEST_REQUIRE(!s);
        TEST_REQUIRE(s.use_count() == 0);
    }
    TEST_REQUIRE(g_destroyed == 0);

    // Constructing from a null value should not allocate or destroy.
    g_destroyed = 0;
    {
        S s{FakeWrapper()};
        TEST_REQUIRE(!s);
        TEST_REQUIRE(s.use_count() == 0);
    }
    TEST_REQUIRE(g_destroyed == 0);

    // Copy construction shares the value; deletion happens on the last copy.
    g_destroyed = 0;
    {
        S a(FakeWrapper(5));
        TEST_REQUIRE(a.use_count() == 1);
        {
            S b = a;
            TEST_REQUIRE(a.use_count() == 2);
            TEST_REQUIRE(b.use_count() == 2);
            TEST_REQUIRE(b->id == 5);
        }
        TEST_REQUIRE(a.use_count() == 1);
        TEST_REQUIRE(g_destroyed == 0);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Move construction transfers the control block.
    g_destroyed = 0;
    {
        S a(FakeWrapper(8));
        S b(std::move(a));
        TEST_REQUIRE(!a);
        TEST_REQUIRE(b.use_count() == 1);
        TEST_REQUIRE(b->id == 8);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Copy assignment releases the previous resource before adopting the new one.
    g_destroyed = 0;
    {
        S a(FakeWrapper(1));
        S b(FakeWrapper(2));
        a = b;
        TEST_REQUIRE(g_destroyed == 1);
        TEST_REQUIRE(a->id == 2);
        TEST_REQUIRE(a.use_count() == 2);
    }
    TEST_REQUIRE(g_destroyed == 2);

    // reset() drops the strong reference and triggers deletion when last.
    g_destroyed = 0;
    {
        S a(FakeWrapper(3));
        a.reset();
        TEST_REQUIRE(g_destroyed == 1);
        TEST_REQUIRE(!a);
        TEST_REQUIRE(a.use_count() == 0);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // makeShared<T>() invokes Traits<T>::create().
    g_destroyed = 0;
    {
        auto s = glaze::makeShared<FakeWrapper>();
        TEST_REQUIRE(static_cast<bool>(s));
        TEST_REQUIRE(s->poke() == 2);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // ── Weak ────────────────────────────────────────────────────────────────

    // Default Weak is expired and locks to an empty Shared.
    {
        W w;
        TEST_REQUIRE(w.expired());
        TEST_REQUIRE(w.use_count() == 0);
        S locked = w.lock();
        TEST_REQUIRE(!locked);
    }

    // Weak constructed from a live Shared can promote back via lock().
    g_destroyed = 0;
    {
        S s(FakeWrapper(11));
        W w(s);
        TEST_REQUIRE(!w.expired());
        TEST_REQUIRE(w.use_count() == 1);
        S locked = w.lock();
        TEST_REQUIRE(static_cast<bool>(locked));
        TEST_REQUIRE(locked->id == 11);
        TEST_REQUIRE(s.use_count() == 2);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Weak outlives the resource: control block survives, lock() returns empty.
    g_destroyed = 0;
    {
        W w;
        {
            S s(FakeWrapper(99));
            w = s;
            TEST_REQUIRE(!w.expired());
        }
        TEST_REQUIRE(g_destroyed == 1);
        TEST_REQUIRE(w.expired());
        S locked = w.lock();
        TEST_REQUIRE(!locked);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Copying a Weak observer does not affect the strong count.
    g_destroyed = 0;
    {
        S s(FakeWrapper(4));
        W w1(s);
        W w2 = w1;
        TEST_REQUIRE(!w1.expired());
        TEST_REQUIRE(!w2.expired());
        TEST_REQUIRE(w2.use_count() == 1);
        TEST_REQUIRE(s.use_count() == 1);
    }
    TEST_REQUIRE(g_destroyed == 1);

    // Moving a Weak observer empties the source.
    g_destroyed = 0;
    {
        S s(FakeWrapper(4));
        W w1(s);
        W w2 = std::move(w1);
        TEST_REQUIRE(w1.expired());
        TEST_REQUIRE(!w2.expired());
    }
    TEST_REQUIRE(g_destroyed == 1);

    return 0;
}
