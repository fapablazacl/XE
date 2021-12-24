#!/bin/bash

# this script performs several steps for prototyping
# future CI builds on the cloud
# Environment: Unix (Linux, macOS)

SOURCE_DIR=`pwd`
BUILD_DIR=".cmake"

echo ""
echo "ci.sh: cleaning up local build directory"
echo "======================================="
rm -rf $BUILD_DIR/*

echo ""
echo "ci.sh: clang version"
echo "======================================="
clang -v

echo ""
echo "ci.sh: Performing Debug build"
echo "======================================="

cd $SOURCE_DIR
mkdir -p $BUILD_DIR/clang/debug
cd $BUILD_DIR/clang/debug
export CXXFLAGS="-Wall"
cmake ../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DXE_TEST=ON -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON

if ! make ; then
    exit 1
fi

echo ""
echo "ci.sh: running unit tests in Debug ..."
echo "======================================="
./src/XE.Test/XE.Test


echo ""
echo "ci.sh: Performing Release build"
echo "======================================="

cd $SOURCE_DIR
mkdir -p $BUILD_DIR/clang/release
cd $BUILD_DIR/clang/release
export CXXFLAGS="-Wall"
cmake ../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DXE_TEST=ON -DXE_PLUGIN_GL=ON -DXE_PLUGIN_ES2=ON -DXE_PLUGIN_GL_GLFW=ON

if ! make ; then
    exit 1
fi

echo ""
echo "ci.sh: running unit tests in Release ..."
echo "======================================="
./src/XE.Test/XE.Test
