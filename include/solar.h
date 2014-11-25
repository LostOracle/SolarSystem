/************************************************************************
* File: solar.h
* Author: Chris Smith, Ian Carlson
* Description: Drawing functions and main function that initiates all
* drawing objects.  
* Date: 20 Nov 2014
************************************************************************/
#ifndef _SOLAR_H_
#define _SOLAR_H_

#include <cstdlib>
#include <GL/freeglut.h>
#include "Planet.h"
#include <stdlib.h>
#include <stdio.h>
#include "Planet_Info.h"
#include "shared_constants.h"
#include "Camera_Structs.h"
#include "CameraFunctions.h"
#include "callbacks.h"
#include <iostream>
#include <math.h>
#include <vector>

void SubMenuHandler( int item );
void MainMenuHandler( int item );
void CreateMenus();
void get_planet(Planet * parent, FILE *& in, int planets_index);
void displaySubMenuHandler( int item );
void jumpToSubMenuHandler( int item );
void OpenGLInit( void );
void Animate( void );
void ResizeWindow( int w, int h );

#endif
