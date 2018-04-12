call "%PROGRAMFILES(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

rmdir /S /Q build
mkdir build
cd build
cmake -G "Ninja" "-DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake" "-DVCPKG_TARGET_TRIPLET=%VCPKG_DEFAULT_TRIPLET%" "-DCMAKE_BUILD_TYPE=Release" ..
cmake --build . --config Release
cd ..
