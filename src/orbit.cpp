#include <cstdlib>
#include <GL/freeglut.h>
#include "../include/Planet.h"
#include <stdio.h>
#include "../include/Planet_Info.h"
#include "../include/shared_constants.h"
#include "../include/Camera_Structs.h"
#include <iostream>
#include <math.h>

using namespace std;
void OpenGLInit( void );
void Animate( void );
void ResizeWindow( int w, int h );

// global variables
double last_click_x = -1;
double last_click_y = -1;
GLenum spinMode = GL_TRUE;
GLenum singleStep = GL_FALSE;
float HourOfDay = 0.0;
float DayOfYear = 0.0;
float AnimateIncrement = 24.0;  // animation time step (hours)

Planet * planets[NUM_PLANETS];

camera_position CameraPos;

camera_velocity CameraVel;

const double camera_translate_coeff = 1;
const double camera_rotate_coeff = 1;
// Animate() handles the animation and the redrawing of the graphics window contents.
void keyboard( unsigned char key, int x, int y );
void special_keyboard( int key, int x, int y);
void keyboardUp( unsigned char key, int x, int y );
void rotate_about_axis3(double &x, double &y, double &z, 
                        double x_hat, double y_hat, double z_hat, double rads);
void move_left(double amount);
void move_up(double amount);
void move_forward(double amount);
void rotate_pitch(double amount);
void rotate_roll(double amount);
void rotate_yaw(double amount);
void click( int button, int state, int x, int y );
int ScreenWidth  = 600;
int ScreenHeight = 600;

void init_camera()
{
    CameraPos.ey_x = 0;
    CameraPos.ey_y = 0;
    CameraPos.ey_z = 0;
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
    CameraVel.th_x = 0;
    CameraVel.th_y = 0;
    CameraVel.th_z = 0;
}

void Animate( void )
{
    // Clear the redering window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glLoadIdentity();
    gluLookAt(CameraPos.ey_x,CameraPos.ey_y,CameraPos.ey_z,
              CameraPos.at_x,CameraPos.at_y,CameraPos.at_z,
              CameraPos.up_x,CameraPos.up_y,CameraPos.up_z);
    glTranslatef(0.0, 0.0, -1000000.0);
    glRotatef(90.0, 0.0, 0.0, 1.0);
    glColor3f(1.0,1.0,0.0);
    glutWireSphere(6960, 50, 50);
    for(int i = 0; i < NUM_PLANETS; i++)
    {
        glPushMatrix();
        planets[i]->animate();
        glPopMatrix();

    }
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();        // Request a re-draw for animation purposes
}

/******************************************************************************
* Function: keyboardUp( unsigned char key, int x, int y )
* Authors: Ian Carlson, Christopher Smith
* Description: Handles the regular keys
* Arguments:
*   key: code of the key that was pressed
*   x: x coordinate the key was pressed at
*   y: y coordinate the key was pressed at
* ****************************************************************************/
void keyboardUp(unsigned char key, int x, int y)
{
    // correct for upside-down screen coordinates
    y = ScreenHeight - y;
    cerr << "keyrelease: " << key << " (" << int( key ) << ") at (" << x << "," << y << ")\n";
    switch(key)
    {
    }
}

//When the user clicks, set the position in the last_pos var so we can
//know where they're going if they drag
void click( int button, int state, int x, int y )
{
    y = ScreenHeight - y;
    last_click_x = x;
    last_click_y = y;
    cerr << "Click at: (" << x << "," << y << ")\n";
}

void drag(int x, int y)
{
    y = ScreenHeight - y;
    double x_diff = x - last_click_x;
    double y_diff = y - last_click_y;
    rotate_pitch(y_diff/100);
    rotate_yaw(x_diff/100);
    last_click_x = x;
    last_click_y = y;
    cerr << "Drag to: (" << x << "," << y << ")\n";
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
    cerr << "keypress: " << key << " (" << int( key ) << ") at (" << x << "," << y << ")\n";
    switch(key)
    {
        case 'a':
        case 'A':
            move_left(10000);
            break;
        case 'w':
        case 'W':
            move_up(10000);
            break;
        case 'd':
        case 'D':
            move_left(-10000);
            break;
        case 's':
        case 'S':
            move_up(-10000);
            break;
        case 'e':
        case 'E':
            rotate_roll(0.2);
            break;
        case 'q':
        case 'Q':
            rotate_roll(-0.2);
            break;
    }

}

