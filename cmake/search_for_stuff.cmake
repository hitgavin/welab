include(${welab_cmake_dir}/welab_utils.cmake)

########################################
# Find QT
find_package(Qt5 COMPONENTS Core Widgets OpenGL Test REQUIRED)

########################################
# Find GTest
find_package(GTest REQUIRED)
