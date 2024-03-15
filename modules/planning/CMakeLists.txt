cmake_minimum_required(VERSION 3.15)
project(planning CXX)

include_directories(include)

find_package(robin_hood REQUIRED)
include_directories(${robin_hood_INCLUDE_DIRS})

add_library(welab_planning
            src/a_star.cpp
            src/node_2d.cpp
            src/node_basic.cpp
            src/smac_planner_2d.cpp
)

target_link_libraries(welab_planning ${robin_hood_LIBRARIES} welab_core welab_map)
target_include_directories(welab_planning PUBLIC include)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS welab_planning
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)