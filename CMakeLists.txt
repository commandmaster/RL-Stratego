cmake_minimum_required(VERSION 3.8)
project(stratego CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add Raylib
include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # Don't build supplied examples
set(BUILD_GAMES    OFF CACHE BOOL "" FORCE) # Don't build supplied example games

if(BUILD_FOR_WASM)
    set(PLATFORM_WEB ON CACHE BOOL "" FORCE) # Enable Web platform for Raylib
endif()

FetchContent_Declare(
    raylib
    GIT_REPOSITORY "https://github.com/raysan5/raylib.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)

FetchContent_MakeAvailable(raylib)

find_package(Boost REQUIRED)
find_package(sioclient CONFIG REQUIRED)
  
# Add source files
file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_LIST_DIR}/sources/*.cpp") # Define PROJECT_SOURCES as a list of all source files
set(PROJECT_INCLUDE "${CMAKE_CURRENT_LIST_DIR}/sources/") # Define PROJECT_INCLUDE to be the path to the include directory of the project

# Declare executable
add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_INCLUDE})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_link_libraries(${PROJECT_NAME} LINK_PUBLIC ${Boost_LIBRARIES})
target_link_libraries(${PROJECT_NAME} PRIVATE sioclient::sioclient sioclient::sioclient_tls)


# Wasm compile script
set(BATCH_FILE_PATH "${CMAKE_SOURCE_DIR}/compile.bat")

# This will build for WASM
add_custom_command(
    TARGET ${PROJECT_NAME}
    POST_BUILD
    COMMAND ${BATCH_FILE_PATH}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)


# Set asset path
if(BUILD_FOR_WASM)
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="./assets/") # Use relative path for WASM
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3 -s ASSERTIONS=1 -s WASM=1 -s ASYNCIFY")
else()
    target_compile_definitions(${PROJECT_NAME} PUBLIC ASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/assets/") # Use absolute path for native builds
endif()

# Add Emscripten linker flags for WebAssembly builds
if(BUILD_FOR_WASM)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s USE_GLFW=3 -s ALLOW_MEMORY_GROWTH=1 -s USE_WEBGL2=1")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s USE_GLFW=3")
endif()

# Copy assets directory to build output for WASM builds
if(BUILD_FOR_WASM)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets" "${CMAKE_CURRENT_BINARY_DIR}/assets"
    )
endif()
