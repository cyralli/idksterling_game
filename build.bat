@echo off
:: > Setup required Enviroment
set root=C:\Users\cyralli\Documents\idksterling_game\src
set mainname=main
set mainfile=C:\Users\cyralli\Documents\idksterling_game\src\main.c
set raylib_path=C:\raylib\raylib
set compiler_path=C:\raylib\w64devkit\bin
:: Let's temporarily put our compiler path in PATH.
set PATH=%PATH%;%compiler_path%
set CC=gcc
set cflags=%raylib_path%\src\raylib.rc.data -s -static -O2 -std=c99 -Wall -I%raylib_path%\src -Iexternal -DPLATFORM_DESKTOP
set ldflags=-lraylib -lopengl32 -lgdi32 -lwinmm
cd %root%
:: > Clean latest build
cmd /c if exist "%mainname%.exe" del "%mainname%.exe"
:: > Compile program
%CC% --version
%CC% -o %mainname%.exe %mainfile% %cflags% %ldflags%
:: > Execute Program
cmd /c if exist "%mainname%.exe" "%mainname%.exe"