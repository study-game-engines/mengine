#! /bin/bash

CONFIGURATION=Release

mkdir -p ../../solutions/dependencies_xcode_ios_sdl/$CONFIGURATION
pushd ../../solutions/dependencies_xcode_ios_sdl/$CONFIGURATION
/Applications/CMake.app/Contents/bin/cmake -G"Xcode" "$PWD/../../../CMake/Depends_Xcode_IOS_SDL" -DCMAKE_BUILD_TYPE:STRING=$CONFIGURATION -DCMAKE_CONFIGURATION_TYPES:STRING=$CONFIGURATION -DCMAKE_TOOLCHAIN_FILE="$PWD/../../../dependencies/ios-cmake/ios.toolchain.cmake" -DIOS_PLATFORM=OS64 -DENABLE_BITCODE=0 -DIOS_DEPLOYMENT_TARGET="9.0"
popd
