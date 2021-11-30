include(${welab_cmake_dir}/welab_utils.cmake)

########################################
# Find QT
find_package(Qt5 COMPONENTS Core Widgets Gui OpenGL Test REQUIRED)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################
# Find GTest
find_package(GTest REQUIRED)

########################################
# Find pybind11
find_package(pybind11 REQUIRED)