void special_keyboard( int key, int x, int y)
{
    y = ScreenHeight -y;
    cerr << "Special Keypress: " << key << " (" << int( key ) << ") at (" << x << "," << y << ")\n";

    switch(key)
    {
    case GLUT_KEY_PAGE_UP:
        move_forward(10000);
        break;
    case GLUT_KEY_PAGE_DOWN:
        move_forward(-10000);
        break;
    }
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
    //rotate at and up about left by amount*camera_rotate_coeff
    //extract vector from at - ey
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    rotate_about_axis3(x, y, z,
                       CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                       CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                       amount*camera_rotate_coeff);
    //reconstruct the actual at from ey + the new vector
    CameraPos.at_x = CameraPos.ey_x + x;
    CameraPos.at_y = CameraPos.ey_y + y;
    CameraPos.at_z = CameraPos.ey_z + z;
}

void rotate_pitch(double amount)
{
    //rotate at and up about left by amount*camera_rotate_coeff
    //extract vector from at - ey
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    rotate_about_axis3(x, y, z,
                       CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                       CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                       amount*camera_rotate_coeff);
    //reconstruct the actual at from ey + the new vector
    CameraPos.at_x = CameraPos.ey_x + x;
    CameraPos.at_y = CameraPos.ey_y + y;
    CameraPos.at_z = CameraPos.ey_z + z;
}

void rotate_roll(double amount)
{
    //rotate up and left about at by amount*camera_rotate_coeff
    rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                       CameraPos.at_x-CameraPos.ey_x,
                       CameraPos.at_y-CameraPos.ey_y,
                       CameraPos.at_z-CameraPos.ey_z,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                       CameraPos.at_x-CameraPos.ey_x,
                       CameraPos.at_y-CameraPos.ey_y,
                       CameraPos.at_z-CameraPos.ey_z,
                       amount*camera_rotate_coeff);
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

// Initialize OpenGL's rendering modes
void OpenGLInit( void )
{
    glShadeModel( GL_FLAT );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glEnable( GL_DEPTH_TEST );
    glutKeyboardFunc( keyboard );
    glutKeyboardUpFunc( keyboardUp);
    glutSpecialFunc(special_keyboard);
    glutMouseFunc( click );
    glutMotionFunc( drag );
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
    gluPerspective( 30.0, aspectRatio, 0.0, 2000000000.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}

// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv )
{
    FILE * in = fopen("planet_info.inf","r");
    Planet_Info info;
    init_camera();
    for(int i = 0; i < NUM_PLANETS; i++)
    {
        fscanf(in,"%s %Lg %Lg %Lg %Lg %Lg %Lg %Lg %lf %lf %lf %s",
                info.name,&info.r,&info.o_r,&info.th,&info.o_v,&info.phi,&info.r_s,&info.t,&info.color[0],&info.color[1],&info.color[2],info.texture);
        
       // printf("%s,%Lg,%Lg,%Lg,%Lg,%Lg,%Lg,%Lg,%lf,%lf,%lf,%s\n\n\n",info.name,info.r,info.o_r,info.th,info.o_v,info.phi,info.r_s,info.t,info.color[0],info.color[1],info.color[2],info.texture);

        planets[i] = new Planet(info);
    }
    fclose(in);
        
    // Need to double buffer for animation
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Create and position the graphics window
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 600, 360 );
    glutCreateWindow( "Solar System" );

    // Initialize OpenGL.
    OpenGLInit();
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
    // Set up the callback function for resizing windows
    glutReshapeFunc( ResizeWindow );

    // Callback for graphics image redrawing
    glutDisplayFunc( Animate );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );
    for(int i = 0; i < NUM_PLANETS; i++)
        delete planets[i];
    return 0;
}
