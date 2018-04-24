#!/bin/bash

if [[ "$OSTYPE" == "darwin"* && "$1" == "gcc" ]]; then
 export CC="/usr/local/bin/gcc-7"
 export CXX="/usr/local/bin/g++-7"
fi

rm -rf build_debug
mkdir -p build_debug
cd build_debug
cmake .. -DCMAKE_BUILD_TYPE=Debug
cmake --build .
cd ..
