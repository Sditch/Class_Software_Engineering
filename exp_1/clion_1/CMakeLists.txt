cmake_minimum_required(VERSION 3.12)
project(clion_1)

set(CMAKE_CXX_STANDARD 11)

add_executable(clion_1 main.cpp)

include_directories(/usr/local/Cellar/gdal/2.3.1_2/include)
link_directories(/usr/local/Cellar/gdal/2.3.1_2/lib)
link_libraries(gdal)

target_link_libraries(clion_1 gdal)
