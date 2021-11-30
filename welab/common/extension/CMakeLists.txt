include_directories(include)

add_library(extension 
  include/extension/extension_global.hpp
  include/extension/iplugin_p.hpp
  include/extension/iplugin.hpp
  include/extension/plugin_spec_p.hpp
  include/extension/plugin_spec.hpp
)

welab_depends(extension PUBLIC DEPENDS macros)

target_link_libraries(extension
  Qt5::Core
  Qt5::Widgets
  Qt5::Gui
  macros
)

