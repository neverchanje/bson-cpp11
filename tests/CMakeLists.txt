
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

add_executable(BSONObjBuilder_unittest
        BSONObjBuilder_unittest.cc
        ../src/BSONObjBuilder.cc
        ../src/BufBuilder.cc
        ../src/Element.cc
        ../src/Type.cc
        ../src/BSONObj.cc)
target_link_libraries(BSONObjBuilder_unittest gtest gtest_main ${SILLY_LIBRARY} ${GLOG_LIBRARY})

add_executable(BSON_unittest
        BSON_unittest.cc
        ../src/BSON.cc
        ../src/BSONObjBuilder.cc
        ../src/BufBuilder.cc
        ../src/Status.cc
        ../src/BSONObj.cc
        ../src/Type.cc
        ../src/Element.cc)
target_link_libraries(BSON_unittest gtest gtest_main ${SILLY_LIBRARY} ${GLOG_LIBRARY})

add_executable(BSONElement_unittest
        BSONElement_unittest.cc
        ../src/Element.cc
        ../src/Type.cc)
target_link_libraries(BSONElement_unittest gtest gtest_main ${SILLY_LIBRARY} ${GLOG_LIBRARY})

add_executable(BSON_perftest
        BSON_perftest.cc
        ../src/BSON.cc
        ../src/BSONObjBuilder.cc
        ../src/BufBuilder.cc
        ../src/Status.cc
        ../src/BSONObj.cc
        ../src/Type.cc
        ../src/Element.cc)
target_link_libraries(BSON_perftest ${BENCHMARK_LIBRARY} ${GLOG_LIBRARY} ${SILLY_LIBRARY})

#add_executable(SharedBuffer_unittest
#        ../src/SharedBuffer.cc)
#target_link_libraries(SharedBuffer_unittest gtest gtest_main)
