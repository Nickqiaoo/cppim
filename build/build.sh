make clean
rm -rf CMakeCache.txt CMakeFiles cmake_install.cmake Makfile
cmake .. -DCMAKE_BUILD_TYPE=Debug 
make -j8