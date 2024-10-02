
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)


file(GLOB_RECURSE OPENAUTO_SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE OPENAUTO_INCLUDE_FILES ${CMAKE_SOURCE_DIR}/include/openauto/*.hpp)


add_library(openauto SHARED
        ${OPENAUTO_SRC_FILES}
        ${OPENAUTO_INCLUDE_FILES}
)

target_include_directories(openauto SYSTEM PUBLIC
        ${QTGSTREAMER_INCLUDE_DIR}
        ${GST_INCLUDE_DIRS}
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_BINARY_DIR}
)

target_link_libraries(openauto PRIVATE
        ${QTGSTREAMER_LIBRARY}
        ${GST_LIBRARIES}
        ${Qt5QuickWidgets_LIBRARIES}
        ${QTGSTREAMER_QUICK_LIBRARY}
)

target_link_libraries(openauto PUBLIC
        libusb
        Threads::Threads
        ${Boost_LIBRARIES}
        aasdk
        aasdk_proto
        Qt5::Bluetooth
        Qt5::MultimediaWidgets
        ${Protobuf_LIBRARIES}
)
