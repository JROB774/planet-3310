@echo off

set CompileFlags=-I src -m64 -bench
set BuildMode=debug

if %BuildMode% == debug set CompileFlags=%CompileFlags% -g -D NK_DEBUG

if not exist bin\win32 mkdir bin\win32
.tcc\tcc %CompileFlags% -o bin\win32\NOKIA.EXE src\MAINWIN.C
