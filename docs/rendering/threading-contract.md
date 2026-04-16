# Threading Contract

Defines how the rendering middleware interacts with threads. The renderer itself creates none — all parallelism lives in the client. This doc specifies which operations are safe from which threads, and the recommended pattern for multi-threaded command recording.

---

## Renderer Has Zero Internal Threads

The renderer creates no threads, owns no mutexes, and makes no use of atomics in the
fast path. All parallelism is the client's responsibility. `XeThread` / `XeJob` is a
**separate utility library** sitting above the renderer in the dependency graph. The
renderer does not link against it.

## Thread Safety Contract

| Operation | Guarantee |
|---|---|
| `CommandBufferT` recording (`draw*`, `bind_*`, `set_*`) | **Safe from any thread** — no shared state |
| `RenderDevice` resource creation (`create_buffer`, etc.) | **Render thread only** |
| `RenderDevice::submit()` / `submit_batch()` | **Render thread only** |
| `RenderDevice::begin_frame()` / `end_frame()` / `present()` | **Render thread only** |
| `BackendRegistry::probe()` | **One-shot, pre-device** |
| `SystemCaps::query_host()` | **Safe from any thread** — read-only after return |

## Multithreaded Recording Pattern

```
[Worker thread A]  CommandBuffer cmdbuf_a(arena_a);
                   cmdbuf_a.bind_pipeline(pl);
                   cmdbuf_a.draw_indexed(n);

[Worker thread B]  CommandBuffer cmdbuf_b(arena_b);
                   cmdbuf_b.bind_pipeline(pl2);
                   cmdbuf_b.draw_indexed(m);

[Render thread]    const CommandBuffer* batch[] = { &cmdbuf_a, &cmdbuf_b };
                   device->submit_batch(batch, 2);   // ordered by array index
                   device->end_frame();
                   device->present();
```

## Deferred Resource Creation Pattern

`create_*` is render-thread-only. Worker threads enqueue creation requests into a
lock-free ring buffer; the render thread flushes them at `begin_frame()` before any
`CommandBuffer` records against the new handles. This is a recommended client idiom,
not part of the renderer itself.

---

## See Also

- [render-device.md](render-device.md) — `RenderDeviceT<Policy>` and `CommandBufferT<Policy>` definitions
- [services.md](services.md) — `XeServices` injected facilities (allocator used by per-thread command buffer arenas)
- [backend-abi.md](backend-abi.md) — `submit_batch` slot on the backend vtable
