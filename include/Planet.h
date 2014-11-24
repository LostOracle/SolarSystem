#ifndef PLANET_H
#define PLANET_H
#include "Planet_Info.h"
#include <string.h>
#include <GL/freeglut.h>
#include <math.h>
class Planet
{
    public:
           // Planet(const char* planet_name, double planet_radius, double orbital_radius, 
           //        double theta, double orbital_velocity, double rotation_speed, double tilt, 
           //       const double* color, const char* file_name );
            Planet(Planet_Info &info);
            ~Planet();

 
            void animate();

            static void set_time_step(const long double &new_time_step);
            static long double get_time_step();

            static void set_draw_mode(const char &new_draw_mode);
            static char get_draw_mode();
    
            //set lighting
            void set_diffuse(const double &r, const double &g, const double &b);
            void set_diffuse(const double new_diffuse[]);
            void set_specular( const double &r, const double &g, const double &b);
            void set_specular( const double new_specular[]);
            //get lighting
            void get_diffuse( double old_diffuse[] );
            void get_specular( double old_specular[] );
 
            void get_planet_name(char * out_str);
            int get_num_moons();
            Planet* add_moon(Planet_Info &info);
    private:

            static char draw_mode;
            static long double time_step;
            //planet radius
            long double r;
            //distance from sun
            long double orbital_r;
            //position around sun
            long double theta;
            //speed of planet orbit
            long double orbital_v;
            //planet rotation postion
            long double phi;
            //how fast the planet rotates
            long double rotation_s;
            long double tilt;
            double color[3];
            char texture_name[1024];
            char planet_name[128];
            long double x, y, z;                   
            //Lighting reflections 
            double diffuse[3];
            double specular[3];

            //animation functions for setting OpenGL variables
            //and draw modes
            void animate_wire();
            void animate_flat();
            void animate_smooth();
            void animate_texture();
            

            Planet **moons;
            int num_moons;
            int allocated_moons;
};


#endif
