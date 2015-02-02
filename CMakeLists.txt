cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wnon-virtual-dtor")

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pthread")
endif(UNIX)

if(CYGWIN OR MINGW)
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " " CMAKE_C_CREATE_SHARED_LIBRARY "${CMAKE_C_CREATE_SHARED_LIBRARY}")
    string(REGEX REPLACE "-Wl,--out-implib,[^ ]+ " " " CMAKE_CXX_CREATE_SHARED_LIBRARY "${CMAKE_CXX_CREATE_SHARED_LIBRARY}")
endif(CYGWIN OR MINGW)

project(DSPatch)

file(GLOB srcs src/*.cpp)
file(GLOB hdrs include/*.h)
file(GLOB in_hdrs include/dspatch/*.h)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
)

# Build shared (LGPL)
add_library(
    ${PROJECT_NAME} SHARED
    ${srcs}
    ${hdrs}
    ${in_hdrs}
)

target_link_libraries(
    ${PROJECT_NAME}
    -static-libgcc
    -static-libstdc++
)

# Link pthread and dl on Unix
if(UNIX)
    target_link_libraries(
        ${PROJECT_NAME}
        pthread
        dl
    )
endif(UNIX)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION lib
)

install(
    FILES ${hdrs}
    DESTINATION include
)

install(
    FILES ${in_hdrs}
    DESTINATION include/dspatch
)

option(BUILD_EXAMPLES "Build Examples" OFF)
option(BUILD_DOC "Build Documentation" OFF)

if(${BUILD_EXAMPLES})
    add_subdirectory(example)
    add_subdirectory(tutorial)
endif(${BUILD_EXAMPLES})

if(${BUILD_DOC})
    add_subdirectory(doc)
endif(${BUILD_DOC})
