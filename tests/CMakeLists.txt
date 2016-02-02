
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
        ../src/Slice.cc)
target_link_libraries(DataOutput_unittest gtest gtest_main)

add_executable(BufBuilder_unittest
        BufBuilder_unittest.cc
        ../src/DataView.cc
        ../src/Slice.cc
        ../src/BufBuilder.cc)
target_link_libraries(BufBuilder_unittest gtest gtest_main)

add_executable(DataView_unittest
        DataView_unittest.cc
        ../src/DataView.cc)
target_link_libraries(DataView_unittest gtest gtest_main)
