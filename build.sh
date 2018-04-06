#!/bin/bash

if [[ "$OSTYPE" == "darwin"* && "$1" == "gcc" ]]; then
 export CC="/usr/local/bin/gcc-7"
 export CXX="/usr/local/bin/g++-7"
fi

mkdir -p build
cd build
cmake "-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE=${WORKSPACE}\sysconfig\cmake\physycom_toolchain.cmake" ..
cmake --build . 
cd ..
