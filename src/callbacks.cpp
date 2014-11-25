/******************************************************************************
 * File: callbacks.cpp
 * Authors: Ian Carlson, Chris Smith
 * Date: 11/24/14
 * Description: Contains the definition for the callback functions for solar
 * ***************************************************************************/
#include "../include/callbacks.h"


//Global variables from orbit.cpp that we need access to
extern bool left_dragging;
extern int ScreenHeight;
extern int ScreenWidth;
extern double last_click_x;
extern double last_click_y;
extern Planet * target_lock;
extern Planet ** planets;
extern bool paused;
extern int num_planets;

/******************************************************************************
 * Function drag
 * Authors: Ian Carlson, Chris Smith
 * Parameters: x,y location the mouse was dragged to
 * Returns: none
 * Description: Handle mouse drags. If the user is dragging the mouse with
 *  the left button held down, we want to pitch and/or yaw the view accordingly
 *  **************************************************************************/
void drag(int x, int y)
{
    //if they aren't holding down the left mouse button, return
    if(!left_dragging)
        return;

    //flip from screen to gl coordinates
    y = ScreenHeight - y;

    //calculate the difference between this and the last point
    double x_diff = x - last_click_x;
    double y_diff = y - last_click_y;
    //rotate by the difference the /100 was what we found gave an
    //intuitive feel on the op lab machines
    rotate_pitch(-y_diff/100.0);
    rotate_yaw(-x_diff/100.0);

    //update the last clicked spot
    last_click_x = x;
    last_click_y = y;
}

/******************************************************************************
 * Function click
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  button - which button was clicked
 *  state - whether the button was pressed or released
 *  x,y - location the mouse was clicked
 * Returns: none
 * Description: Handle mouse clicks including the scroll wheel and left mouse
 *  clicks.
 *  **************************************************************************/
void click( int button, int state, int x, int y )
{
    //convert screen to gl coordinates
    y = ScreenHeight - y;

    //If the user clicks the left mouse button, they may be about to drag,
    //prepare for it
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            last_click_x = x;
            last_click_y = y;
            left_dragging = true;
        }
        else if(state == GLUT_UP)
        {
            left_dragging = false;
        }
    }
    else if(button == 3)
    {
        if(state == GLUT_DOWN)
        {
            //zoom in
            //zooming doesn't make sense when there's no locked target
            if(target_lock == NULL)
                return;
            zoom(ZOOM_COEFF);
        }
    }
    else if(button == 4)
    {
        if(state == GLUT_DOWN)
        {
            //zoom out
            //zooming doesn't make sense when there's no locked target
            if(target_lock == NULL)
                return;
            zoom(-ZOOM_COEFF);
        }
    }
}

/******************************************************************************
* Function: keyboard( unsigned chart key, int x, int y )
* Authors: Ian Carlson, Christopher Smith
* Parameters:
*   key: code of the key that was pressed
*   x: x coordinate the key was pressed at
*   y: y coordinate the key was pressed at
* Returns: none
* Description: Handles keyboard events
* ****************************************************************************/
void keyboard( unsigned char key, int x, int y )
{
    //flip to gl coordinates
    y = ScreenHeight - y;
    switch(key)
    {
    case ' ':
        move_up(TRANSLATE_COEFF);
        break;
    case 'c':
    case 'C':
    case 'x':
    case 'X':
        move_up(-TRANSLATE_COEFF);
        break;
    case 'a':
    case 'A':
        move_left(TRANSLATE_COEFF);
        break;
    case 'w':
    case 'W':
        move_forward(TRANSLATE_COEFF);
        break;
    case 'd':
    case 'D':
        move_left(-TRANSLATE_COEFF);
        break;
    case 's':
    case 'S':
        move_forward(-TRANSLATE_COEFF);
        break;
    case 'e':
    case 'E':
        rotate_roll(ROTATE_COEFF);
        break;
    case 'q':
    case 'Q':
        rotate_roll(-ROTATE_COEFF);
        break;
    //increase simulation speed
    case '+':
    case '=':
        Planet::increment_time_step(TIME_STEP_COEFF);
        break;
    case '-':
        Planet::increment_time_step(-TIME_STEP_COEFF);
        break;
    case 'p':
    case 'P':
        paused = !paused;
        break;
    case 't':
    case 'T':
        paused = true;
        //take one timestep forward
        for(int i = 0; i < num_planets; i++)
            planets[i]->animate();
        break;

    //increase the planet resolution
    case '*':
        Planet::increment_wire_res();
        break;
    //decrease the planet resolution
    case '/':
        Planet::decrement_wire_res();
        break;
    }
}

/******************************************************************************
* Function: special_keyboard( unsigned chart key, int x, int y )
* Authors: Ian Carlson, Christopher Smith
* Paremeters:
*   key: code of the key that was pressed
*   x: x coordinate the key was pressed at
*   y: y coordinate the key was pressed at
* Returns: none
* Description: Handles fancy keyboard events
* ****************************************************************************/
void special_keyboard( int key, int x, int y)
{
    //flip to gl coordinates
    y = ScreenHeight -y;

    switch(key)
    {
        //alternate method for zooming in
        case GLUT_KEY_PAGE_UP:
            zoom(ZOOM_COEFF);
            break;
        //alternate method for zooming out
        case GLUT_KEY_PAGE_DOWN:
            zoom(-ZOOM_COEFF);
            break;
    }
}



/******************************************************************************
* Function: ResizeWindow
* Authors: Ian Carlson, Christopher Smith
*   Note: this is pretty much exactly Dr. Weiss's code from renderDemo.cpp
* Parameters:
*   w,h new screen dimensions in pixels
* Returns: none
* Description: Handles window resizes
* ****************************************************************************/
void ResizeWindow( int w, int h )
{
    float aspectRatio;
    ScreenWidth = w;
    ScreenHeight = h;
    h = ( h == 0 ) ? 1 : h;
    w = ( w == 0 ) ? 1 : w;
    glViewport( 0, 0, w, h );   // View port uses whole window
    aspectRatio = ( float ) w / ( float ) h;

    // Set up the projection view matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 30.0, aspectRatio, 1000.0, 200000000.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}
