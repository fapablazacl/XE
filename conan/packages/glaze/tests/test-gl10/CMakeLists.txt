
project (test-gl10)
set(CMAKE_CXX_STANDARD 11)
add_executable(test-gl10 gl.h gl.hpp test-gl10.cpp src/gl.c)
target_include_directories(test-gl10 PUBLIC "include")
