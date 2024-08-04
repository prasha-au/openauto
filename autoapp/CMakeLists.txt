
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

file(GLOB_RECURSE BIN_SRC_FILES
        autoapp.cpp
        assets/resources.qrc
        *.cpp
        *.ui
        *.hpp
        ${CMAKE_SOURCE_DIR}/include/autoapp/*.hpp
)

add_executable(autoapp ${BIN_SRC_FILES})


target_include_directories(autoapp PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_CURRENT_BINARY_DIR}
        )

target_link_libraries(autoapp
        openauto
        asound
        )

