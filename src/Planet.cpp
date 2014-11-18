#include "../include/Planet.h"

/*Planet::Planet(const char* planet_name, double planet_radius, double orbital_radius, double theta, 
               double orbital_velocity, double rotation_speed, double tilt, const double * color, 
               const char * texture_filename)
{
    

}
*/

Planet::Planet( Planet_Info &info ):r(info.r), orbital_r(info.o_r), theta(info.th), orbital_v(info.o_v), rotation_s(info.r_s), tilt(info.t)
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
