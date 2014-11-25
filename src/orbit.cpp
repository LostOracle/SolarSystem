/******************************************************************************
 * File: solar.cpp
 * Authors: Ian Carlson, Chris Smith
 * Date: 11/25/2014
 * Description: The entry point for the program solar. This file also contains
 *  code to read in planets from the file planet_info.inf, in addition to setting
 *  up most of the OpenGL constants and settings.
 *  **************************************************************************/

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

//a vector - we got lazy - of all the objects including moons, which are also
//planets but harder to get to
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

/******************************************************************************
 * Function: Animate
 * Authors: Ian Carlson, Chris Smith
 * Parameters: none
 * Returns: none
 * Description: This function handles updating the planets with their new
 *  locations by calling the Planet class's update method. Moons are handled
 *  by their parent planet, so they don't have to be animated directly.
 * ***************************************************************************/
void Animate( void )
{
    // Clear the redering window
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    //update the positions of the planets if we're not paused
    if(!paused)
        for(int i = 0; i < num_planets; i++)
            planets[i]->animate();

    //track the target planet, if any
    if(target_lock != NULL)
    {
        long double target_x, target_y;
        long double vect_x, vect_y,vect_z;
        //get our current view vector so we can maintain it when jumping
        //to the new planet
        vect_x = CameraPos.at_x - CameraPos.ey_x;
        vect_y = CameraPos.at_y - CameraPos.ey_y;
        vect_z = CameraPos.at_z - CameraPos.ey_z;

        //get the location of our target
        target_lock->get_location(target_x,target_y);

        //move the camera 'eye' and 'at' to track the target
        CameraPos.at_x = target_x;
        CameraPos.at_y = target_y;
        CameraPos.at_z = 0;
        CameraPos.ey_x = CameraPos.at_x - vect_x;
        CameraPos.ey_y = CameraPos.at_y - vect_y;
        CameraPos.ey_z = CameraPos.at_z - vect_z;
    }

    glLoadIdentity();
    //update the virtual camera with new coordinates
    gluLookAt(CameraPos.ey_x,CameraPos.ey_y,CameraPos.ey_z,
              CameraPos.at_x,CameraPos.at_y,CameraPos.at_z,
              CameraPos.up_x,CameraPos.up_y,CameraPos.up_z);

    //Hardcoded the sun's light source to always be at 0,0,0
    GLfloat light_position[] = { 0, 0, 0, 1.0 };
    glLightfv( GL_LIGHT0, GL_POSITION, light_position );

    //draw all of the planets
    for(int i = 0; i < num_planets; i++)
    {
        glPushMatrix();
        planets[i]->draw();
        glPopMatrix();
    }
    //update the screen
    glFlush();
    glutSwapBuffers();

    //update the screen as fast as possible
    glutPostRedisplay();
}

/******************************************************************************
 * Function: OpenGlInit
 * Authors: Ian Carlson, Chris Smith
 * Parameters: none
 * Returns: none
 * Description: This function sets up all the gl parameters we used in the
 *  program.
 * ***************************************************************************/
void OpenGLInit( void )
{
    // Create and position the graphics window
    glutInitWindowPosition( 0, 0 );
    glutInitWindowSize( 600, 360 );
    glutCreateWindow( "Solar System" );

    // specify light source properties
    GLfloat light_ambient[] = { 0, 0, 0, 1.0 };
    GLfloat light_diffuse[] = { 1, 1, 1, 1.0 };
    GLfloat light_specular[] = { 1, 1, 1, 1.0 };

    //enable the only light source, the sun and set its properties
    glEnable(GL_LIGHT0);
    glLightfv( GL_LIGHT0, GL_AMBIENT, light_ambient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, light_diffuse );
    glLightfv( GL_LIGHT0, GL_SPECULAR, light_specular );

    glEnable( GL_DEPTH_TEST );  // enable depth buffer for hidden-surface elimination
    glEnable( GL_NORMALIZE );   // automatic normalization of normals
    glEnable( GL_CULL_FACE );   // eliminate backfacing polygons

    //enable the lighting model
    glEnable( GL_LIGHTING ) ;

    //allow coloring surfaces in addition to the lighting model
    glEnable(GL_COLOR_MATERIAL);

    //set background color to black
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );

    //register callbacks
    glutKeyboardFunc( keyboard );
    glutSpecialFunc(special_keyboard);
    glutMouseFunc( click );
    glutMotionFunc( drag );
    glutReshapeFunc( ResizeWindow );
    glutDisplayFunc( Animate );

    //we have some cleanup to do, so return to main when the window is closed
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
}

/******************************************************************************
 * Function: main
 * Authors: Ian Carlson, Chris Smith
 * Parameters: argc - unused, argv - unused
 * Returns: 0
 * Description: Entry point to the program. Reads in the planet_info.inf
 *  file and instantiates class objects, then inits gl and drops into the
 *  gl event loop.
 * ***************************************************************************/
int main( int argc, char** argv )
{
    //Check for the info file
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
        get_planet(NULL,in,i);
    fclose(in);

    //start off locked on the sun, which is the first planet in the file
    target_lock = planets[0];

    //set up our camera variables to a nice default view
    init_camera();

    //initialize glut for double buffering and 3D
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );

    OpenGLInit();

    //set up the right-click menus
    CreateMenus();

    // Start the main loop
    glutMainLoop( );

    //delete everything we allocated
    for(int i = 0; i < num_planets; i++)
        delete planets[i];
    delete []planets;
    return 0;
}

