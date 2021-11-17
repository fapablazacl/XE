#!/bin/bash

# glbinding
git clone --depth 1 --branch v3.1.0 https://github.com/cginternals/glbinding.git dep/glbinding
mkdir -p dep/glbinding/build 
cmake -B dep/glbinding/build -DCMAKE_BUILD_TYPE=Debug -S . 
cmake --build dep/glbinding/build 
sudo cmake --build dep/glbinding/build --target install

# glades2
git clone --depth 1 https://github.com/fapablazacl/glades2.git dep/glades2
mkdir -p dep/glades2/build 
cmake -B dep/glades2/build -DCMAKE_BUILD_TYPE=Debug -S . 
cmake --build dep/glades2/build 
sudo cmake --build dep/glades2/build --target install

# glfw
git clone --depth 1 --branch 3.3.5 https://github.com/glfw/glfw.git dep/glfw
mkdir -p dep/glfw/build 
cmake -B dep/glfw/build -DCMAKE_BUILD_TYPE=Debug -S . 
cmake --build dep/glfw/build 
sudo cmake --build dep/glfw/build --target install
