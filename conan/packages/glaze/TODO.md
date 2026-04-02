
# Current TODOs
1. Complete the usage for the Flags in the generated C++ header.
1. Add a generation timestamp, indicating that the file is automatically generated and should not be modified.
1. Parse the documentation from the OpenGL-refpages and include it in the generated code as Doxygen comments. This should be included in both C and C++ headers.
1. Add support for DSA (Direct State Access) functions, by creating proper classes with methods.
1. Add overloads for functions that upload data to the GPU (mainly textures and buffers), using an ArrayLike view class.
