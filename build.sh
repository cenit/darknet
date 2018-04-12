#!/bin/bash

#if [[ "$OSTYPE" == "darwin"* && "$1" == "gcc" ]]; then
if [[ "$OSTYPE" == "darwin"* ]]; then
 export CC="/usr/local/bin/gcc-7"
 export CXX="/usr/local/bin/g++-7"
fi

mkdir -p build
cd build
cmake ..
cmake --build .
cd ..
