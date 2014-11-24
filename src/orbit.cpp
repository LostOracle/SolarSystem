#include <cstdlib>
#include <GL/freeglut.h>
#include "../include/Planet.h"
#include <stdio.h>
#include "../include/Planet_Info.h"
#include "../include/shared_constants.h"
#include "../include/Camera_Structs.h"
#include <iostream>
#include <math.h>
#include <vector>

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
int num_planets;
bool left_dragging = false;

Planet ** planets;
vector<Planet *> all_celestial_bodies;
Planet * target_lock;
//Some handy mouse handling constants provided by Dr. Weiss
const int MouseButtonPress	= 0;
const int MouseButtonRelease= 1;
const int MouseLeftButton	= 0;
const int MouseMiddleButton	= 1; //unused
const int MouseRightButton	= 1; //unused

camera_position CameraPos;
camera_velocity CameraVel;

double camera_translate_coeff = 100;
double camera_rotate_coeff = .2;
double time_step_coeff = 10;
// Animate() handles the animation and the redrawing of the graphics window contents.
void keyboard( unsigned char key, int x, int y );
void special_keyboard( int key, int x, int y);
void keyboardUp( unsigned char key, int x, int y );
void rotate_about_axis3(double &x, double &y, double &z, 
                        double x_hat, double y_hat, double z_hat, double rads);
void SubMenuHandler( int item );
void MainMenuHandler( int item );
void testSubMenuHandler(int item);
void zoom(double amount);
void CreateMenus();
void move_left(double amount);
void move_up(double amount);
void move_forward(double amount);
void rotate_pitch(double amount);
void rotate_roll(double amount);
void rotate_yaw(double amount);
void get_planet(Planet * parent, FILE *& in, int planets_index);
void click( int button, int state, int x, int y );
void displaySubMenuHandler( int item );
void jumpToSubMenuHandler( int item );
int ScreenWidth  = 600;
int ScreenHeight = 600;

void init_camera()
{
    CameraPos.ey_x = 0;
    CameraPos.ey_y = 0;
    CameraPos.ey_z = 1000000;
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
    CameraVel.roll = 0;
    CameraVel.pitch = 0;
    CameraVel.yaw = 0;
}

void Animate( void )
{
    // Clear the redering window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //update the positions of the planets
    for(int i = 0; i < NUM_PLANETS; i++)
        planets[i]->animate();

    //track the target planet, if any
    if(target_lock != NULL)
    {
        long double target_x, target_y;
        long double vect_x, vect_y,vect_z;
        vect_x = CameraPos.at_x - CameraPos.ey_x;
        vect_y = CameraPos.at_y - CameraPos.ey_y;
        vect_z = CameraPos.at_z - CameraPos.ey_z;

        target_lock->get_location(target_x,target_y);
        CameraPos.at_x = target_x;
        CameraPos.at_y = target_y;
        CameraPos.at_z = 0;
        CameraPos.ey_x = CameraPos.at_x - vect_x;
        CameraPos.ey_y = CameraPos.at_y - vect_y;
        CameraPos.ey_z = CameraPos.at_z - vect_z;
    }

    //draw everything, the planets take care of drawing their moons
    glLoadIdentity();
    gluLookAt(CameraPos.ey_x,CameraPos.ey_y,CameraPos.ey_z,
              CameraPos.at_x,CameraPos.at_y,CameraPos.at_z,
              CameraPos.up_x,CameraPos.up_y,CameraPos.up_z);
    for(int i = 0; i < NUM_PLANETS; i++)
    {
        glPushMatrix();
        planets[i]->draw();
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
            zoom(0.05);
            cout << "Zoom in" << endl;
        }
    }
    else if(button == 4)
    {
        if(state == GLUT_DOWN)
        {
            zoom(-0.05);
            cout << "zoom out" << endl;
        }
    }
    else
        cout << "button: " << button << "state: " << state << endl;
}

