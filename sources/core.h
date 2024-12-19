#pragma once

#include "movement.h"
#include "raylib.h"

#ifdef __EMSCRIPTEN__
#define WASM
#include <emscripten.h>
#include <emscripten/html5.h>
#define WASM_ONLY(code) code
#define NATIVE_ONLY(code)
#else
#include <sio_client.h>
#define WASM_ONLY(code)
#define NATIVE_ONLY(code) code
#endif