/******************************************************************************
 * Function: get_planet
 * Authors: Ian Carlson, Chris Smith
 * Parameters:
 *  parent - a pointer to the parent planet, if this is a moon
 *  in - a pointer to the input file
 *  planets_index - a little hacky, a pointer to the spot in the planets
 *      array to be filled with a new planet object
 * Returns: none
 * Description: Reads a line from the planet_info.inf file and checks for rings
 *  and moons. In the case of moons, the function is called recursively.
 * ***************************************************************************/
void get_planet(Planet * parent, FILE *& in, int planets_index)
{

    Planet_Info info;
    //Monster scanf to read in a whole line
    fscanf(in,"%s %Lg %Lg %Lg %Lg %Lg %Lg %Lg %lf %lf %lf %s %d %d",
           info.name,&info.r,&info.o_r,&info.th,&info.o_v,&info.phi,&info.r_s,
           &info.t,&info.color[0],&info.color[1],&info.color[2],info.texture,
           &info.moons, &info.rings);
    //read in another few values if there are rings
    if(info.rings)
        fscanf(in,"%Lg %Lg %s",&info.inner_r, &info.outer_r, info.ring_texture);

    //If this is a planet, create it and add it to the planets array and the
    //celestial bodies vector
    if(NULL == parent)
    {
        planets[planets_index] = new Planet(info,NULL);
        all_celestial_bodies.push_back(planets[planets_index]);
        for(int i = 0; i < info.moons; i++)
        {
            //the index doesn't matter if the parent * is non null
            get_planet(planets[planets_index],in,0);
        }
    }
    //if this object is a moon, have the parent planet handle it, but add a pointer to our
    //celestial bodies array so we can track it via the right click menu
    else
    {
        Planet * new_parent = parent->add_moon(info);
        for(int i = 0; i < info.moons; i++)
            get_planet(new_parent,in,0);//the index doesn't matter if the parent * is non null
        all_celestial_bodies.push_back(new_parent);
    }
}

/******************************************************************************
 * Function: CreateMenus
 * Authors: Ian Carlson, Chris Smith
 * Parameters: none
 * Returns: none
 * Description: Builds the right-click menus.
 * ***************************************************************************/
void CreateMenus()
{
    int value = 1;

    //create the display method submenu
    int displaySubMenu = glutCreateMenu( displaySubMenuHandler );
    glutAddMenuEntry( "Wireframe", value++ );
    glutAddMenuEntry( "Flat Shading", value++ );
    glutAddMenuEntry( "Smooth Shading", value++ );
    glutAddMenuEntry( "Textures", value++ );

    value = 1;
    //create the jump-to submenu
    int jumpToSubMenu = glutCreateMenu(jumpToSubMenuHandler);
    for(unsigned int i = 0; i < all_celestial_bodies.size(); i++)
    {
        char name[1024];
        all_celestial_bodies[i]->get_planet_name(name);
        glutAddMenuEntry(name,value++);
    }

    value = 1;
    //create the main right-click menu
    glutCreateMenu( MainMenuHandler );
    glutAddSubMenu("Display Method",displaySubMenu);
    glutAddSubMenu("Jump To",jumpToSubMenu);
    glutAddMenuEntry("Unlock Camera",value++);
    glutAddMenuEntry("Increase Timestep(+)",value++);
    glutAddMenuEntry("Decrease Timestep(-)",value++);
    glutAddMenuEntry("Increase Resolution(*)",value++);
    glutAddMenuEntry("Decrease Resolution(/)",value++);
    glutAddMenuEntry("Pause/Resume(p)",value++);
    glutAddMenuEntry("Single Step(t)",value++);
    glutAddMenuEntry("Help",value++);
    glutAttachMenu( GLUT_RIGHT_BUTTON );
}

/******************************************************************************
 * Function: MainMenuHandler
 * Authors: Ian Carlson, Chris Smith
 * Parameters: int item
 * Returns: none
 * Description: handles main menu clicks, currently only unlocks the camera
 *  from a planet
 * ***************************************************************************/
void MainMenuHandler( int item )
{
    switch ( item )
    {
    //release the lock on the target planet, if any
    case 1:
        target_lock = NULL;
        break;
    case 2:
        Planet::increment_time_step(TIME_STEP_COEFF);
        break;
    case 3:
        Planet::increment_time_step(-TIME_STEP_COEFF);
        break;
    //increase object resolution
    case 4:
        Planet::increment_wire_res();
        break;
    //decrease objecct resolution
    case 5:
        Planet::decrement_wire_res();
        break;
    //pause
    case 6:
        paused = true;
        break;
    //single step
    case 7:
        paused = true;
        //take one timestep forward
        for(int i = 0; i < num_planets; i++)
            planets[i]->animate();
        break;
    //display readme for detailed help
    case 8:
        system("gedit Readme.txt");
        break;
    default:    // should not occur
        cout << "invalid main menu item " << item << endl;
        break;
    }
}

/******************************************************************************
 * Function: displaySubMenuHandler
 * Authors: Ian Carlson, Chris Smith
 * Parameters: int item
 * Returns: none
 * Description: handles clicks to the display method sub menu, calls a static
 *  class method in Planet to change how the planets are drawn
 * ***************************************************************************/
void displaySubMenuHandler( int item )
{
    Planet::set_draw_mode(item - 1);
}

/******************************************************************************
 * Function: displaySubMenuHandler
 * Authors: Ian Carlson, Chris Smith
 * Parameters: int item
 * Returns: none
 * Description: handles clicks to the jump to menu. Tells the camera to follow
 *  the selected planet
 * ***************************************************************************/
void jumpToSubMenuHandler( int item )
{
    target_lock = all_celestial_bodies[item-1];
}
