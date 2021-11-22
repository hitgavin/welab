cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)

if(COMMAND CMAKE_POLICY)
  CMAKE_POLICY(SET CMP0003 NEW)
  CMAKE_POLICY(SET CMP0004 NEW)
endif(COMMAND CMAKE_POLICY)

if(POLICY CMP0100)
  cmake_policy(SET CMP0100 NEW)
endif()

project(welab)

add_compile_options(-std=c++17)

string(TOLOWER ${PROJECT_NAME} PROJECT_NAME_LOWER)
string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)

set(WELAB_MAJOR_VERSION 1)
set(WELAB_MINOR_VERSION 0)
set(WELAB_PATCH_VERSION 0)

set(WELAB_VERSION ${WELAB_MAJOR_VERSION}.${WELAB_MINOR_VERSION})
set(WELAB_VERSION_FULL ${WELAB_MAJOR_VERSION}.${WELAB_MINOR_VERSION}.${WELAB_PATCH_VERSION})

message(STATUS "${PROJECT_NAME} version ${WELAB_VERSION_FULL}")

set(welab_cmake_dir ${PROJECT_SOURCE_DIR}/cmake CACHE PATH "Location of CMake scripts")

if (NOT DEFINED BUILD_TESTING)
  set(BUILD_TESTING OFF)
endif()

set(CMAKE_CXX_EXTENSIONS off)

# Documentation
add_subdirectory(doc)

enable_testing()

# with -fPIC
if(UNIX AND NOT WIN32)
  set(CMAKE_INSTALL_PREFIX "/usr" CACHE STRING "Install Prefix")
  find_program(CMAKE_UNAME uname /bin /usr/bin /usr/local/bin)
  if(CMAKE_UNAME)
    exec_program(uname ARGS -m OUTPUT_VARIABLE CMAKE_SYSTEM_PROCESSOR)
    set(CMAKE_SYSTEM_PROCESSOR ${CMAKE_SYSTEM_PROCESSOR} CACHE INTERNAL "processor type (i386 and x86_64)")
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
      add_definitions(-fPIC)
    endif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
  endif(CMAKE_UNAME)
endif()

set(INCLUDE_INSTALL_DIR "${CMAKE_INSTALL_INCLUDEDIR}/welab-${WELAB_MAJOR_VERSION}/welab")
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})
set(BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR})

set(BUILD_WELAB ON CACHE INTERNAL "Build welab" FORCE)
set(build_errors "" CACHE INTERNAL "build errors" FORCE)
set(build_warnings "" CACHE INTERNAL "build warnings" FORCE)

set(MIN_OGRE_VERSION 1.7.4 CACHE INTERNAL "Ogre version requirement" FORCE)
set(MIN_BOOST_VERSION 1.40.0 CACHE INTERNAL "Boost min version requirement" FORCE)

set(WELAB_RELATIVE_PLUGIN_LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/welab-${WELAB_MAJOR_VERSION}/plugins)
set(WELAB_RELATIVE_PLUGIN_BIN_INSTALL_DIR ${CMAKE_INSTALL_BINDIR}/welab-${WELAB_MAJOR_VERSION}/plugins)
set(WELAB_PLUGIN_LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${WELAB_RELATIVE_PLUGIN_LIB_INSTALL_DIR})
set(WELAB_PLUGIN_BIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/${WELAB_RELATIVE_PLUGIN_BIN_INSTALL_DIR})

if(WIN32)
  set(WELAB_RELATIVE_PLUGIN_PATH ${WELAB_RELATIVE_PLUGIN_BIN_INSTALL_DIR})
else()
  set(WELAB_RELATIVE_PLUGIN_PATH ${WELAB_RELATIVE_PLUGIN_LIB_INSTALL_DIR})
endif()
set(WELAB_PLUGIN_PATH ${CMAKE_INSTALL_PREFIX}/${WELAB_RELATIVE_PLUGIN_PATH})
FILE(TO_NATIVE_PATH "${WELAB_PLUGIN_PATH}" WELAB_PLUGIN_PATH)

set(WELAB_RELATIVE_MODEL_PATH ${CMAKE_INSTALL_DATAROOTDIR}/welab-${WELAB_MAJOR_VERSION}/models)
set(WELAB_MODEL_PATH ${CMAKE_INSTALL_PREFIX}/${WELAB_RELATIVE_MODEL_PATH})
FILE(TO_NATIVE_PATH "${WELAB_MODEL_PATH}" WELAB_MODEL_PATH)

