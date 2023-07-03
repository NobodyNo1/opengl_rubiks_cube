@echo off
cd /d %~dp0
set "path_arg=%cd:\=/%"
build\win_rubiks_cube.exe "%path_arg%"
