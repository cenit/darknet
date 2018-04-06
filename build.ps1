#!/usr/bin/env powershell

mkdir -Force build
cd build

cmake -G "Ninja" "-DCMAKE_TOOLCHAIN_FILE=$env:WORKSPACE\vcpkg\scripts\buildsystems\vcpkg.cmake" "-DCMAKE_BUILD_TYPE=Release" ..
cmake --build . --config Release

cd ..
