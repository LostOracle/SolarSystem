#ifndef _SOLAR_H_
#define _SOLAR_H_

#include <cstdlib>
#include <GL/freeglut.h>
#include "../include/Planet.h"
#include <stdio.h>
#include "../include/Planet_Info.h"
#include "../include/shared_constants.h"
#include "../include/Camera_Structs.h"
#include "../include/CameraFunctions.h"
#include <iostream>
#include <math.h>
#include <vector>
using namespace std;

double camera_translate_coeff = 100;
double camera_rotate_coeff = .2;
double time_step_coeff = 10;
double zoom_amount = 0.05;
void keyboard( unsigned char key, int x, int y );
void special_keyboard( int key, int x, int y);
void keyboardUp( unsigned char key, int x, int y );
void SubMenuHandler( int item );
void MainMenuHandler( int item );
void CreateMenus();
void get_planet(Planet * parent, FILE *& in, int planets_index);
void click( int button, int state, int x, int y );
void displaySubMenuHandler( int item );
void jumpToSubMenuHandler( int item );
void OpenGLInit( void );
void Animate( void );
void ResizeWindow( int w, int h );

#endif
