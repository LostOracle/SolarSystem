/******************************************************************************
 * File: CameraFunctions.cpp
 * Authors: Ian Carlson, Chris Smith
 * Date: 11/24/14
 * Description: This file contains most of the camera manipulations functions
 *  which contain most of the heavy trig we did for planet tracking and handling
 *  user input for camera motion.
 * ***************************************************************************/
#include "../include/CameraFunctions.h"

//globals we need access to from orbit.cpp
extern camera_position CameraPos;
extern camera_velocity CameraVel;
extern Planet * target_lock;

/******************************************************************************
 * Function: init_camera
 * Authors: Ian Carlson, Chris Smith
 * Parameters: none
 * Returns: none
 * Description: This function initializes the camera such that it is looking
 *  "down" on the solar system from high up.
 * ***************************************************************************/
void init_camera()
{
    CameraPos.ey_x = 0;
    CameraPos.ey_y = 0;
    CameraPos.ey_z = 1000000;
    CameraPos.at_x = 0;
    CameraPos.at_y = 0;
    CameraPos.at_z = -1;
    CameraPos.up_x = 1;
    CameraPos.up_y = 0;
    CameraPos.up_z = 0;
    CameraPos.lf_x = 0;
    CameraPos.lf_y = 1;
    CameraPos.lf_z = 0;
    
    //Note: CameraVel is currently not used, we were playing with various control
    //methods, but left it here because it might be useful for smoother
    //control
    CameraVel.x = 0;
    CameraVel.y = 0;
    CameraVel.z = 0;
    CameraVel.roll = 0;
    CameraVel.pitch = 0;
    CameraVel.yaw = 0;
}

/******************************************************************************
 * Function: zoom
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to zoom
 * Returns: none
 * Description: This function moves the camera as if it were zooming in
 *  on the target planet. If zoom is between 0 and 1, it zooms in. If the value
 *  is greater than 1, it zooms out.
 * ***************************************************************************/
void zoom(double amount)
{
    long double x,y,z;
    //get the old vector
    x = CameraPos.ey_x - CameraPos.at_x;
    y = CameraPos.ey_y - CameraPos.at_y;
    z = CameraPos.ey_z - CameraPos.at_z;
    //scale it
    x = (1-amount)*x;
    y = (1-amount)*y;
    z = (1-amount)*z;
    //put the scaled version back in the camera variable
    CameraPos.ey_x = CameraPos.at_x + x;
    CameraPos.ey_y = CameraPos.at_y + y;
    CameraPos.ey_z = CameraPos.at_z + z;
}

/******************************************************************************
 * Function: move_forward
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to move
 * Returns: none
 * Description: This function moves the camera forward by some amount, based
 *  on the current view
 * ***************************************************************************/
void move_forward(double amount)
{
    //at - ey will give the forward-facing vector, but
    //we need to normalize it to make sure we're to scale
    double x,y,z;
    x = CameraPos.at_x - CameraPos.ey_x;
    y = CameraPos.at_y - CameraPos.ey_y;
    z = CameraPos.at_z - CameraPos.ey_z;
    double mag = x*x + y*y + z*z;
    mag = sqrt(mag);
    x/=mag;
    y/=mag;
    z/=mag;

    //now that we have a unit vector for the forward
    //direction, move along that vector
    CameraPos.at_x += x*amount;
    CameraPos.at_y += y*amount;
    CameraPos.at_z += z*amount;
    CameraPos.ey_x += x*amount;
    CameraPos.ey_y += y*amount;
    CameraPos.ey_z += z*amount;
}

/******************************************************************************
 * Function: move_up
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to move
 * Returns: none
 * Description: This function moves the camera up by some amount, based
 *  on the current view
 *  Moves down if amount is negative
 * ***************************************************************************/
void move_up(double amount)
{
    //update at and eye by amount*up
    //up is a unit vector, so the scaling is easy
    CameraPos.at_x += amount*CameraPos.up_x;
    CameraPos.at_y += amount*CameraPos.up_y;
    CameraPos.at_z += amount*CameraPos.up_z;
    CameraPos.ey_x += amount*CameraPos.up_x;
    CameraPos.ey_y += amount*CameraPos.up_y;
    CameraPos.ey_z += amount*CameraPos.up_z;
}

/******************************************************************************
 * Function: move_left
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to move
 * Returns: none
 * Description: This function moves the camera left by some amount, based
 *  on the current view
 *  Move right if amount is negative
 * ***************************************************************************/
void move_left(double amount)
{
    //update at and eye by amount*lf
    CameraPos.at_x += amount*CameraPos.lf_x;
    CameraPos.at_y += amount*CameraPos.lf_y;
    CameraPos.at_z += amount*CameraPos.lf_z;
    CameraPos.ey_x += amount*CameraPos.lf_x;
    CameraPos.ey_y += amount*CameraPos.lf_y;
    CameraPos.ey_z += amount*CameraPos.lf_z;
}

/******************************************************************************
 * Function: rotate_yaw
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to rotate
 * Returns: none
 * Description: This function rotates the camera about its up vector, updating
 *  lf ey and at in the CameraPos variable
 * ***************************************************************************/
