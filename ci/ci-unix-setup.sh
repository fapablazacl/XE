#!/bin/bash

mkdir -p .build/gcc/debug
cd .build/gcc/debug

cmake --build .build/gcc/debug -G "Unix Makefiles" -DCXX_FLAGS="-Werror -Wall -Wextra" -DCMAKE_BUILD_TYPE=Debug -DXE_TEST=ON -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON
cd ../../..
