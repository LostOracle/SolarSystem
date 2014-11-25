#include "../include/CameraFunctions.h"

extern camera_position CameraPos;
extern camera_velocity CameraVel;
extern Planet * target_lock;
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

    CameraVel.x = 0;
    CameraVel.y = 0;
    CameraVel.z = 0;
    CameraVel.roll = 0;
    CameraVel.pitch = 0;
    CameraVel.yaw = 0;
}

void zoom(double amount)
{
    //move 5% closer to the target object, if any
    long double x,y,z;
    x = CameraPos.ey_x - CameraPos.at_x;
    y = CameraPos.ey_y - CameraPos.at_y;
    z = CameraPos.ey_z - CameraPos.at_z;
    x = (1-amount)*x;
    y = (1-amount)*y;
    z = (1-amount)*z;
    CameraPos.ey_x = CameraPos.at_x + x;
    CameraPos.ey_y = CameraPos.at_y + y;
    CameraPos.ey_z = CameraPos.at_z + z;
}

void move_forward(double amount)
{
    //at-ey will give the forward-facing vector, but
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

void move_up(double amount)
{
    //update at and eye by amount*lf
    CameraPos.at_x += amount*CameraPos.up_x;
    CameraPos.at_y += amount*CameraPos.up_y;
    CameraPos.at_z += amount*CameraPos.up_z;
    CameraPos.ey_x += amount*CameraPos.up_x;
    CameraPos.ey_y += amount*CameraPos.up_y;
    CameraPos.ey_z += amount*CameraPos.up_z;
}

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

//when the user wants to rotate left/right, we rotate
//about the up vector, updating the left and at
//vectors
void rotate_yaw(double amount)
{
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    double mag = sqrt(x*x + y*y + z*z);
    x = x/mag;
    y = y/mag;
    z = z/mag;

    //maintain distance, but move eye left or right
    if(target_lock != NULL)
    {
        //invert the ey to at vector and rotate it around
        //the vector 0,0,1
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
        CameraPos.ey_x = CameraPos.at_x + x*mag;
        CameraPos.ey_y = CameraPos.at_y + y*mag;
        CameraPos.ey_z = CameraPos.at_z + z*mag;
    }

    //rotate at and up about left by amount*camera_rotate_coeff
    //extract vector from at - ey
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

void rotate_pitch(double amount)
{
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    double mag = sqrt(x*x + y*y + z*z);
    x = x/mag;
    y = y/mag;
    z = z/mag;
    //invert x,y,z and rotate that vector about lf then move ey
    if(target_lock != NULL)
    {
        rotate_about_axis3(CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount); 
        x = -x;
        y = -y;
        z = -z;
        rotate_about_axis3(x,y,z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        CameraPos.ey_x = CameraPos.at_x + x*mag;
        CameraPos.ey_y = CameraPos.at_y + y*mag;
        CameraPos.ey_z = CameraPos.at_z + z*mag;
                    
    }
    //rotate at and up about left by amount*camera_rotate_coeff
    //extract vector from at - ey
    else
    {
        rotate_about_axis3(x, y, z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount);
        //reconstruct the actual at from ey + the new vector
        CameraPos.at_x = CameraPos.ey_x + x*mag;
        CameraPos.at_y = CameraPos.ey_y + y*mag;
        CameraPos.at_z = CameraPos.ey_z + z*mag;
    }
}

void rotate_roll(double amount)
{
    double mag = 0;
    mag += (CameraPos.at_x-CameraPos.ey_x)*(CameraPos.at_x-CameraPos.ey_x);
    mag += (CameraPos.at_y-CameraPos.ey_y)*(CameraPos.at_y-CameraPos.ey_y);
    mag += (CameraPos.at_z-CameraPos.ey_z)*(CameraPos.at_z-CameraPos.ey_z);
    mag = sqrt(mag);

    //rotate up and left about at by amount*camera_rotate_coeff
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
void rotate_about_axis3(double &x, double &y, double &z, 
                        double x_hat, double y_hat, double z_hat, double rads)
{
    //http://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
    double new_x, new_y, new_z;
    //don't need to do this a bunch of times
    double c = cos(rads);
    double s = sin(rads);

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
