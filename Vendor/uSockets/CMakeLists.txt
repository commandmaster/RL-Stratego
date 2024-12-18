cmake_minimum_required(VERSION 3.24)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
FetchContent_Declare(
  uSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uSockets.git
  GIT_TAG v0.8.5
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uSockets_content)

file(GLOB_RECURSE SOURCES ${uSockets_content_SOURCE_DIR}/src/*.c)
add_library(uSockets STATIC ${SOURCES})
target_include_directories(uSockets PUBLIC ${uSockets_content_SOURCE_DIR}/src)
target_compile_definitions(uSockets PRIVATE LIBUS_NO_SSL)
