#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define WASM_ONLY(code) code
#define NATIVE_ONLY(code)
#else
#include <sio_client.h>
#define WASM_ONLY(code)
#define NATIVE_ONLY(code) code
#endif

