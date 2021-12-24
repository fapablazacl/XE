#!/bin/bash

export CC=gcc 
export CXX=gcc 

# build catch
cmake -Bdep/.build/Catch2 -Sdep/Catch2 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=local -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF
cmake --build dep/.build/Catch2
cmake --install dep/.build/Catch2

# build glades2
cmake -Bdep/.build/glades2/debug -Sdep/glades2 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=local
cmake --build dep/.build/glades2/debug
cmake --install dep/.build/glades2/debug

cmake -Bdep/.build/glades2/release -Sdep/glades2 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=local
cmake --build dep/.build/glades2/release
cmake --install dep/.build/glades2/release

# build glfw
cmake -Bdep/.build/glfw/debug -Sdep/glfw -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=local -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
cmake --build dep/.build/glfw/debug
cmake --install dep/.build/glfw/debug

cmake -Bdep/.build/glfw/release -Sdep/glfw -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=local -DGLFW_BUILD_DOCS=OFF -DGLFW_BUILD_EXAMPLES=OFF -DGLFW_BUILD_TESTS=OFF
cmake --build dep/.build/glfw/release
cmake --install dep/.build/glfw/release
