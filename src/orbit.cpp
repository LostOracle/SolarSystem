#include <cstdlib>
#include <GL/freeglut.h>
#include "../include/Planet.h"
#include <stdio.h>
#include "../include/Planet_Info.h"
#include "../include/shared_constants.h"
#include <iostream>
#include <math.h>

using namespace std;
void OpenGLInit( void );
void Animate( void );
void ResizeWindow( int w, int h );

// global variables
GLenum spinMode = GL_TRUE;
GLenum singleStep = GL_FALSE;
float HourOfDay = 0.0;
float DayOfYear = 0.0;
float AnimateIncrement = 24.0;  // animation time step (hours)

Planet * planets[NUM_PLANETS];

struct camera_position
{
    double ey_x,ey_y,ey_z;
    double at_x,at_y,at_z;
    double up_x,up_y,up_z;
    double lf_x,lf_y,lf_z;
}CameraPos;

struct camera_velocity
{
    double x,y,z;
    double th_x,th_y,th_z;
}CameraVel;

const double camera_translate_coeff = 1;
const double camera_rotate_coeff = 1;
// Animate() handles the animation and the redrawing of the graphics window contents.
void keyboard( unsigned char key, int x, int y );
void keyboardUp( unsigned char key, int x, int y );
void rotate_about_axis3(double &x, double &y, double &z, 
                        double x_hat, double y_hat, double z_hat, double rads);
void rotate_camera_pitch(double amount);
void rotate_camera_roll(double amount);
void rotate_camera_yaw(double amount);
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
    CameraPos.lf_x = -1;
    CameraPos.lf_y = 0;
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
    gluLookAt(0,0,0,0,0,-1,1,1,0);
    glTranslatef(0.0, 0.0, -1000000.0);
    glRotatef(15.0, 1.0, 0.0, 0.0);
    glColor3f(1.0,1.0,0.0);
    glutWireSphere(6960, 50, 50);
    glTranslatef(100000,0,0);
    glutWireSphere( 2439,50,50);
    for(int i = 0; i < NUM_PLANETS; i++)
    {
        planets[i]->animate();
    }
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
        case 'a':
        case 'A':
    //        camera_vel.x += 100;
            break;
        case 'd':
        case 'D':
    //        camera_vel.x -= 100;
            break;
        case 'w':
        case 'W':   
    //        camera_vel.
            break;
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
    cerr << "keypress: " << key << " (" << int( key ) << ") at (" << x << "," << y << ")\n";
    switch(key)
    {
    //    case 'a':
    }

}

//when the user wants to rotate left/right, we rotate
//about the up vector, updating the left and at
//vectors
void rotate_camera_yaw(double amount)
{
    //rotate at and left about up by amount*camera_rotate_coeff
    rotate_about_axis3(CameraPos.at_x, CameraPos.at_y, CameraPos.at_z,
                       CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                       CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                       amount*camera_rotate_coeff);
}

void rotate_camera_pitch(double amount)
{
    //rotate at and up about left by amount*camera_rotate_coeff
    rotate_about_axis3(CameraPos.at_x, CameraPos.at_y, CameraPos.at_z,
                       CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                       CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                       amount*camera_rotate_coeff);
}

void rotate_camera_roll(double amount)
{
    //rotate up and left about at by amount*camera_rotate_coeff
    rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                       CameraPos.at_x,CameraPos.at_y,CameraPos.at_z,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                       CameraPos.at_x,CameraPos.at_y,CameraPos.at_z,
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
    gluPerspective( 30.0, aspectRatio, 0.0, 2000000.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}

// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv )
{
    FILE * in = fopen("planet_info.inf","r");
    Planet_Info info;
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
