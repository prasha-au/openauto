# include(CPM)
include(FetchContent)


set(aasdk_patch git apply ${CMAKE_CURRENT_SOURCE_DIR}/patches/aasdk_openssl-fips-fix.patch)

FetchContent_Declare(aasdk
        GIT_REPOSITORY https://github.com/OpenDsh/aasdk
        GIT_TAG 1bc0fe69d5f5f505c978a0c6e32c860e820fa8f6
        PATCH_COMMAND ${aasdk_patch}
        UPDATE_DISCONNECTED 1
        )
FetchContent_MakeAvailable(aasdk)

include_directories(${aasdk_SOURCE_DIR}/include ${aasdk_BINARY_DIR})

file(GLOB_RECURSE aasd_EXCLUDE_MOC ${aasdk_BINARY_DIR}/**/*.c*)
set_property(SOURCE ${aasd_EXCLUDE_MOC} PROPERTY SKIP_AUTOMOC ON)

set_target_properties(aasdk_proto PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set_target_properties(aasdk PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)




# include(CPM)
# include(CMakePrintHelpers)

# CPMAddPackage(
#     NAME aasdk
#     GIT_REPOSITORY "https://github.com/OpenDsh/aasdk"
#     GIT_TAG "1bc0fe69d5f5f505c978a0c6e32c860e820fa8f6"
#     # PATCHES ${CMAKE_CURRENT_SOURCE_DIR}/patches/aasdk_openssl-fips-fix.patch
# )


# include_directories(${aasdk_SOURCE_DIR}/include ${aasdk_BINARY_DIR})

# file(GLOB_RECURSE aasd_EXCLUDE_MOC ${aasdk_BINARY_DIR}/**/*.c*)
# set_property(SOURCE ${aasd_EXCLUDE_MOC} PROPERTY SKIP_AUTOMOC ON)

# set_target_properties(aasdk PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

