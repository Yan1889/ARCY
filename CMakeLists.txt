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
        World/Pixel.cpp
)

file(COPY ${CMAKE_SOURCE_DIR}/src/images DESTINATION ${CMAKE_BINARY_DIR})

set(CITY_IMAGE ${CMAKE_CURRENT_SOURCE_DIR}/src/images/city.png)

# set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ${CITY_IMAGE}@/city.png")

target_link_libraries(ARCY PRIVATE raylib)

# Web-specific options
if(PLATFORM STREQUAL "Web")
    set_target_properties(ARCY PROPERTIES SUFFIX ".html")

    target_link_options(ARCY PRIVATE
            "-sUSE_GLFW=3"
            "-sASYNCIFY"
            "-sFULL_ES2=1"
            "-sFORCE_FILESYSTEM=1"
            "-sINITIAL_MEMORY=536870912"      # 512MB in bytes
            "-sALLOW_MEMORY_GROWTH=1"
            "-sASSERTIONS=1"
            "--preload-file src/images"
    )
endif()

# How to build a game on web

# 1. Use this CMakeList below
# 2. Maybe manually set to WEB in Raylib? (dont know if the code already does that or not)
# 3. Install ninja and put it in C: (rename it to just "ninja")
# 3. Temporarily create a PATH for ninja in Powershell: $env:PATH = "C:\ninja;$env:PATH"
# 4. Install CMake with MingW64 (with pacman)
# 4. Temporarily create a PATH for cmake in Powershell: $env:PATH = "C:\msys64\mingw64\bin;$env:PATH"
# 5. Go to Powershell and go to the project with: cd C:\ [...]
# 6. In the best case, emcmake is already in project (idk how to install it there if it shouldnt be there)
# 7. Use these two lines in Powershell in the folder of the project (cd C:\ [...]) to create the build:
#     emcmake cmake -B build_web -G Ninja -DCMAKE_BUILD_TYPE=Release -DPLATFORM=Web
#     (a folder called "build_web" should be created in the project)
#     emmake cmake --build build_web
# 8. Done you have a lot of stuff but you only need the .html, .js and .wasm files
# 9. Make a index.html and write the same stuff like in this project

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