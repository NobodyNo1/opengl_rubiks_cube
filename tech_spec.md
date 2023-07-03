# Plan and Approaches

## Goal

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
        - [x] "swipe" action
        - [ ] via code
        - [ ] UI keys
    - [x] Camera
        - [x] via code
        - [x] Movement via position of cursor
        - [x] Movement via cursor drag (GENERED BY ChatGPT need to learn how is done)
    - [ ] Limit camera "pitch" angle (so it doesn't reach to -1 and 1)
        - [ ] Fix camera "yaw" rotation angle speed at higher "pitch" angles

4. More
    - [x] Controls
        - [ ] Camera
            - [x] Cursor movement
            - [ ] Arrow keys
            - [ ] Dedicated keys to show single side (orth view)
        - [ ] Object
            - [x] Select cube and swipe along its side to the direction of movement
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


## Approaches

### Camera Impl

TODO: write explanation

### "Swipe" Side Rotation implementation

0. Setup initial positions.
    cube: id, sides: [id1, id2, id3, etc..]
1. Ray cast side selection
    a. Filter only sides that is outside
    b. Filter sides that are visible from the camera (using yaw and pitch of the camera)
    c. Loop through intersection and select with minimal intersection magnitude (distance from camera?)
    d. If suitable found write down as start mouse position for a swipe else skip action
2. Ray cast next movement of mouse find direction of movement, and execute rotation after reaching some delta (probably within delta time if not update start pos?)
3. Find maching axis with the "swipe direction", find all cube that match with this value of axis (depends on selected cube side and/or camera rotation).
    a. While rotation is active block user interactions (maybe we can leave camera movement)
4. After rotation update positions.
    so it would look like: cube: id-> new_id, side: [id1 -> new_id2, id2 -> new_id2]
    a. find id's that will be rotated. (Find positions that matched clicked items coordinates at N that where N is value of M coordinates. M - axis of rotation)
        1. And rotate side idx (and colors) before full matrix rotation
    b. Generate 3x3 matrix containing this ids and rotate them by +-90
    c. Update indices matrix with rotated one
