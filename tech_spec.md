Goal:

- Create 3x3 rubik's cube with free camera

Plan:

1. Sample

    - [x] Create simple box
    - [x] Add model rotation

2. Basics

    - [x] Create 8 boxes that will represent each element of rubik's cube
    - [x] Add shaders for it in order to color them

3. MVP

    - [x] Object Lighing
    - [x] Make each side of cubes different color
        - [x] Only color "outer" sides of rubik's cube
    - [ ] Make movement of cube sides
        - [ ] via code
        - [ ] UI keys
    - [x] Camera
        - [x] via code
        - [x] Movement via position of cursor
        - [x] Movement via cursor drag (GENERED BY ChatGPT need to learn how is done)

4. More
    - [ ] Controls
        - [ ] Camera
            - [ ] Cursor movement
            - [ ] Arrow keys
            - [ ] Dedicated keys to show single side (orth view)
        - [ ] Object
            - [ ] Select cube and swipe along its side to the direction of movement
            - [ ] Cube algoritm movements (e.g FURBLD)
                - [ ] Keyboard
                - [ ] UI keys
    - [ ] Sounds

5. Extra
    - [ ] Unwraped cube view (see all sides of cube)
    - [ ] Auto scrumble
    - [ ] Auto solve
    - [ ] Animations
        - [ ] Cube select animation
        - [ ] When one side is finished
        - [ ] Whole cube is finished

6. Even more extra
    - [ ] Add support of different dimention cubes



### Camera Impl
TODO: write explanation

### Side Rotation Imple

0. Setup initial positions.
    cube: id, sides: [id1, id2, id3, etc..]
1. Ray cast side selection
2. Ray cast next movement of mouse find direction of movement, and execute rotation after reaching some delta (probably within delta time if not update start pos?)
3. Find maching axis with the "swipe direction", find all cube that match with this value of axis. Rotated by that axis from current_angle to current_angle +- 90.
    a. While rotation is active block user interactions (maybe we can leave camera movement)
4. After rotation update positions.
    so it would look like: cube: id-> new_id, side: [id1 -> new_id2, id2 -> new_id2]
    a. need to calculate new ids somehow (TODO: find how to)

