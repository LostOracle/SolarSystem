#define PLANET_H
#ifndef PLANET_H


class Planet
{
    public:
            Planet(double planet_radius, double orbital_radius, double theta, double orbital_velocity, 
                   double rotation_speed, double tilt,const double *color, const char * file_name );
            ~Planet();

            double r;
            double orbital_r;
            double theta;
            double orbital_v;
            double rotation_s;
            double tilt;
            double color[3];
            char texture_name[1024];
            double x, y, z;           
}


#endif
