# Rubik's Cube in OpenGL

![Screenshot](/screenshots/screenshot.jpg)

## Description

Project made for self-practice in OpenGL (3.3).

Currently supports only 3x3 rubik's cube.

Fuctionality:

- Camera

- Swipe controls

### Controls

- Hold RMB (right mouse button) and drag to rotate the camera

- Select one of the cube sides using LMB, and drag/swipe to the direction where you want to rotate

## Running and Building

From the project root

Mac

``` shell script

# to execute existing build
sh ./run.sh

# or to build locally
sh ./local_build.sh
```

For Window

``` batch script

:: to execute existing build
.\run.bat

:: or to build locally
.\local_build.bat
```

## Dependencies

- OpenGL 3.3
- GLM
- GLEW
- GLAD

## Todo

- [ ] support up to 10x10 rubik's cube
- [ ] keyboard controls
- [ ] auto scumbler
- [ ] auto solver
- [ ] posh ui

### Extra

Special thanks for ChatGPT for explaining the camera control logic.
