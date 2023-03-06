#! /bin/bash

if test -f "/usr/local/bin/cmake"; then
    CMAKE=/usr/local/bin/cmake
fi

if test -f "/Applications/CMake.app/Contents/bin/cmake"; then
    CMAKE=/Applications/CMake.app/Contents/bin/cmake
fi

CONFIGURATION=$1
DEPLOY_PATH=$2
BUILD_VERSION=$3

if test -z "$CONFIGURATION"; then
    echo "please setup CONFIGURATION"
    exit 0
fi

if test -z "$DEPLOY_PATH"; then
    echo "please setup DEPLOY_PATH"
    exit 0
fi

if test -z "$BUILD_VERSION"; then
    echo "please setup BUILD_VERSION"
    exit 0
fi

mkdir -p ../../solutions/solution_xcode_macos_sdl/$CONFIGURATION
pushd ../../solutions/solution_xcode_macos_sdl/$CONFIGURATION

$CMAKE -G"Xcode" "$PWD/../../../cmake/Xcode_MacOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING="$CONFIGURATION" -DMENGINE_DEPLOY_PATH:STRING="$DEPLOY_PATH" -DMENGINE_BUILD_VERSION:STRING="$BUILD_VERSION"

if [ $? -ne 0 ]; then
    echo "please fix CMake"
    exit 0
fi

if test -f "Podfile"; then
    pod install --repo-update
    
    if [ $? -ne 0 ]; then
        echo "please fix Cocoapods"
        exit 0
    fi
fi

popd
