include_directories(include)
add_definitions(-DUTILS_LIBRARY)
add_library(utils 
  include/utils/host_os_info.hpp
  include/utils/os_specific_aspects.hpp
  include/utils/porting.hpp
  include/utils/utils_global.hpp
  src/host_os_info.cpp
  src/string_utils.cpp 
  src/wl_assert.cpp
)
target_link_libraries(utils
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  macros
)
target_include_directories(utils INTERFACE include)

install(TARGETS utils
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include 
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)