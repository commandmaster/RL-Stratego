@echo on
REM Run the emcc command and output errors to a log file
powershell -Command "emcc -o stratego.html sources/main.cpp -Wall -std=c++20 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I C:/raylib/src -I C:/raylib/src/external -L. -L C:/raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1  --shell-file C:/raylib/src/minshell.html C:/raylib/src/web/libraylib.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall"

REM Pause to show output or errors
pause
