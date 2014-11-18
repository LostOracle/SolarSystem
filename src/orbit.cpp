/*
* orbit.cpp
*
* Program to demonstrate how to use a local
* coordinate method to position parts of a
* model in relation to other model parts.
*
* Draws a simple solar system, with a sun, planet and moon.
* Based on sample code from the OpenGL programming guide
* by Woo, Neider, Davis.  Addison-Wesley.
*
* Author: Samuel R. Buss
*
* Software accompanying the book
* 3D Computer Graphics: A Mathematical Introduction with OpenGL,
* by S. Buss, Cambridge University Press, 2003.
*
* Software is "as-is" and carries no warranty.  It may be used without
* restriction, but if you modify it, please change the filenames to
* prevent confusion between different versions.
*
* Bug reports: Sam Buss, sbuss@ucsd.edu.
* Web page: http://math.ucsd.edu/~sbuss/MathCG
*
* USAGE:
*    Press "r" key to toggle (off and on) running the animation
*    Press "s" key to single-step animation
*    The up and down array keys control the time step used in the animation rate.
*    Each key press multiplies or divides the times by a factor of two.
*    Press ESCAPE to exit.
*/

// JMW: Minor modifications for CSC433/533 Computer Graphics, Fall 2014.

#include <cstdlib>
#include <GL/freeglut.h>

// function prototypes
void OpenGLInit( void );
void Animate( void );
void Key_r( void );
void Key_s( void );
void Key_up( void );
void Key_down( void );
void ResizeWindow( int w, int h );
void KeyPressFunc( unsigned char Key, int x, int y );
void SpecialKeyFunc( int Key, int x, int y );

// global variables
GLenum spinMode = GL_TRUE;
GLenum singleStep = GL_FALSE;
float HourOfDay = 0.0;
float DayOfYear = 0.0;
float AnimateIncrement = 24.0;  // animation time step (hours)

// glutKeyboardFunc is called to set this function to handle normal key presses
void KeyPressFunc( unsigned char Key, int x, int y )
{
    switch ( Key )
    {
        case 'R':
        case 'r':
            Key_r();
            break;
        case 's':
        case 'S':
            Key_s();
            break;
        case 27:    // Escape key
            exit( 1 );
    }
}

// glutSpecialFunc is called to set this function to handle all special key presses
// (see glut.h for names of special keys)
void SpecialKeyFunc( int Key, int x, int y )
{
    switch ( Key )
    {
        case GLUT_KEY_UP:
            Key_up();
            break;
        case GLUT_KEY_DOWN:
            Key_down();
            break;
    }
}

// restart animation
void Key_r( void )
{
    if ( singleStep )	// restart animation
    {
        singleStep = GL_FALSE;
        spinMode = GL_TRUE;     
    }
    else				// toggle animation on and off
    {
        spinMode = !spinMode;   
    }
}

// single step animation
void Key_s( void )
{
    singleStep = GL_TRUE;
    spinMode = GL_TRUE;
}

// animation speed
void Key_up( void )
{
    AnimateIncrement *= 2.0;            // double the animation time step
}

// animation speed
void Key_down( void )
{
    AnimateIncrement /= 2.0;            // halve the animation time step
}

// Animate() handles the animation and the redrawing of the graphics window contents.
void Animate( void )
{
    // Clear the redering window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( spinMode )
    {
        // Update the animation state
        HourOfDay += AnimateIncrement;
        DayOfYear += AnimateIncrement / 24.0;

        HourOfDay = HourOfDay - ( ( int ) ( HourOfDay / 24 ) ) * 24;
        DayOfYear = DayOfYear - ( ( int ) ( DayOfYear / 365 ) ) * 365;
    }

    // Clear the current matrix (Modelview)
    glLoadIdentity();

    // Back off eight units to be able to view from the origin.
    glTranslatef ( 0.0, 0.0, -8.0 );

    // Rotate the plane of the elliptic
    // (rotate the model's plane about the x axis by fifteen degrees)
    glRotatef( 15.0, 1.0, 0.0, 0.0 );

    // Draw the sun -- as a yellow, wireframe sphere
    glColor3f( 1.0, 1.0, 0.0 );
    glutWireSphere( 1.0, 15, 15 );

    // Draw the Earth
    // First position it around the sun. Use DayOfYear to determine its position.
    glRotatef( 360.0 * DayOfYear / 365.0, 0.0, 1.0, 0.0 );
    glTranslatef( 4.0, 0.0, 0.0 );
    glPushMatrix();                     // Save matrix state
    // Second, rotate the earth on its axis. Use HourOfDay to determine its rotation.
    glRotatef( 360.0 * HourOfDay / 24.0, 0.0, 1.0, 0.0 );
    // Third, draw the earth as a wireframe sphere.
    glColor3f( 0.2, 0.2, 1.0 );
    glutWireSphere( 0.4, 10, 10 );
    glPopMatrix();                      // Restore matrix state

    // Draw the moon. Use DayOfYear to control its rotation around the earth
    glRotatef( 360.0 * 12.0 * DayOfYear / 365.0, 0.0, 1.0, 0.0 );
    glTranslatef( 0.7, 0.0, 0.0 );
    glColor3f( 0.3, 0.7, 0.3 );
    glutWireSphere( 0.1, 5, 5 );

    // Flush the pipeline, and swap the buffers
    glFlush();
    glutSwapBuffers();

    if ( singleStep ) spinMode = GL_FALSE;

    glutPostRedisplay();        // Request a re-draw for animation purposes
}

// Initialize OpenGL's rendering modes
void OpenGLInit( void )
{
    glShadeModel( GL_FLAT );
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glEnable( GL_DEPTH_TEST );
}

// ResizeWindow is called when the window is resized
void ResizeWindow( int w, int h )
{
    float aspectRatio;
    h = ( h == 0 ) ? 1 : h;
    w = ( w == 0 ) ? 1 : w;
    glViewport( 0, 0, w, h );   // View port uses whole window
    aspectRatio = ( float ) w / ( float ) h;

    // Set up the projection view matrix (not very well!)
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective( 60.0, aspectRatio, 1.0, 30.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}

// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv )
{
    // Need to double buffer for animation
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    // Create and position the graphics window
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 600, 360 );
    glutCreateWindow( "Solar System Demo" );

    // Initialize OpenGL.
    OpenGLInit();

    // Set up callback functions for key presses
    glutKeyboardFunc( KeyPressFunc );
    glutSpecialFunc( SpecialKeyFunc );

    // Set up the callback function for resizing windows
    glutReshapeFunc( ResizeWindow );

    // Callback for graphics image redrawing
    glutDisplayFunc( Animate );

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    return 0;
}
