cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(yaml LANGUAGES CXX)


add_executable(yaml main.cpp)

target_include_directories(yaml PRIVATE .)

find_package(yaml-cpp REQUIRED)
target_link_libraries(yaml PRIVATE yaml-cpp)

find_package(PkgConfig REQUIRED)
pkg_check_modules(libyaml REQUIRED IMPORTED_TARGET yaml-0.1)
target_link_libraries(yaml PRIVATE PkgConfig::libyaml)

pkg_check_modules(libfyaml REQUIRED IMPORTED_TARGET libfyaml)
target_link_libraries(yaml PRIVATE PkgConfig::libfyaml)

target_link_libraries(yaml PRIVATE pthread)
