/******************************************************************************
 * File: solar.cpp
 * Authors: Ian Carlson, Chris Smith
 * Date: 11/25/2014
 * Description: The entry point for the program solar. This file also contains
 *  functions
 *  */

#include "../include/solar.h"
using namespace std;

//Global Variables-------------------------------------------------------------
//keep track of the last place the user clicked for dragging purposes
double last_click_x;
double last_click_y;

//the number of planets (not moons) read in from the planet_info.inf file
int num_planets;

//if the user is currently dragging the mouse with the left button held down
bool left_dragging = false;

//paused or running state of the simulation
bool paused = false;

//an array of pointers to Planet objects
Planet ** planets;

//a vector - we got lazy - of all the objects including moons, which are also planets but harder to get to
vector<Planet *> all_celestial_bodies;

//the planet the camera is locked to
Planet * target_lock;

//camera state information
camera_position CameraPos;
camera_velocity CameraVel;

//screen dimensions
int ScreenWidth  = 600;
int ScreenHeight = 600;
//-----------------------------------------------------------------------------

void Animate( void )
{
    // Clear the redering window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //update the positions of the planets
    if(!paused)
        for(int i = 0; i < num_planets; i++)
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
    GLfloat light_position[] = { 0, 0, 0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, light_position );
    for(int i = 0; i < num_planets; i++)
    {
        glPushMatrix();
        planets[i]->draw();
        glPopMatrix();
    }
    glFlush();
    glutSwapBuffers();
    glutPostRedisplay();        // Request a re-draw for animation purposes
}

// Initialize OpenGL's rendering modes
void OpenGLInit( void )
{
    // Create and position the graphics window
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 600, 360 );
    glutCreateWindow( "Solar System" );

    // specify light source properties
    GLfloat light_ambient[] = { 0, 0, 0, 1.0 };       // ambient light
    GLfloat light_diffuse[] = { 1, 1, 1, 1.0 };       // diffuse light
    GLfloat light_specular[] = { 1, 1, 1, 1.0 };      // highlights
   
    glEnable(GL_LIGHT0);
    glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );
    
    glEnable( GL_DEPTH_TEST );  // enable depth buffer for hidden-surface elimination
    glEnable( GL_NORMALIZE );   // automatic normalization of normals
    glEnable( GL_CULL_FACE );   // eliminate backfacing polygons
    glEnable( GL_LIGHTING ) ;
    glEnable(GL_COLOR_MATERIAL); 
    
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );
    glutKeyboardFunc( keyboard );
    glutSpecialFunc(special_keyboard);
    glutMouseFunc( click );
    glutMotionFunc( drag );
    glutReshapeFunc( ResizeWindow );
    glutDisplayFunc( Animate );
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
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

    for(int i = 0; i < num_planets; i++)
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

    // Initialize OpenGL.
    OpenGLInit();
    CreateMenus();

    // Start the main loop.  glutMainLoop never returns.
    glutMainLoop( );

    for(int i = 0; i < num_planets; i++)
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
        planets[planets_index] = new Planet(info,NULL);
        all_celestial_bodies.push_back(planets[planets_index]);
        for(int i = 0; i < info.moons; i++)
            get_planet(planets[planets_index],in,0);//the index doesn't matter if the parent * is non null
    }
    else
    {
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
    /*int mainmenu = */glutCreateMenu( MainMenuHandler );
    glutAddSubMenu("Display Method",displaySubMenu);
    glutAddSubMenu("Jump To",jumpToSubMenu);    
    glutAddMenuEntry("Unlock Camera",value++);
    // right button click activates menu
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

// MainMenuHandler() controls the main menu
// only action is to print selected menu entry
void MainMenuHandler( int item )
{
    switch ( item )
    {
        case 1:
            target_lock = NULL;
            break;
        default:    // should not occur
            cout << "invalid main menu item " << item << endl;
            break;
    }
}

void displaySubMenuHandler( int item )
{
    Planet::set_draw_mode(item - 1);
}

// SubMenuHandler() controls the submenu
// only action is to print selected submenu entry
void jumpToSubMenuHandler( int item )
{
    long double x,y;
    target_lock = all_celestial_bodies[item-1];
    all_celestial_bodies[item-1]->get_location(x,y);
}
