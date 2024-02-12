cmake_minimum_required(VERSION 3.15)
project(core CXX)

include_directories(include)

find_package(spdlog)
find_package(fmt)
include_directories(${spdlog_INCLUDE_DIRS} ${fmt_INCLUDE_DIRS})

add_library(welab_core src/logger.cpp src/logger_impl.cpp)
target_link_libraries(welab_core ${fmt_LIBRARIES} ${spdlog_LIBRARIES} pthread)
target_include_directories(welab_core PUBLIC include ${spdlog_INCLUDE_DIRS} ${fmt_INCLUDE_DIRS})


add_subdirectory(pycore)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

install(TARGETS welab_core
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)