void rotate_yaw(double amount)
{
    //get a unit vector in the forward direction
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    double mag = sqrt(x*x + y*y + z*z);
    x = x/mag;
    y = y/mag;
    z = z/mag;

    //if we have a target, strafe around it at a constant distance
    if(target_lock != NULL)
    {
        //invert the ey to at vector and rotate it around the up vector
        x = -x;
        y = -y;
        z = -z;
        rotate_about_axis3(x,y,z,
                           CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           amount);

        //also update the left vector
        rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                           CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           amount);

        //restore the new ey to at vector
        CameraPos.ey_x = CameraPos.at_x + x*mag;
        CameraPos.ey_y = CameraPos.at_y + y*mag;
        CameraPos.ey_z = CameraPos.at_z + z*mag;
    }

    //if we don't have a target, don't move, but rotate around the up vector
    else
    {
        rotate_about_axis3(x, y, z,
                           CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           amount);
        rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                           CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           amount);
        //reconstruct the actual at from ey + the new vector
        CameraPos.at_x = CameraPos.ey_x + x*mag;
        CameraPos.at_y = CameraPos.ey_y + y*mag;
        CameraPos.at_z = CameraPos.ey_z + z*mag;
    }
}

/******************************************************************************
 * Function: rotate_pitch
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to rotate
 * Returns: none
 * Description: This function rotates the camera about its left vector, updating
 *  up ey and at in the CameraPos variable
 * ***************************************************************************/
void rotate_pitch(double amount)
{
    //get a unit vector for the forward direction
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    double mag = sqrt(x*x + y*y + z*z);
    x = x/mag;
    y = y/mag;
    z = z/mag;

    //if we have a target, strafe around it, but keep a constant distance
    if(target_lock != NULL)
    {
        //rotate the up vector around the left vector by amount
        rotate_about_axis3(CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        //flip the forward vector so it points from at to ey
        x = -x;
        y = -y;
        z = -z;
        //rotate that vector around the left vector by amount
        rotate_about_axis3(x,y,z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        //update ey to reflect the new angles while keeping the same
        //magnitude
        CameraPos.ey_x = CameraPos.at_x + x*mag;
        CameraPos.ey_y = CameraPos.at_y + y*mag;
        CameraPos.ey_z = CameraPos.at_z + z*mag;
    }
    //If we don't have a target, don't move but rotate about the left vector
    else
    {
        //rotate the forward vectory around the left vector by amount
        rotate_about_axis3(x, y, z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        //rotate the up vector around the left vector by amount
        rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        //reconstruct the new at from ey + the new vector
        CameraPos.at_x = CameraPos.ey_x + x*mag;
        CameraPos.at_y = CameraPos.ey_y + y*mag;
        CameraPos.at_z = CameraPos.ey_z + z*mag;
    }
}

/******************************************************************************
 * Function: rotate_roll
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  amount - how much to rotate
 * Returns: none
 * Description: This function rotates the camera about its forward vector
 *  by amount
 * ***************************************************************************/
void rotate_roll(double amount)
{
    //get the magnitude of the vector for the forward direction
    double mag = 0;
    mag += (CameraPos.at_x-CameraPos.ey_x)*(CameraPos.at_x-CameraPos.ey_x);
    mag += (CameraPos.at_y-CameraPos.ey_y)*(CameraPos.at_y-CameraPos.ey_y);
    mag += (CameraPos.at_z-CameraPos.ey_z)*(CameraPos.at_z-CameraPos.ey_z);
    mag = sqrt(mag);

    //rotate about axis3 likes everything to be unit vectors
    
    //rotate up and left about the forward vector by amount
    rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                       (CameraPos.at_x-CameraPos.ey_x)/mag,
                       (CameraPos.at_y-CameraPos.ey_y)/mag,
                       (CameraPos.at_z-CameraPos.ey_z)/mag,
                       amount);
    rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                       (CameraPos.at_x-CameraPos.ey_x)/mag,
                       (CameraPos.at_y-CameraPos.ey_y)/mag,
                       (CameraPos.at_z-CameraPos.ey_z)/mag,
                       amount);
}

/******************************************************************************
 * Function: rotate_about_axis3
 * Authors: Ian Carlson
 * Parameters:
 *  x - the x component of the vector to be rotated
 *  y - the y component of the vector to be rotated
 *  z - the z component of the vector to be rotated
 *  x_hat - the x component of the vector to rotate around
 *  y_hat - the y component of the vector to rotate around
 *  z_hat - the z component fo the vector to rotate around
 *  rads - how much to rotate in radians
 * Returns: none
 * Description: This rotates <x,y,z> about <x_hat,y_hat,z_hat> by rads and
 *  returns the nex vector in x,y,z
 * Note1: I wrote the code myself, but the math is based on the linked
 *  wikipedia article on rotation matrices.
 * Note2: I wrote this for a 3D LEDdar reconstruction project a while back,
 *  which is why Chris didn't get any credit for it.
 * ***************************************************************************/
void rotate_about_axis3(double &x, double &y, double &z,
                        double x_hat, double y_hat, double z_hat, double rads)
{
    //http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    double new_x, new_y, new_z;
    
    //don't need to do this a bunch of times
    double c = cos(rads);
    double s = sin(rads);

    //I'm not even going to try explaining why this works, see the linked article
    new_x = x*(c + x_hat*x_hat*(1-c))
            + y*(x_hat*y_hat*(1-c) - z_hat*s)
            + z*(x_hat*z_hat*(1-c) + y_hat*s);

    new_y = x*(y_hat*x_hat*(1-c) + z_hat*s)
            + y*(c + y_hat*y_hat*(1-c))
            + z*(y_hat*z_hat*(1-c) - x_hat*s);

    new_z = x*(z_hat*x_hat*(1-c) - y_hat*s)
            + y*(z_hat*y_hat*(1-c) + x_hat*s)
            + z*(c + z_hat*z_hat*(1-c));

    x = new_x;
    y = new_y;
    z = new_z;
}
