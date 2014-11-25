#include "../include/callbacks.h"

extern bool left_dragging;
extern int ScreenHeight;
extern int ScreenWidth;
extern double last_click_x;
extern double last_click_y;
extern Planet * target_lock;
extern Planet ** planets; 
extern bool paused;
extern int num_planets;

void drag(int x, int y)
{
    if(!left_dragging)
        return;
    y = ScreenHeight - y;
    double x_diff = x - last_click_x;
    double y_diff = y - last_click_y;
    rotate_pitch(-y_diff/100);
    rotate_yaw(-x_diff/100);
    last_click_x = x;
    last_click_y = y;
}
//When the user clicks, set the position in the last_pos var so we can
//know where they're going if they drag
void click( int button, int state, int x, int y )
{
    //only for clicks on the left mouse button
    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            y = ScreenHeight - y;
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
            if(target_lock == NULL)
                return;
            zoom(-ZOOM_COEFF);
        }
    }
}
/******************************************************************************
* Function: keyboard( unsigned chart key, int x, int y )
* Authors: Ian Carlson, Christopher Smith
* Description: Handles the normal key presses for pong
* Arguments:
*   key: code of the key that was pressed
*   x: x coordinate the key was pressed at
*   y: y coordinate the key was pressed at
* ****************************************************************************/

// callback function that tells OpenGL how to handle keystrokes
void keyboard( unsigned char key, int x, int y )
{
    // correct for upside-down screen coordinates
    y = ScreenHeight - y;
    switch(key)
    {
        case 'a':
        case 'A':
            move_left(TRANSLATE_COEFF);
            break;
        case 'w':
        case 'W':
            move_up(TRANSLATE_COEFF);
            break;
        case 'd':
        case 'D':
            move_left(-TRANSLATE_COEFF);
            break;
        case 's':
        case 'S':
            move_up(-TRANSLATE_COEFF);
            break;
        case 'e':
        case 'E':
            rotate_roll(ROTATE_COEFF);
            break;
        case 'q':
        case 'Q':
            rotate_roll(-ROTATE_COEFF);
            break;
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
        case '*':
            Planet::increment_wire_res();
            break;
        case '/':
            Planet::decrement_wire_res();
            break;
    }
}

void special_keyboard( int key, int x, int y)
{
    y = ScreenHeight -y;

    switch(key)
    {
    case GLUT_KEY_PAGE_UP:
        zoom(ZOOM_COEFF);
        break;
    case GLUT_KEY_PAGE_DOWN:
        zoom(-ZOOM_COEFF);
        break;
    }
}



// ResizeWindow is called when the window is resized
void ResizeWindow( int w, int h )
{
    float aspectRatio;
    ScreenWidth = w;
    ScreenHeight = h;
    h = ( h == 0 ) ? 1 : h;
    w = ( w == 0 ) ? 1 : w;
    glViewport( 0, 0, w, h );   // View port uses whole window
    aspectRatio = ( float ) w / ( float ) h;

    // Set up the projection view matrix (not very well!)
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 30.0, aspectRatio, 1000.0, 200000000.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}
