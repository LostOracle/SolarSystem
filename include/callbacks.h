/************************************************************************
* File: callbacks.h
* Author: Chris Smith, Ian Carlson
* Description: OpenGL registered event handler callback functions
* Date: 20 Nov 2014
************************************************************************/
#ifndef _CALLBACKS_H_
#define _CALLBACKS_H_

#include "CameraFunctions.h"
#include "Planet.h"
#include "shared_constants.h"
void keyboard( unsigned char key, int x, int y );
void special_keyboard( int key, int x, int y);
void keyboardUp( unsigned char key, int x, int y );
void click( int button, int state, int x, int y );
void drag(int x, int y);
#endif
