/************************************************************************
* File: CameraFunctions.h
* Author: Chris Smith, Ian Carlson
* Description: Functions for handling camera movement
* Date: 20 Nov 2014
************************************************************************/
#ifndef _CAMERA_FUNCTIONS_H_
#define _CAMERA_FUNCTIONS_H_

#include "Camera_Structs.h"
#include "Planet.h"
#include <math.h>
void zoom(double amount);
void move_left(double amount);
void move_up(double amount);
void move_forward(double amount);
void rotate_pitch(double amount);
void rotate_roll(double amount);
void rotate_yaw(double amount);
void init_camera();
void rotate_about_axis3(double &x, double &y, double &z,
                        double x_hat, double y_hat, double z_hat, double rads);
#endif
