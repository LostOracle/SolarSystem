#include "../include/solar.h"

// global variables
double last_click_x = -1;
double last_click_y = -1;
int num_planets;
bool left_dragging = false;
bool paused = false;
Planet ** planets;
vector<Planet *> all_celestial_bodies;
Planet * target_lock;
//Some handy mouse handling constants provided by Dr. Weiss
camera_position CameraPos;
camera_velocity CameraVel;

int ScreenWidth  = 600;
int ScreenHeight = 600;


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
            //zooming doesn't make sense when there's no locked target
            if(target_lock == NULL)
                return;
            zoom(zoom_amount);
        }
    }
    else if(button == 4)
    {
        if(state == GLUT_DOWN)
        {
            if(target_lock == NULL)
                return;
            zoom(-zoom_amount);
        }
    }
    else
        cout << "button: " << button << "state: " << state << endl;
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
    }
}

void special_keyboard( int key, int x, int y)
{
    y = ScreenHeight -y;
    cerr << "Special Keypress: " << key << " (" << int( key ) << ") at (" << x << "," << y << ")\n";

    switch(key)
    {
    case GLUT_KEY_PAGE_UP:
        zoom(zoom_amount);
        break;
    case GLUT_KEY_PAGE_DOWN:
        zoom(-zoom_amount);
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
    glutKeyboardUpFunc( keyboardUp);
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
    Planet::set_draw_mode(item - 1);
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
