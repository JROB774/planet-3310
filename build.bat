@echo off

set CompileFlags=-I src -m64 -bench
set BuildMode=debug

if %BuildMode% == debug set CompileFlags=%CompileFlags% -g -D NK_DEBUG

.tcc\tcc -run src\DATAPACK.C dat\FONT.PNG src\FONT.C NK_FONT
.tcc\tcc -run src\DATAPACK.C dat\TILE.PNG src\TILE.C NK_TILE

if not exist bin\win32 mkdir bin\win32
.tcc\tcc %CompileFlags% -lwinmm -o bin\win32\NOKIA.EXE src\WINMAIN.C
