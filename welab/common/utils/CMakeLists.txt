include_directories(include)
add_definitions(-DUTILS_LIBRARY)
add_library(utils 
  include/utils/porting.hpp
  include/utils/utils_global.hpp
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