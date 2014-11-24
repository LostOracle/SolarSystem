#include "../include/Planet.h"

/*Planet::Planet(const char* planet_name, double planet_radius, double orbital_radius, double theta, 
               double orbital_velocity, double rotation_speed, double tilt, const double * color, 
               const char * texture_filename)
{
    

}
*/

Planet::Planet( Planet_Info &info, Planet* ptr = NULL  ):r(info.r), orbital_r(info.o_r), theta(info.th), orbital_v(info.o_v),phi(info.phi), rotation_s(info.r_s), tilt(info.t), num_moons(info.moons), rings(info.rings)
{
    int i;
    for( i = 0; i < 3; i++)
        color[i] = info.color[i];

    strcpy(texture_name, info.texture);
    strcpy(planet_name, info.name);
    if( 1 == rings )
    {
        inner_r = info.inner_r;
        outer_r = info.outer_r;
        strcpy(ring_texture, info.ring_texture);
    
    }
    moons = new Planet* [num_moons];
    allocated_moons = 0;
    parent = ptr;
}

Planet::~Planet()
{
    for( int i = 0; i < allocated_moons; i++)
        delete moons[i];
    delete []moons;
}

void Planet::get_planet_name(char * out_str)
{
    strcpy(out_str,planet_name);
}

Planet* Planet::add_moon(Planet_Info &info )
{
    moons[allocated_moons] = new Planet(info, this);
    allocated_moons +=1;
    return moons[ allocated_moons - 1 ];
}

#include <iostream>
using namespace std;
void  Planet::get_location( long double &x, long double &y )
{
    x = 0;
    y = 0;
    if(NULL !=  parent)
    {
        char name[1000];
        parent->get_planet_name(name);
        cout << "Parent: " << name << endl;
        parent -> get_location(x,y);
    }
    else
        cout << "No parent" << endl;
    x += (orbital_r*cos(M_PI*theta/180));
    y += (orbital_r*sin(M_PI*theta/180));
}

void Planet::animate()
{
    theta += (orbital_v *time_step);
    while( theta > 360 )
        theta -= 360;

    phi += ( rotation_s * time_step );
    while( phi > 360 )
        rotation_s -= 360;

 for( int i = 0; i < allocated_moons; i++ ) 
    moons[i]->animate();

}

void Planet::draw( )
{
    // Draw the Planet
    // First position it around the sun. Use DayOfYear to determine its position.
    glColor3f(1,1,1);
    
    /*glBegin( GL_LINE_STRIP );
        for ( double i = 0.0; i < 200; i++ )
        {
            glVertex3f( orbital_r/2.0 * cos( 2 * M_PI * i / 200 ),  orbital_r/2.0 * sin( 2 * M_PI * i / 200 ),0 );
        }
    glEnd();*/

    glRotatef( theta, 0.0, 0.0, 1.0 );
    glTranslatef( orbital_r, 0.0,0.0 );
    // Second, rotate the planet on its axis. 
    glRotatef( phi, 0.0, 0.0, 1.0 );   
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
    glPushMatrix();
    for( int i = 0; i < allocated_moons; i++)
    {
        moons[i]->draw();
        glPopMatrix();
        glPushMatrix();
    }
    glPopMatrix();
}

void Planet::set_time_step( const long double &new_time_step )
{
    time_step = new_time_step;
}
long double Planet::get_time_step()
{
    return time_step;
}

void Planet::increment_time_step( const long double &increment )
{
    time_step += increment;
    if( time_step  < 0 )
        time_step = 0;
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
    glColor3f( color[0], color[1], color[2] );
    glutWireSphere(r,100,100 );
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


void Planet::set_diffuse( const double &r, const double &g, const double &b) 
{
    diffuse[0] = r;
    diffuse[1] = g;
    diffuse[2] = b;
}

void Planet::set_diffuse( const double new_diffuse[] )
{
    diffuse[0] = new_diffuse[0];
    diffuse[1] = new_diffuse[1];
    diffuse[2] = new_diffuse[2];
}

void Planet::set_specular(const double &r, const double &g, const double &b) 
{
 specular[0] = r;
 specular[1] = g;
 specular[2] = b;
}

void Planet::set_specular( const double new_specular[] )
{
    specular[0] = new_specular[0];
    specular[1] = new_specular[1];
    specular[2] = new_specular[2];
}

void Planet::get_diffuse(double old_diffuse[] )
{
    old_diffuse[0] = diffuse[0];
    old_diffuse[1] = diffuse[1];
    old_diffuse[2] = diffuse[2];
}

void Planet::get_specular( double old_specular[] )
{
    old_specular[0] = specular[0];
    old_specular[1] = specular[1];
    old_specular[2] = specular[2];
}
   


/*******STATIC VARIABLES FOR CLASS**********/
long double Planet::time_step = 0;
char Planet::draw_mode = 0;
