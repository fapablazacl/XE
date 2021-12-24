#!/bin/bash

export CC=clang
export CXX=clang++

cmake -B build/clang/debug \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON -DXE_PLUGIN_VULKAN=ON -DXE_UNIT_TEST=ON \
    -Dglfw3_DIR=$PWD/local/glfw/lib/cmake/glfw3 \
    -Dglbinding_DIR=$PWD/local/glbinding \
    -Dglades2_DIR=$PWD/local/glades2/lib/cmake/glades2 \
    -DCatch2_DIR=$PWD/local/Catch2/lib/cmake/Catch2

cmake -B build/clang/release \
    -G "Unix Makefiles" \
    -DCMAKE_BUILD_TYPE=Release \
    -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON -DXE_PLUGIN_VULKAN=ON -DXE_UNIT_TEST=ON \
    -Dglfw3_DIR=$PWD/local/glfw/lib/cmake/glfw3 \
    -Dglbinding_DIR=$PWD/local/glbinding \
    -Dglades2_DIR=$PWD/local/glades2/lib/cmake/glades2 \
    -DCatch2_DIR=$PWD/local/Catch2/lib/cmake/Catch2
