#include "../include/Planet.h"

/*Planet::Planet(const char* planet_name, double planet_radius, double orbital_radius, double theta, 
               double orbital_velocity, double rotation_speed, double tilt, const double * color, 
               const char * texture_filename)
{
    

}
*/

Planet::Planet( Planet_Info &info ):r(info.r), orbital_r(info.o_r), theta(info.th), orbital_v(info.o_v),phi(info.phi), rotation_s(info.r_s), tilt(info.t)
{
    int i;
    for( i = 0; i < 3; i++)
        color[i] = info.color[i];

    strcpy(texture_name, info.texture);
    strcpy(planet_name, info.name);
}

Planet::~Planet()
{

}



void Planet::animate( )
{

    theta += (orbital_v *time_step);
    while( theta > 360 )
        theta -= 360;

    phi += ( rotation_s * time_step );
    while( rotation_s > 360 )
        rotation_s -= 360;
    
    
    glLoadIdentity(); 
    // Draw the Planet
    // First position it around the sun. Use DayOfYear to determine its position.
    glRotatef( theta, 0.0, 1.0, 0.0 );
    glTranslatef( orbital_r/1000.0, 0.0, -1000.0 );
    // Second, rotate the planet on its axis. 
    glRotatef( phi, 0.0, 1.0, 0.0 );   
    draw_mode = 0;
    if( draw_mode == 0)
    {
        animate_wire();
    }
    else if( draw_mode == 1)
    {
        animate_flat();
    }
    else if( draw_mode == 2)
    {
        animate_smooth();
    }
    else
    {
        animate_texture();
    }
}

void Planet::set_time_step( const long double &new_time_step )
{
    time_step = new_time_step;
}
long double Planet::get_time_step()
{
    return time_step;
}

void Planet::set_draw_mode( const char &new_draw_mode )
{
    draw_mode = new_draw_mode;
}
char Planet::get_draw_mode()
{
    return draw_mode;
}


void Planet::animate_wire()
{
    // Third, draw the earth as a wireframe sphere.
    glColor3f( 1, 1, 1 );
    glutWireSphere( r/1000.0, 100, 100 );
}
void Planet::animate_flat()
{

}
void Planet::animate_smooth()
{

}

void Planet::animate_texture()
{

}


/*******STATIC VARIABLES FOR CLASS**********/
long double Planet::time_step = 0;
char Planet::draw_mode = 0;
