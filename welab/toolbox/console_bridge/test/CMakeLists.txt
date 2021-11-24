set (tests
  console_test.cpp 
)

#################################################
# Build all the tests
foreach(gtest_src_file ${tests})
  string(REGEX REPLACE ".cpp" "" BINARY_NAME ${gtest_src_file})
  add_executable(${BINARY_NAME} ${gtest_src_file})

  add_dependencies(${BINARY_NAME}
    console_bridge)
  
  target_link_libraries(${BINARY_NAME}
    ${GTEST_LIBRARIES}
    console_bridge
  )

  if (UNIX AND NOT ANDROID AND NOT QNX)
    target_link_libraries(${BINARY_NAME} pthread)
  endif()

  add_test(NAME    ${BINARY_NAME}
           COMMAND ${BINARY_NAME})
  
  set_tests_properties(${BINARY_NAME} PROPERTIES TIMEOUT 240)
endforeach()
