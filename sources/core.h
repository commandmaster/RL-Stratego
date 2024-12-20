#pragma once

#include "raylib.h"

#ifdef __EMSCRIPTEN__
#define WASM
#include <emscripten.h>
#include <emscripten/html5.h>
#include "WasmOnly/networking.h"
#define WASM_ONLY(code) code
#define NATIVE_ONLY(code)

#else
#include "NativeOnly/networking.h" 
#define WASM_ONLY(code)
#define NATIVE_ONLY(code) code
#endif

