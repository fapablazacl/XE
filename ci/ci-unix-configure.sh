#!/bin/bash

configure_clang(){
    buildType=$1
    buildDir=".build/clang/$buildType"

    if [ $buildType != "static-analysis" ] && [ $buildType != "debug" ] && [ $buildType != "release" ]
    then
        echo "Supplied build type $buildType unknown for the clang toolchain"
        exit 1
    fi

    mkdir -p $buildDir
    cd $buildDir

    FLAGS="-Werror -Wall -Wextra"

    if [$buildType = "static-analysis"]
    then
        buildType="Debug"
        FLAGS="$FLAGS --analyze"
    fi

    if [$buildType = "debug"]
    then
        buildType="Debug"
    fi

    if [$buildType = "release"]
    then
        buildType="Release"
    fi

    CC=clang && CXX=clang++ && cmake ../../../../ -G "Unix Makefiles" \
        -DCMAKE_C_FLAGS=$FLAGS \
        -DCMAKE_CXX_FLAGS=$FLAGS \
        -DCMAKE_BUILD_TYPE=Debug \
        -DXE_TEST=ON \
        -DXE_PLUGIN_GL=ON \
        -DXE_PLUGIN_ES2=ON \
        -DXE_PLUGIN_GL_GLFW=ON
}


configure_clang $1
