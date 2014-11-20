#include <cstdlib>
#include <GL/freeglut.h>
#include "../include/Planet.h"
#include <stdio.h>
#include "../include/Planet_Info.h"
#include "../include/shared_constants.h"
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
// Animate() handles the animation and the redrawing of the graphics window contents.
void Animate( void )
{
    // Clear the redering window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -15000.0);
    glRotatef(15.0, 1.0, 0.0, 0.0);
    glColor3f(1.0,1.0,0.0);
    glutWireSphere(696000.0/1000.0, 50, 50);
    for(int i = 0; i < NUM_PLANETS; i++)
    {
        planets[i]->animate();
    }
    glFlush();
    glutSwapBuffers();
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
    gluPerspective( 30.0, aspectRatio, 0.0, 8000.0 );

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
        fscanf(in,"%s,%Lg,%Lg,%Lg,%Lg,%Lg,%Lg,%Lg,%lg,%lg,%lg,%s",
                info.name,&info.r,&info.o_r,&info.th,&info.o_v,&info.phi,&info.r_s,&info.t,&info.color[0],&info.color[1],&info.color[2],info.texture);
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
