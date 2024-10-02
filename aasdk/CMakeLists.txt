

file(GLOB_RECURSE AASDK_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE AASDK_INCLUDE_FILES ${CMAKE_SOURCE_DIR}/include/aasdk/*.hpp)

file(GLOB_RECURSE AASDK_TEST_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.ut.cpp)
list(REMOVE_ITEM AASDK_SRC_FILES ${AASDK_TEST_SRC_FILES})


add_library(aasdk SHARED
        ${AASDK_SRC_FILES}
        ${AASDK_INCLUDE_FILES})

add_dependencies(aasdk aasdk_proto)

target_link_libraries(aasdk
        aasdk_proto
        libusb
        ${Boost_LIBRARIES}
        ${PROTOBUF_LIBRARIES}
        ${OPENSSL_LIBRARIES}
        ${WINSOCK2_LIBRARIES})

target_include_directories(aasdk PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

install(TARGETS aasdk DESTINATION lib)


