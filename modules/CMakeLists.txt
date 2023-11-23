cmake_minimum_required(VERSION 3.15)
project(cpp_modules)

add_compile_options(-std=c++17 -Wall -Wextra)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Set the build type." FORCE)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INSTALL_INCLUDEDIR ${CMAKE_INSTALL_PREFIX}/include)
set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_PREFIX}/lib)
set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_PREFIX}/bin)

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
  enable_testing()
endif()

add_subdirectory(core)
# add_subdirectory(control)
# add_subdirectory(optimization)
add_subdirectory(planning)
