cmake_minimum_required(VERSION 3.8)

include(FetchContent)

FetchContent_Declare(
  uWebSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uWebSockets
  GIT_TAG v20.37.0
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uWebSockets_content)


find_package(OpenSSL REQUIRED)


add_library(uWebSockets INTERFACE)
target_include_directories(uWebSockets INTERFACE ${uWebSockets_content_SOURCE_DIR}/src/)
target_link_libraries(uWebSockets INTERFACE uSockets zlib OpenSSL::SLL)
