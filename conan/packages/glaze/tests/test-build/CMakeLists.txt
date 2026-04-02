
project (test-gl10)
set(CMAKE_CXX_STANDARD 11)
add_executable(test-gl10 include/glaze/gl.h include/glaze/gl.hpp test-gl-h.cpp test-gl-hpp.cpp src/gl.c test-gl10.cpp)
target_include_directories(test-gl10 PUBLIC "include")
