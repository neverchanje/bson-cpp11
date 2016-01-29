
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
add_subdirectory(lib/googletest/googletest)

add_executable(Slice_unittest
        Slice_unittest.cc
        ../src/Slice.cc)
target_link_libraries(Slice_unittest gtest gtest_main)

add_executable(Status_unittest
        Status_unittest.cc
        ../src/Status.cc
        ../src/Slice.cc)
target_link_libraries(Status_unittest gtest gtest_main)

add_executable(DataOutput_unittest
        DataOutput_unittest.cc
        ../src/DataOutput.cc
        ../src/Fmt.h
        ../src/FixedBuffer.h
        )
target_link_libraries(DataOutput_unittest gtest gtest_main)
