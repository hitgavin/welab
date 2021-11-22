################################################################################
#APPEND_TO_CACHED_STRING(_string _cacheDesc [items...])
# Appends items to a cached list.
MACRO (APPEND_TO_CACHED_STRING _string _cacheDesc)
  FOREACH (newItem ${ARGN})
    SET (${_string} "${${_string}} ${newItem}" CACHE INTERNAL ${_cacheDesc} FORCE)
  ENDFOREACH (newItem ${ARGN})
  #STRING(STRIP ${${_string}} ${_string})
ENDMACRO (APPEND_TO_CACHED_STRING)

################################################################################
# APPEND_TO_CACHED_LIST (_list _cacheDesc [items...]
# Appends items to a cached list.
MACRO (APPEND_TO_CACHED_LIST _list _cacheDesc)
  SET (tempList ${${_list}})
  FOREACH (newItem ${ARGN})
    LIST (APPEND tempList ${newItem})
  ENDFOREACH (newItem ${newItem})
  SET (${_list} ${tempList} CACHE INTERNAL ${_cacheDesc} FORCE)
ENDMACRO(APPEND_TO_CACHED_LIST)

#################################################
# Macro to turn a list into a string (why doesn't CMake have this built-in?)
MACRO (LIST_TO_STRING _string _list)
    SET (${_string})
    FOREACH (_item ${_list})
      SET (${_string} "${${_string}} ${_item}")
    ENDFOREACH (_item)
    #STRING(STRIP ${${_string}} ${_string})
ENDMACRO (LIST_TO_STRING)

#################################################
# BUILD ERROR macro
macro (BUILD_ERROR)
  foreach (str ${ARGN})
    SET (msg "\t${str}")
    MESSAGE (STATUS ${msg})
    APPEND_TO_CACHED_LIST(build_errors "build errors" ${msg})
  endforeach ()
endmacro (BUILD_ERROR)

#################################################
# BUILD WARNING macro
macro (BUILD_WARNING)
  foreach (str ${ARGN})
    SET (msg "\t${str}" )
    MESSAGE (STATUS ${msg} )
    APPEND_TO_CACHED_LIST(build_warnings "build warning" ${msg})
  endforeach (str ${ARGN})
endmacro (BUILD_WARNING)

#################################################
macro (wl_setup_unix)
    # Using dynamic linking in UNIX by default
    set(BUILD_SHARED_LIBS TRUE)
endmacro()

#################################################
macro (wl_setup_windows)
    # Using dynamic linking in Windows by default
    set(BUILD_SHARED_LIBS TRUE)
    add_definitions(-DWIN32_LEAN_AND_MEAN)

    # Need for M_PI constant
    add_definitions(-D_USE_MATH_DEFINES)

    # Don't pull in the Windows min/max macros
    add_definitions(-DNOMINMAX)

    #use static libraries for FREEIMAGE
    add_definitions(-DFREEIMAGE_LIB)

    # Use dynamic linking for boost
    add_definitions(-DBOOST_ALL_DYN_LINK)

    # Use dynamic linking for protobuf
    add_definitions(-DPROTOBUF_USE_DLLS)

    # And we want exceptions
    add_definitions("/EHsc")

    if (MSVC AND CMAKE_SIZEOF_VOID_P EQUAL 8)
      # Not need if proper cmake gnerator (-G "...Win64") is passed to cmake
      # Enable as a second measure to workaround over bug
      # http://www.cmake.org/Bug/print_bug_page.php?bug_id=11240
      set(CMAKE_SHARED_LINKER_FLAGS "/machine:x64")
    endif()

    if (MSVC)
      add_compile_options(/Zc:__cplusplus)
    endif()
endmacro()

#################################################
macro (wl_setup_apple)
  # NOTE MacOSX provides different system versions than CMake is parsing.
  #      The following table lists the most recent OSX versions
  #     9.x.x = Mac OSX Leopard (10.5)
  #    10.x.x = Mac OSX Snow Leopard (10.6)
  #    11.x.x = Mac OSX Lion (10.7)
  #    12.x.x = Mac OSX Mountain Lion (10.8)
  if (${CMAKE_SYSTEM_VERSION} LESS 10)
    add_definitions(-DMAC_OS_X_VERSION=1050)
  elseif (${CMAKE_SYSTEM_VERSION} GREATER 10 AND ${CMAKE_SYSTEM_VERSION} LESS 11)
    add_definitions(-DMAC_OS_X_VERSION=1060)
  elseif (${CMAKE_SYSTEM_VERSION} GREATER 11 AND ${CMAKE_SYSTEM_VERSION} LESS 12)
    add_definitions(-DMAC_OS_X_VERSION=1070)
  elseif (${CMAKE_SYSTEM_VERSION} GREATER 12 OR ${CMAKE_SYSTEM_VERSION} EQUAL 12)
    add_definitions(-DMAC_OS_X_VERSION=1080)
  else ()
    add_definitions(-DMAC_OS_X_VERSION=0)
  endif ()

  # libstdc++ used on 10.8 and earlier
  # libc++ after that
  if (${CMAKE_SYSTEM_VERSION} LESS 13)
    set (APPLE_PKGCONFIG_LIBS "${APPLE_PKGCONFIG_LIBS} -lstdc++")
  else()
    set (APPLE_PKGCONFIG_LIBS "${APPLE_PKGCONFIG_LIBS} -lc++")
  endif()

  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-undefined -Wl,dynamic_lookup")
endmacro()

#################################################
# Macro to setup supported compiler flags
# Based on work of Florent Lamiraux, Thomas Moulard, JRL, CNRS/AIST.
include(CheckCXXCompilerFlag)

macro(filter_valid_compiler_flags)
  foreach(flag ${ARGN})
    CHECK_CXX_COMPILER_FLAG(${flag} R${flag})
    if(${R${flag}})
      set(VALID_CXX_FLAGS "${VALID_CXX_FLAGS} ${flag}")
    endif()
  endforeach()
endmacro()

#####################################
# Gnu Precompiled Headers
if (CMAKE_COMPILER_IS_GNUCXX)
  option(USE_PCH "compiles using gnu precompiled headers" OFF)
endif()

# target_name a target name for generating the PCH file
# filename the name of the PCH file, relative to the dir of the CMakeLists calling the macro
macro(add_pch target_name filename)

  set(pch_out ${CMAKE_CURRENT_BINARY_DIR}/${filename}.out.gch)
  set(pch_in ${CMAKE_CURRENT_SOURCE_DIR}/${filename})
  set(FLAGS -fPIC -x c++-header)

  separate_arguments(ARGS UNIX_COMMAND "${CMAKE_C_FLAGS} ${CMAKE_CXX_FLAGS}")
  add_custom_command(OUTPUT ${pch_out}
    COMMAND ${CMAKE_CXX_COMPILER} ${ARGS} ${ARGN} ${FLAGS} ${pch_in} -o ${pch_out}
    DEPENDS ${pch_in}
    COMMENT "Generating precompiled header: ${pch_out}"
    VERBATIM)

  add_custom_target(${target_name}_pch DEPENDS ${pch_out})

  target_compile_options(${target_name} PRIVATE -Winvalid-pch -include ${filename}.out)
  add_dependencies(${target_name} ${target_name}_pch)

endmacro()
