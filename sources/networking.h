#include "core.h"

#ifdef WASM
#include "WasmOnly/networking.h"	
#else
#include "NativeOnly/networking.h"
#endif 

