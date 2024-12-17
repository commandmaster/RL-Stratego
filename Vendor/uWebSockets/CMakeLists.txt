cmake_minimum_required(VERSION 3.24)
include(FetchContent)

# Fetch Zlib
FetchContent_Declare(
  zlib_content
  GIT_REPOSITORY https://github.com/madler/zlib
  GIT_TAG v1.2.13  # Replace with the desired Zlib version
  GIT_SHALLOW ON
)
FetchContent_MakeAvailable(zlib_content)

# Fetch uSockets
FetchContent_Declare(
  uSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uSockets
  GIT_TAG v0.8.5
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uSockets_content)

# Fetch uWebSockets
FetchContent_Declare(
  uWebSockets_content
  GIT_REPOSITORY https://github.com/uNetworking/uWebSockets
  GIT_TAG v20.37.0
  GIT_SHALLOW ON
  GIT_SUBMODULES ""
)
FetchContent_MakeAvailable(uWebSockets_content)

# Add Zlib library
add_library(ZLIB STATIC ${zlib_content_SOURCE_DIR}/adler32.c
                         ${zlib_content_SOURCE_DIR}/compress.c
                         ${zlib_content_SOURCE_DIR}/crc32.c
                         ${zlib_content_SOURCE_DIR}/deflate.c
                         ${zlib_content_SOURCE_DIR}/gzclose.c
                         ${zlib_content_SOURCE_DIR}/gzlib.c
                         ${zlib_content_SOURCE_DIR}/gzread.c
                         ${zlib_content_SOURCE_DIR}/gzwrite.c
                         ${zlib_content_SOURCE_DIR}/inflate.c
                         ${zlib_content_SOURCE_DIR}/infback.c
                         ${zlib_content_SOURCE_DIR}/inftrees.c
                         ${zlib_content_SOURCE_DIR}/inffast.c
                         ${zlib_content_SOURCE_DIR}/trees.c
                         ${zlib_content_SOURCE_DIR}/uncompr.c
                         ${zlib_content_SOURCE_DIR}/zutil.c)
target_include_directories(ZLIB PUBLIC ${zlib_content_SOURCE_DIR})

# Add uSockets library
file(GLOB_RECURSE uSockets_SOURCES ${uSockets_content_SOURCE_DIR}/src/*.c)
add_library(uSockets STATIC ${uSockets_SOURCES})
target_include_directories(uSockets PUBLIC ${uSockets_content_SOURCE_DIR}/src)
target_compile_definitions(uSockets PRIVATE LIBUS_NO_SSL)

# Add uWebSockets library
add_library(uWebSockets INTERFACE)
target_include_directories(uWebSockets INTERFACE ${uWebSockets_content_SOURCE_DIR}/src/)
target_link_libraries(uWebSockets INTERFACE uSockets ZLIB)
