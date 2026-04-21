#!/bin/bash
set -e

PROFILE_ARG=""
if [ -n "$1" ]; then
    PROFILE_ARG="--profile=$1"
    echo "Using profile: $1"
else
    echo "Using default profile."
fi

BUILD_TYPES="Debug Release RelWithDebInfo"

for b in $BUILD_TYPES; do
    echo ""
    echo "========================================================"
    echo "Installing build type: $b"
    echo "========================================================"
    conan install . --build=missing $PROFILE_ARG --settings=build_type=$b
done

echo ""
echo "All build types installed successfully."
