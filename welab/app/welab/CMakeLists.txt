include_directories(include)

add_executable(welab
  src/main.cpp
)

welab_depends(welab PUBLIC DEPENDS extension)

target_link_libraries(welab
  extension
)