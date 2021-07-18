#!/bin/sh

# TODO: parametrize the compiler
lcov -c -d .build/gcc-7.3.1/coverage/src/ -o coverage.info
genhtml coverage.info -o out
