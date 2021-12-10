include_directories(include)

add_definitions(-DEXTENSION_LIBRARY)

add_library(extension 
  include/extension/extension_global.hpp
  include/extension/iplugin_p.hpp
  include/extension/iplugin.hpp
  include/extension/plugin_manager_p.hpp
  include/extension/plugin_manager.hpp
  include/extension/plugin_spec_p.hpp
  include/extension/plugin_spec.hpp
  src/plugin_spec.cpp 
  src/plugin_manager.cpp
)

target_include_directories(extension INTERFACE include)

welab_depends(extension PUBLIC DEPENDS macros utils)

target_link_libraries(extension
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  macros
  utils
)

install(TARGETS extension
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
