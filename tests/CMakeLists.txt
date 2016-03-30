
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_subdirectory(lib/googletest/googletest)

add_executable(Status_unittest
        Status_unittest.cc
        ../src/Status.cc)
target_link_libraries(Status_unittest gtest gtest_main)

add_executable(BufBuilder_unittest
        BufBuilder_unittest.cc
        ../src/BufBuilder.cc)
target_link_libraries(BufBuilder_unittest gtest gtest_main ${SILLY_LIBRARY})