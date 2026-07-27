#!/bin/bash

BUILD_DIR="build"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

OS="$(uname -s)"

case "$OS" in
    Linux*|Darwin*)
        echo "Detected Unix-like system (Mac/Linux), building with Unix Makefiles..."
        cmake .. -G "Unix Makefiles"
        make
        ;;
    CYGWIN*|MINGW*|MSYS*)
        echo "Detected Windows, building with MinGW..."
        cmake .. -G "MinGW Makefiles"
        mingw32-make
        ;;
    *)
        echo "Unknown OS: $OS"
        exit 1
        ;;
esac

echo "Build complete!"

# Run
./CDMA