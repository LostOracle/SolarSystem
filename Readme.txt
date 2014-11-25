README

CSC 433/533 - Computer Graphics
Program 3 - Solar System Simulator

Authors:
Ian Carlson, Christopher Smith

Program Description:
    This program implements a simulation of the solar system. Because the solar system has
bafflingly huge proportions, we did make the following modifications to reality
in order to provide a more interesting simulation.

-Reduced orbital distances by a factor of 100
-Sun reduced in size by a 1/2
-Increased all planets size by a factor of 10
-Redcued Earth's Moon orbital distances by a factor of 2 while doubling Europa's

Instructions:
    The program initially starts out with a view point locked onto the sun.
    The user can right click to bring up a menu to Choose: 
 
        Display Method
            Wireframe
            Flat Shading
            Smooth Shading
            Textures
        Jump To (Locks camera to Planet)
            Any celestial body that is listed
        Unlock Camera
        Increment timestep
        Decrement timestep
        Increase Resolution
        Decrement Resolution
        Pause/Resume Simulation
        Single Step
        Help (Brings up this Readme)
        
    Always available:
        +,=      - Increases time_step
        -        - Decreases time_step
        P,p      - (Un)pauses simulation
        T,t      - Single time_step while paused simulation
        *        - Increases planet resolution
        /        - Decrease  planet resolution        

    Not locked on a planet:
        SpaceBar - Moves the camera up    
        C,c,X,x  - Moves the camera down
        A,a      - Moves the camera left
        D,d      - Moves the camera right
        W,w      - Moves the camera forward
        S,s      - Moves the camera backward
        E,e      - Rotates the camera clockwise
        Q,q      - Rotates teh camera counter clockwise

    Locked on a planet:
        Mouse Wheel Zoom
        Click and drag to rotate around planet
        
Features
    Required Features
        Different Color For Each Planet
        Text Labels
        Orbital Track
        User can change wireframe resolution
            animation speed
        Single Step Mode
        User can change between
            wireframe
            flat/smooth shading
            textures
        Circular Orbits
        Constant Trajectories
        Keyboard/Mouse Control
        Popup Menu
        Help (pops up this readme in linux)

    Extra Features
        Jump to Planet
        Track Planet In Motion
        Europa
        Easily reconfigurable planets without recompiling

Outstanding Bugs
 - Jupiters moon Europa incorrectly displays its texture map with a green hue to it. 
   This is possibly due to a conversion from jpeg to bmp since we changed the texture 
   map to the Earth's moon and it did not have any color hues at all.

 - Saturn's rings do not get mapped correctly to the gluCylinder.

 - When the user clicks and drags the mouse eraticaly around the screen with quick motions, 
   the camera will zoom in on a locked planet or out to infinity. Also the controls will lock 
   up or be barely function for moving the camera if this happens. If it zooms into a planet 
   it is impossible to get unstuck once locked at the origin of the planet, and the same with the zoom out to infinity.

 - The rotational velocity of the planets just doesn't look right, even though we checked out math a few times.

Other Notes:
Planet_info.inf is our information file that stores all the values needed to create a planet object. 
The first line in the file corresponds the number of Planets that are to be drawn with following lines being the actual information:

<num_planets>
<planet_name> <radius> <orbital_radius> <theta> <orbital_velocity> <phi> <rotation_speed> <tilt> <color_r> <color_g> <color_b> <planet_texture> <moons> <rings>

If moons is greater than 0 than that planet has n moons and the following n lines are moons that orbit the planet and will have the same information in their lines as above.
If rings is 1 than the following information will be on the same line as the planet they are associated with:

<inner_r> <outer_r> <ring_texture_file>

Our program will read up to num_planets top level planets, however we can dynamically create moons so we can have moons orbiting moons 
(The camera does not follow Moons of moons that well when locked onto a moon of a moon).
