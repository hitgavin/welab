find_package(GTest REQUIRED CONFIG)
include_directories(${GTest_INCLUDE_DIRS})

add_executable(logger_test logger_test.cpp)
target_link_libraries(logger_test welab_core ${GTest_LIBRARIES})
add_test(NAME logger_test COMMAND logger_test)