cmake_minimum_required(VERSION 3.20)
project(Asterogue VERSION 0.1)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(RAYLIB_VERSION 5.0)
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
    FetchContent_Populate(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
    add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
  endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

file(GLOB_RECURSE SOURCE_LIST
    "include/*.h"
    "src/*.cpp"
    "../raygui/src/*h"
)

foreach(_source IN ITEMS ${SOURCE_LIST})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

include_directories(
    "include"
    "src"
    "../raygui/src"
)

add_compile_definitions(_DEBUG)
add_compile_definitions(RAYGUI_IMPLEMENTATION)
add_executable(Asterogue
    "${SOURCE_LIST}"
)
target_link_libraries(Asterogue
    raylib
)

if (${PLATFORM} STREQUAL "Web")
    set_target_properties(Asterogue PROPERTIES
        SUFFIX ".html"
        LINK_FLAGS "--preload-file ../../resources --shell-file ../../minshell.html"
        COMPILE_FLAGS "-Wno-narrowing"
    )
endif()