void zoom(double amount)
{
    //zooming doesn't make sense when there's no locked target
    if(target_lock == NULL)
        return;
    //move 5% closer to the target object, if any
    long double x,y,z;
    x = CameraPos.ey_x - CameraPos.at_x;
    y = CameraPos.ey_y - CameraPos.at_y;
    z = CameraPos.ey_z - CameraPos.at_z;
    x = (1-amount)*x;
    y = (1-amount)*y;
    z = (1-amount)*z;
    CameraPos.ey_x = CameraPos.at_x + x;
    CameraPos.ey_y = CameraPos.at_y + y;
    CameraPos.ey_z = CameraPos.at_z + z;
}

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
            move_left(camera_translate_coeff);
            break;
        case 'w':
        case 'W':
            move_up(camera_translate_coeff);
            break;
        case 'd':
        case 'D':
            move_left(-camera_translate_coeff);
            break;
        case 's':
        case 'S':
            move_up(-camera_translate_coeff);
            break;
        case 'e':
        case 'E':
            rotate_roll(camera_rotate_coeff);
            break;
        case 'q':
        case 'Q':
            rotate_roll(-camera_rotate_coeff);
            break;
        case '+':
        case '=':
            Planet::increment_time_step(time_step_coeff);
            break;
        case '-':
            Planet::increment_time_step(-time_step_coeff);
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
        move_forward(camera_translate_coeff);
        break;
    case GLUT_KEY_PAGE_DOWN:
        move_forward(-camera_translate_coeff);
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
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    //maintain distance, but move eye left or right
    if(target_lock != NULL)
    {
        //invert the ey to at vector and rotate it around
        //the vector 0,0,1
        x=-x;
        y=-y;
        z=-z;
        rotate_about_axis3(x,y,z,
                CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           amount*camera_rotate_coeff);

        //also update the left vector
        rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                amount*camera_rotate_coeff);
        CameraPos.ey_x = CameraPos.at_x + x;
        CameraPos.ey_y = CameraPos.at_y + y;
        CameraPos.ey_z = CameraPos.at_z + z;
    }

    //rotate at and up about left by amount*camera_rotate_coeff
    //extract vector from at - ey
    else
    {
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
}

