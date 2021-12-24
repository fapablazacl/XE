#!/bin/bash

export CC=gcc 
export CXX=gcc 

# build catch
cmake -Bdep/Catch2/build -SCatch2 -G "Unix Makefiles" -DCMAKE_INSTALL_PREFIX=local -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF
cmake --build Catch2/build
cmake --install Catch2/build

# build glades2
# cmake -BCatch2/build -SCatch2 -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=local -DCATCH_BUILD_TESTING=OFF -DCATCH_INSTALL_DOCS=OFF
