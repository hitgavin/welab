find_package(pybind11 CONFIG REQUIRED)

pybind11_add_module(core MODULE
  src/core/core.cpp
  src/core/logger/logger.cpp
)

target_link_libraries(core PRIVATE welab_core)

install(TARGETS core
        DESTINATION ${CMAKE_INSTALL_SCRIPTDIR}/core
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)