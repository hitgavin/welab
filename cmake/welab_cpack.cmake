################################################################################
# Find available package generators

# DEB
if("${CMAKE_SYSTEM}" MATCHES "Linux")
  find_program(DPKG_PROGRAM dpkg)
  if(EXISTS ${DPKG_PROGRAM})
    list(APPEND CPACK_GENERATOR "DEB")
  endif()

  find_program(RPMBUILD_PROGRAM rpmbuild)
endif()

list(APPEND CPACK_SOURCE_GENERATOR "TBZ2")
list(APPEND CPACK_SOURCE_GENERATOR "ZIP")
list(APPEND CPACK_SOURCE_IGNORE_FILES ";Ogre.log;TODO;/.hg/;.swp$;/build/;.hgtags")

include(InstallRequiredSystemLibraries)

set(DEBIAN_PACKAGE_DEPENDS "libogre-dev, libfreeimage-dev, libqt5-dev, libprotobuf-dev, libprotoc-dev, libtbb2, libboost-all-dev")
set(RPM_PACKAGE_DEPENDS "libogre-dev, libfreeimage-dev, libqt5-dev, libprotobuf-dev, libprotoc-dev, libtbb2, libboost-all-dev")
set(WELAB_CPACK_CFG_FILE "${PROJECT_BINARY_DIR}/cpack_options.cmake")
