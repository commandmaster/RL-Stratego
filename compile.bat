@echo on
REM Get all .cpp files from the "sources" directory
setlocal enabledelayedexpansion
set FILES=
for %%f in (sources\*.cpp) do set FILES=!FILES! %%f
for %%f in (sources\WasmOnly\*.cpp) do set FILES=!FILES! %%f

REM Print out the files being passed to em++
echo !FILES!

REM Run the emcc command and output errors to a log file
powershell -Command "em++ -o server/public/stratego.html !FILES! -Wall -std=c++20 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os -I. -I C:/raylib/src -I C:/raylib/src/external -L. -L C:/raylib/src -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file minshell.html C:/raylib/src/web/libraylib.a -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=[\"_free\",\"_malloc\",\"_main\",\"_handleMessageFromJS\",\"_handleJsonFromJS\",\"_handleBinaryFromJS\"]' -s EXPORTED_RUNTIME_METHODS=ccall"
