include(GNUInstallDirs)

add_library(macros INTERFACE)
target_include_directories(macros INTERFACE include)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})