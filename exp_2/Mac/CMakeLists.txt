cmake_minimum_required(VERSION 3.12)
project(mac_exp2)

set(CMAKE_CXX_STANDARD 14)


add_executable(mac_exp2 task2.cpp)

include_directories(/usr/local/Cellar/gdal/2.3.1_2/include)
link_directories(/usr/local/Cellar/gdal/2.3.1_2/include)


target_link_libraries(mac_exp2 /usr/local/Cellar/gdal/2.3.1_2/lib/libgdal.20.dylib)
