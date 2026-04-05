# glaze

Glaze is a generator for both a classic C API and a type-safe C++11 API wrapper for OpenGL and OpenGL ES based on the XML Khronos specifications, inspired by the Vulkan C++ API.

It supports the following OpenGL versions:
- OpenGL ( 1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 2.1, 3.0, 3.1, 3.2, 3.3, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6)
- OpenGL Compat (1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 2.1) — legacy compatibility profile, capped at GL 2.1
- OpenGL ES (1.0, 1.1, 2.0, 3.0, 3.1, 3.2)

## Design goals (relevant when extending the generator)

- **Generated code** must have zero runtime overhead (inline functions in C++), no external dependencies, and catch errors at build time via `enum class` and strong typedefs.
- **Generator code** is intentionally simple and hackable; prefer clarity over abstraction.
- The C++ API style is close to raw OpenGL but inspired by Vulkan-Hpp (namespace, enum classes, span).
- The internal architecture should be:
1. A reader that generates an intermediate representation of the OpenGL API
2. One generator that renders the C++ API from that intermediate representation
3. One generator that renders the C API from that intermediate representation
4. It should be a cli driver tool that can be used to generate the C++ and C APIs from the Khronos XML registry, from the required features (versions and extensions) by the user.

## Thoughts on Generator code
1. Generator code should be easily hackable
2. Generator code should be extensible

## Thoughts on Generated code
1. Generated code should catch build-time errors
2. Generated code should check for runtime errors

### C++ language
In the case of C++, an API that is very similar to OpenGL should be generated (and inspired by Vulkan C++), but with the following changes:

| Strategy                                        | Reasoning           |
|-------------------------------------------------|---------------------|
| Use inline functors                             | Zero-overhead       |
| Enclose all symbols in the `gl` namespace       | Clean code          |
| Use `enum class` over macros                    | Prevents API misuse |
| Use strong typedefs over primitive types        | Prevents API misuse |
| Attach the OpenGL documentation                 | Prevents API misuse |

- If the application that is using this is in debug mode, each functor call should have a RAII-based error mechanism.
- Group function families in the same functor (for example, gl::vertex). Each OpenGL function should be called by one operator() overload.

Some expected usage examples about this API:

```cpp

gl::clearColor(0.2f, 0.3f, 0.3f, 1.0f);
gl::clear(gl::ClearBufferMask::eColorBufferBit | gl::ClearBufferMask::eDepthBufferBit);

if (gl::drawElementsInstanced) {
    // supported, safe to use 
    gl::drawElementsInstanced(gl::PrimitiveType::eTriangles, 6, gl::DrawElementsType::eUnsignedInt, nullptr, 10);
}
```

## Other considerations
- Be completely self-contained, with no additional dependencies.
- Have the documentation attached.
