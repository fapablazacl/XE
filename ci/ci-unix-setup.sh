#!/bin/bash

mkdir -p .build/gcc/debug
cd .build/gcc/debug
cmake ../../../../ -G "Unix Makefiles" -DXE_CXX_FLAGS="-Werror -Wall" -DCMAKE_BUILD_TYPE=Debug -DXE_TEST=ON -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON
cd ../../..