set(WELAB_RELATIVE_RESOURCE_PATH ${CMAKE_INSTALL_DATAROOTDIR}/welab-${WELAB_MAJOR_VERSION})
set(WELAB_RESOURCE_PATH ${CMAKE_INSTALL_PREFIX}/${WELAB_RELATIVE_RESOURCE_PATH})
FILE(TO_NATIVE_PATH "${WELAB_RESOURCE_PATH}" WELAB_RESOURCE_PATH)

message(STATUS "\n\n====== Finding 3rd Party Packages ======")
include(${welab_cmake_dir}/search_for_stuff.cmake)
message(STATUS "----------------------------------------\n")

################################################
message(STATUS "Checking welab build type")
# Set the default build type
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug" CACHE STRING
      "Choose the type of build, options are: Debug Release" FORCE)
endif(NOT CMAKE_BUILD_TYPE)
string(TOUPPER ${CMAKE_BUILD_TYPE} CMAKE_BUILD_TYPE_UPPERCASE)

set(WELAB_BUILD_TYPE_RELEASE FALSE)
set(WELAB_BUILD_TYPE_DEBUG FALSE)

if("${CMAKE_BUILD_TYPE_UPPERCASE}" STREQUAL "RELEASE")
  set(WELAB_BUILD_TYPE_RELEASE TRUE)
elseif("${CMAKE_BUILD_TYPE_UPPERCASE}" STREQUAL "DEBUG")
  set(WELAB_BUILD_TYPE_DEBUG TRUE)
else()
  build_error("CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} unknown. Valid options are: Debug Release")
endif()

# Check if warning options are available for the compiler and return WARNING_CXX_FLAGS variable
if(MSVC)
  # Default Windows-MSVC.cmake sets MSVC warning flag to /W3. Add some extra warning excludes.
  set(WARN_LEVEL "/wd4005 /wd4068 /wd4244 /wd4251 /wd4267 /wd4275 /wd4996")
else()
  set(WARN_LEVEL "-Wall")
endif()
filter_valid_compiler_flags(${WARN_LEVEL}
  -Wextra -Wno-long-long -Wno-unused-value -Wfloat-equal -Wshadow
  -Wswitch-default -Wmissing-include-dirs -pedantic
)

if(MSVC)
  # Unable to be filtered flags (failing due to limitations in filter_valid_compiler_warnings)
  # Handling exceptions rightly
  set(UNFILTERED_FLAGS "/EHsc")
else()
  set(UNFILTERED_FLAGS "")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${VALID_CXX_FLAGS} ${UNFILTERED_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR})


################################################
message(STATUS "\n\n====== Configuring 3rd Party Packages ======")
add_subdirectory(deps)
message(STATUS "-----------------------------------------\n")

################################################
if(build_warnings)
  message("-- BUILD WARNINGS")
  foreach(msg ${build_warnings})
    message("-- ${msg}")
  endforeach()
  message("-- END BUILD WARNINGS\n")
endif(build_warnings)

########### Add uninstall target ###############
configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)
add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

if(build_errors)
  message("-- BUILD ERRORS: These must be resolved before compiling.")
  foreach(msg ${build_errors})
    message("-- ${msg}")
  endforeach()
  message("-- END BUILD ERRORS\n")
  message(FATAL_ERROR "Errors encountered in build. Please see the BUILD ERRORS above.")
else(build_errors)

  ################################################
  # Write the config.h file
  if (DEFINED CMAKE_CXX_FLAGS)
    message (STATUS "Custom CFlags:${CMAKE_CXX_FLAGS}")
  else()
    message (STATUS "Use default CFlags")
  endif()
  message (STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
  message (STATUS "Install path: ${CMAKE_INSTALL_PREFIX}")

  if(BUILD_WELAB)
    add_subdirectory(welab)
    if(BUILD_TESTING)
      add_subdirectory(test)
    else()
      add_subdirectory(test EXCLUDE_FROM_ALL)
    endif()
  endif(BUILD_WELAB)


  ################################################
  # Make the package config files
  
endif(build_errors)