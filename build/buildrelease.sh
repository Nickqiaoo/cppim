#!/bin/bash
make clean
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makfile
cmake .. -DCMAKE_BUILD_TYPE=Release -DOPEN_MEMORY_CHECK=false
make -j8