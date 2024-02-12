find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(map MODULE
  src/map/map.cpp
  src/map/costmap/costmap_2d.cpp
)

target_link_libraries(map PRIVATE welab_map)

install(TARGETS map
        DESTINATION ${CMAKE_INSTALL_SCRIPTDIR}/map
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)