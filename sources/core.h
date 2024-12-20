#pragma once

#include "raylib.h"

#ifdef __EMSCRIPTEN__
#define WASM
#include <emscripten.h>
#include <emscripten/html5.h>
#define WASM_ONLY(code) code
#define NATIVE_ONLY(code)

#else
#define WASM_ONLY(code)
#define NATIVE_ONLY(code) code
#endif

