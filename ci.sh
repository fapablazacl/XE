#!/bin/bash

# this script performs several steps for prototyping
# future CI builds on the cloud

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
echo "ci.sh: Performing debug build with clang"
echo "======================================="

mkdir -p $BUILD_DIR/clang/debug
cd $BUILD_DIR/clang/debug
export CFLAGS="-Wall -Werror" 
export CXXFLAGS="-Wall -Werror"
cmake ../../../ -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DXE_TEST=ON

if ! make ; then
    exit 1
fi

echo ""
echo "ci.sh: running unit tests ..."
echo "======================================="
./src/XE.Test/XE.Test
