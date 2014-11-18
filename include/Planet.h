#define PLANET_H
#ifndef PLANET_H
#include "Planet_Info.h"
#include <string.h>
class Planet
{
    public:
           // Planet(const char* planet_name, double planet_radius, double orbital_radius, 
           //        double theta, double orbital_velocity, double rotation_speed, double tilt, 
           //       const double* color, const char* file_name );
            Planet(planet_info info);
            ~Planet();

            double r;
            double orbital_r;
            double theta;
            double orbital_v;
            double rotation_s;
            double tilt;
            double color[3];
            char texture_name[1024];
            char planet_name[128];
            double x, y, z;           
}


#endif
