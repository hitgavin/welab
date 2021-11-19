cmake_minimum_required(VERSION 3.0.2)
project(console_bridge)

include(GNUInstallDirs)
include(GenerateExportHeader)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(POLICY CMP0042)
  cmake_policy(SET CMP0042 NEW)
endif()

# If compiler support symbol visibility, enable it.
include(CheckCCompilerFlag)
check_c_compiler_flag(-fvisibility=hidden HAS_VISIBILITY)
if (HAS_VISIBILITY)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fvisibility=hidden")
endif()

if(NOT WIN32)
  # Use c++11 compiler flag, since it's needed for console.cpp
  # It isn't in a header file, so it doesn't need to be exported
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CMAKE_CXX_FLAGS}")

if(MSVC OR MSVC90 OR MSVC10)
  set(MSVC ON)
endif (MSVC OR MSVC90 OR MSVC10)


if(NOT DEFINED BUILD_SHARED_LIBS)
  option(BUILD_SHARED_LIBS "Build dynamically-linked binaries" ON)
endif()

# Control where libraries and executables are placed during the build
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

add_library(${PROJECT_NAME} src/console.cpp)
set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION
               ${CONSOLE_BRIDGE_MAJOR_VERSION}.${CONSOLE_BRIDGE_MINOR_VERSION})
target_include_directories(${PROJECT_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
	$<INSTALL_INTERFACE:include>)
generate_export_header(${PROJECT_NAME}
    EXPORT_MACRO_NAME CONSOLE_BRIDGE_DLLAPI)

install(TARGETS ${PROJECT_NAME}
  EXPORT ${PROJECT_NAME}-targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  FILES_MATCHING PATTERN "*.h")

install(FILES ${PROJECT_BINARY_DIR}/${PROJECT_NAME}_export.h
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME})

if(WIN32 AND NOT CYGWIN)
  set(CMAKE_CONFIG_INSTALL_DIR CMake)
else()
  set(CMAKE_CONFIG_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/${PROJECT_NAME}/cmake/)
endif()

if(BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
