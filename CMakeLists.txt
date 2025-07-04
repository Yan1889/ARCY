cmake_minimum_required(VERSION 3.31)
project(ARCY)

set(CMAKE_CXX_STANDARD 20)

set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_MakeAvailable(raylib)
    endif()
endif()

add_executable(ARCY main.cpp
        World/PerlinNoise.cpp
        World/PerlinNoise.h
        World/TextureCollection.cpp
        World/TextureCollection.h
        World/Player.cpp
        World/Player.h
        World/Globals.h
        World/Money.cpp
        World/Money.h
        World/Pixel.h
)

file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR})

target_link_libraries(ARCY PRIVATE raylib)