cmake_minimum_required(VERSION 3.31)
project(ARCY)

set(CMAKE_CXX_STANDARD 20)

# Detect if we're using Emscripten
if("${CMAKE_CXX_COMPILER}" MATCHES "em\\+\\+")
    set(PLATFORM "Web")
endif()

set(RAYLIB_VERSION 5.5)

# Download raylib if needed
find_package(raylib ${RAYLIB_VERSION} QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED)
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

# Web-specific options
if(PLATFORM STREQUAL "Web")
    set_target_properties(ARCY PROPERTIES SUFFIX ".html")

    target_link_options(ARCY PRIVATE
            "-sUSE_GLFW=3"
            "-sASYNCIFY"
            "-sFULL_ES2=1"
            "-sFORCE_FILESYSTEM=1"
    )
endif()

# Backup of old CMAKE

#cmake_minimum_required(VERSION 3.31)
#project(ARCY)

#set(CMAKE_CXX_STANDARD 20)

#set(RAYLIB_VERSION 5.5)
#find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
#if (NOT raylib_FOUND) # If there's none, fetch and build raylib
#    include(FetchContent)
#    FetchContent_Declare(
#            raylib
#            DOWNLOAD_EXTRACT_TIMESTAMP OFF
#            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
#    )
#    FetchContent_GetProperties(raylib)
#    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
#        set(FETCHCONTENT_QUIET NO)
#        FetchContent_MakeAvailable(raylib)
#    endif()
#endif()
#
#add_executable(ARCY main.cpp
#        World/PerlinNoise.cpp
#        World/PerlinNoise.h
#        World/TextureCollection.cpp
#        World/TextureCollection.h
#        World/Player.cpp
#        World/Player.h
#        World/Globals.h
#        World/Money.cpp
#        World/Money.h
#        World/Pixel.h
#)

#file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR})

#target_link_libraries(ARCY PRIVATE raylib)