void rotate_pitch(double amount)
{
    double x = CameraPos.at_x - CameraPos.ey_x;
    double y = CameraPos.at_y - CameraPos.ey_y;
    double z = CameraPos.at_z - CameraPos.ey_z;
    //invert x,y,z and rotate that vector about lf then move ey
    if(target_lock != NULL)
    {
        rotate_about_axis3(CameraPos.up_x,CameraPos.up_y,CameraPos.up_z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount*camera_rotate_coeff); 
        x = -x;
        y = -y;
        z = -z;
        rotate_about_axis3(x,y,z,
                           CameraPos.lf_x,CameraPos.lf_y,CameraPos.lf_z,
                           amount*camera_rotate_coeff);
        CameraPos.ey_x = CameraPos.at_x + x;
        CameraPos.ey_y = CameraPos.at_y + y;
        CameraPos.ey_z = CameraPos.at_z + z;
                    
    }
    //rotate at and up about left by amount*camera_rotate_coeff
    //extract vector from at - ey
    else
    {
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
}

void rotate_roll(double amount)
{
    double mag = 0;
    mag += (CameraPos.at_x-CameraPos.ey_x)*(CameraPos.at_x-CameraPos.ey_x);
    mag += (CameraPos.at_y-CameraPos.ey_y)*(CameraPos.at_y-CameraPos.ey_y);
    mag += (CameraPos.at_z-CameraPos.ey_z)*(CameraPos.at_z-CameraPos.ey_z);
    mag = sqrt(mag);

    //rotate up and left about at by amount*camera_rotate_coeff
    rotate_about_axis3(CameraPos.lf_x, CameraPos.lf_y, CameraPos.lf_z,
                       (CameraPos.at_x-CameraPos.ey_x)/mag,
                       (CameraPos.at_y-CameraPos.ey_y)/mag,
                       (CameraPos.at_z-CameraPos.ey_z)/mag,
                       amount*camera_rotate_coeff);
    rotate_about_axis3(CameraPos.up_x, CameraPos.up_y, CameraPos.up_z,
                       (CameraPos.at_x-CameraPos.ey_x)/mag,
                       (CameraPos.at_y-CameraPos.ey_y)/mag,
                       (CameraPos.at_z-CameraPos.ey_z)/mag,
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
    CreateMenus();
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
    gluPerspective( 30.0, aspectRatio, 1.0, 2000000000.0 );

    // Select the Modelview matrix
    glMatrixMode( GL_MODELVIEW );
}

// Main routine
// Set up OpenGL, hook up callbacks, and start the main loop
int main( int argc, char** argv )
{
    FILE * in = fopen("planet_info.inf","r");
    if(!in)
    {
        cout << "Failed to find planet description file \"planet_info.inf\"" << endl;
        return 0;
    }
    //Get the number of entries in planet_info.inf
    fscanf(in,"%d",&num_planets);

    //create an array of points to that many objects
    planets = new Planet*[num_planets];

    for(int i = 0; i < NUM_PLANETS; i++)
    {
        get_planet(NULL,in,i);
    }
    fclose(in);
    //start off locked on the sun, which is the first planet in the file
    target_lock = planets[0];
        
    // Need to double buffer for animation
    init_camera();
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
    delete []planets;
    return 0;
}

void get_planet(Planet * parent, FILE *& in, int planets_index)
{

    Planet_Info info;
    //read the next line of the file
    fscanf(in,"%s %Lg %Lg %Lg %Lg %Lg %Lg %Lg %lf %lf %lf %s %d %d",
            info.name,&info.r,&info.o_r,&info.th,&info.o_v,&info.phi,&info.r_s,&info.t,&info.color[0],&info.color[1],&info.color[2],info.texture, &info.moons, &info.rings);
    if(info.rings)
        fscanf(in,"%Lg %Lg %s",&info.inner_r, &info.outer_r, info.ring_texture);

    if(NULL == parent)
    {
        cout << "Adding planet: " << info.name << endl;
        planets[planets_index] = new Planet(info,NULL);
        all_celestial_bodies.push_back(planets[planets_index]);
        for(int i = 0; i < info.moons; i++)
            get_planet(planets[planets_index],in,0);//the index doesn't matter if the parent * is non null
    }
    else
    {
        char name[1000];
        parent->get_planet_name(name);
        cout << "\tAdding moon to planet: " << name << "moon name: " << info.name << endl;
        Planet * new_parent = parent->add_moon(info);
        for(int i = 0; i < info.moons; i++)
            get_planet(new_parent,in,0);//the index doesn't matter if the parent * is non null
        all_celestial_bodies.push_back(new_parent);
    }
}    

// CreateMenus() creates the right mouse button menu
void CreateMenus()
{
    // create submenu
    int value = 1;

    int displaySubMenu = glutCreateMenu( displaySubMenuHandler );
    glutAddMenuEntry( "Wireframe", value++ );
    glutAddMenuEntry( "Flat Shading", value++ );
    glutAddMenuEntry( "Smooth Shading", value++ );
    glutAddMenuEntry( "Textures", value++ );

    value = 1;
    int jumpToSubMenu = glutCreateMenu(jumpToSubMenuHandler);
    for(unsigned int i = 0; i < all_celestial_bodies.size(); i++)
    {
        char name[1024];
        all_celestial_bodies[i]->get_planet_name(name);
        glutAddMenuEntry(name,value++);
    }

    // create main menu
    value = 1;
    int mainmenu = glutCreateMenu( MainMenuHandler );
    cout << "mainmenu id = " << mainmenu << endl;
    glutAddSubMenu("Display Method",displaySubMenu);
    glutAddSubMenu("Jump To",jumpToSubMenu);    
    glutAddMenuEntry("Unlock Camera",value++);
    // right button click activates menu
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

void testSubMenuHandler(int item)
{

}

// MainMenuHandler() controls the main menu
// only action is to print selected menu entry
void MainMenuHandler( int item )
{
    switch ( item )
    {
        case 1:
            target_lock = NULL;
            cout << "you selected main menu item " << item << endl;
            break;
        default:    // should not occur
            cout << "invalid main menu item " << item << endl;
            break;
    }
}

void displaySubMenuHandler( int item )
{
    switch ( item )
    {
        case 1:
        case 2:
        case 3:
        case 4:
            cout << "you selected submenu item " << item << endl;
            break;
        break;
        default:    // should not occur
            cout << "invalid submenu item " << item << endl;
            break;
    }   
}

// SubMenuHandler() controls the submenu
// only action is to print selected submenu entry
void jumpToSubMenuHandler( int item )
{
    char name[1024];
    long double x,y;
    target_lock = all_celestial_bodies[item-1];
    all_celestial_bodies[item-1]->get_location(x,y);

    all_celestial_bodies[item-1]->get_planet_name(name);
    cout << "Jump to: " << name << endl;
}
