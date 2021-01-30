@echo off

set CompileFlags=-I src -m64 -bench
set BuildMode=debug

if %BuildMode% == debug set CompileFlags=%CompileFlags% -g -D NK_DEBUG

.tcc\tcc -run src\DATPAK.C dat\FONT.PNG src\FONT.C NK_FONT
.tcc\tcc -run src\DATPAK.C dat\TILESET.PNG src\TILESET.C NK_TILESET

if not exist bin\win32 mkdir bin\win32
.tcc\tcc %CompileFlags% -lwinmm -o bin\win32\NOKIA.EXE src\WIN32.C
