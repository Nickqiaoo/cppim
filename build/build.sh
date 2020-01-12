#!/bin/bash
make clean
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makfile
cmake .. -DCMAKE_BUILD_TYPE=Debug -DOPEN_MEMORY_CHECK=true -DGPERFTOOLS=true
make -j8