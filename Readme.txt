README

CSC 433/533 - Computer Graphics
Program 3 - Solar System Simulator

Authors:
Ian Carlson, Christopher Smith

Program Description:

Instructions:

Features

Outstanding Bugs
 - Jupiters moon Europa incorrectly displays its texture map with a green hue to it. 
   This is possibly due to a conversion from jpeg to bmp since we changed the texture 
   map to the Earth's moon and it did not have any color hues at all.

 - Saturn's rings do not get mapped correctly to the gluCylinder.

 - When the user clicks and drags the mouse eraticaly around the screen with quick motions, 
   the camera will zoom in on a locked planet or out to infinity. Also the controls will lock 
   up or be barely function for moving the camera if this happens. If it zooms into a planet 
   it is impossible to get unstuck once locked at the origin of the planet, and the same with the zoom out to infinity.

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
