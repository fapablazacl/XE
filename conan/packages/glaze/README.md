# glaze

Glaze is a generator for a type-safe C++11 API wrapper for OpenGL and OpenGL ES based on the XML Khronos specifications, inspired by the Vulkan C++ API.

It supports the following OpenGL versions:
- OpenGL ( 1.0, 1.1, 1.2, 1.3, 1.4, 2.0, 2.1, 3.0, 3.1, 3.2, 3.3, 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6)
- OpenGL ES (1.0, 1.1, 2.0, 3.0, 3.1, 3.2)

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
| Use inline functions                            | Zero-overhead       |
| Enclose all symbols in the `gl` namespace       | Clean code          |
| Use `enum class` over macros                    | Prevents API misuse |
| Use strong typedefs over primitive types        | Prevents API misuse |

The C++ API should not have any CPU usage impact (zero overhead)

## Other considerations
- Be completely self-contained, with no additional dependencies.
- Have the documentation attached.
