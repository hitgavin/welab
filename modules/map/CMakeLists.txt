cmake_minimum_required(VERSION 3.15)
project(map CXX)

include_directories(include)

add_library(welab_map
            src/costmap_2d.cpp
            src/footprint_collision_checker.cpp
)
target_link_libraries(welab_map welab_core)
target_include_directories(welab_map PUBLIC include)

add_subdirectory(pymap)

install(TARGETS welab_map
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})