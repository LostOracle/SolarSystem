/************************************************************************
* File: Camera_Structs.h
* Author: Chris Smith, Ian Carlson
* Description: Camera position and speed structs
* Date: 20 Nov 2014
************************************************************************/
#ifndef CAMERA_STRUCTS_H_
#define CAMERA_STRUCTS_H_


struct camera_position
{
    double ey_x,ey_y,ey_z;
    double at_x,at_y,at_z;
    double up_x,up_y,up_z;
    double lf_x,lf_y,lf_z;
};

struct camera_velocity
{
    double x,y,z;
    double roll,pitch,yaw;
};

#endif
