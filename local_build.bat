@echo off
cd /d %~dp0
set "path_arg=%cd:\=/%"
mkdir build
g++ -g -std=c++17 -Iinclude -Llib/win/ -lglfw3 src/glad.c rubiks_cube.cpp -o build/win_rubiks_cube.exe
if %errorlevel% equ 0 (
    echo Compilation successful.
    build\win_rubiks_cube.exe "%path_arg%"
) else (
    echo Compilation failed. Please check for errors.
)
