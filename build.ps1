#!/usr/bin/env powershell

mkdir -Force build
cd build

cmake -G "Ninja" "-DCMAKE_TOOLCHAIN_FILE=$env:WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=$env:WORKSPACE\sysconfig\cmake\physycom_toolchain.cmake" "-DCMAKE_BUILD_TYPE=Release" ..
cmake --build . --config Release

cd ..
