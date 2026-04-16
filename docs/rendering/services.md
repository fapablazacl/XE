# `XeServices` — Facility Injection Hub

The single struct of function-pointer interfaces the renderer uses for allocation, logging, profiling, assertion, and optional memory tracking. It is passed explicitly to every subsystem that needs it. There is no global state — clients that need multiple renderers with independent diagnostics simply pass different `XeServices` instances.

---

```cpp
enum class XeLogLevel : uint8_t { Trace, Debug, Info, Warn, Error, Fatal };

struct XeLogSink {
    void (*write)(void* user, XeLogLevel level, const char* tag,
                  const char* msg, const char* file, int line) noexcept;
    void*      user_data;
    XeLogLevel min_level = XeLogLevel::Info;
};

struct XeProfilerHook {
    void (*begin_cpu_scope)(void* user, const char* name, uint32_t argb_color) noexcept;
    void (*end_cpu_scope)  (void* user)                                         noexcept;
    void (*mark)           (void* user, const char* name)                       noexcept;
    void (*gpu_timestamp)  (void* user, const char* name, uint64_t ns)          noexcept;
    void (*counter)        (void* user, const char* name, int64_t value)        noexcept;
    void* user_data;
};

struct XeAllocator {
    void* (*alloc)  (void* user, size_t size, size_t align)                          noexcept;
    void  (*free)   (void* user, void* ptr, size_t size, size_t align)               noexcept;
    void* (*realloc)(void* user, void* ptr, size_t old_sz, size_t new_sz, size_t align) noexcept;
    void* user_data;
    static XeAllocator make_default()                              noexcept;
    static XeAllocator make_tracking(XeAllocator base, XeAllocTracker*) noexcept;
};

struct XeAllocTracker {
    void (*on_alloc)(void* user, void* ptr, size_t size, size_t align,
                     const char* tag, const char* file, int line) noexcept;
    void (*on_free) (void* user, void* ptr, size_t size)          noexcept;
    void* user_data;
};

struct XeAssertHandler {
    bool (*on_assert)(void* user, const char* expr, const char* msg,
                      const char* file, int line) noexcept; // false → abort()
    void* user_data;
};

struct XeServices {
    XeAllocator     allocator;
    XeLogSink       log;
    XeProfilerHook  profiler;
    XeAllocTracker* alloc_tracker = nullptr;
    XeAssertHandler assert_handler;
    static XeServices make_default() noexcept;
};

// Internal-use macros (implementation files only)
// XE_LOG(svc, level, tag, fmt, ...)
// XE_TRACE_SCOPE(svc, name, color)
// XE_ASSERT(svc, expr, msg)
// XE_ALLOC(svc, size, align)
// XE_FREE(svc, ptr, size, align)
```

---

## See Also

- [render-device.md](render-device.md) — every `RenderDeviceT<Policy>` stores an `XeServices` by value
- [resources.md](resources.md) — `RenderDeviceDesc` embeds `XeServices` at creation time
- [system-caps.md](system-caps.md) — `SystemCaps::query_host` and `query_gpu` both take `const XeServices&`
- [threading-contract.md](threading-contract.md) — log / profiler / allocator callbacks must themselves be thread-safe if the client records command buffers on multiple threads
