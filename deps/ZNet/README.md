Z-Wave protocol library
===================================================

Build and run ZNet test
------------------------------
> git submodule init  
git submodule update --recursive  
mkdir build  
cd build  
cmake -DBUILD_TESTS=1 ..  
make check

#### SKIP_ZNET_LIB_INFO_IF_EXISTS
This cmake option prevents generation of znet_lib_info.h in case if such file already exists.
By default it is OFF, thus generation of znet_lib_info.h is always performed. 

Code style formatter
------------------------------
required clang-format 6.0.X
> mkdir build  
cd build  
make pretty
