/************************************************************************
* File: Planet.h
* Author: Chris Smith, Ian Carlson
* Description: Planet class and functions used to read in bmp files
* that Dr. Weiss wrote.
* Date: 20 Nov 2014
************************************************************************/
#ifndef PLANET_H
#define PLANET_H
#include "Planet_Info.h"
#include <string.h>
#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
//FUNCTION PROTOTYPES THAT DR. WEISS WROTE.
bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, unsigned char* &ImagePtr );
short readShort( FILE* infile );
int readLong( FILE* infile );
void skipChars( FILE* infile, int numChars );
inline int GetNumBytesPerRow( int NumCols );

//Planet Class
class Planet
{
public:
    // Planet(const char* planet_name, double planet_radius, double orbital_radius,
    //        double theta, double orbital_velocity, double rotation_speed, double tilt,
    //       const double* color, const char* file_name );
    Planet(Planet_Info &info, Planet *ptr);
    ~Planet();


    void animate();

    static void set_time_step(const long double &new_time_step);
    static long double get_time_step();

    static void set_draw_mode(const char &new_draw_mode);
    static char get_draw_mode();
    static void increment_time_step( const long double &increment);
    //set lighting
    void set_diffuse(const double &r, const double &g, const double &b);
    void set_diffuse(const double new_diffuse[]);
    void set_specular( const double &r, const double &g, const double &b);
    void set_specular( const double new_specular[]);
    //get lighting
    void get_diffuse( double old_diffuse[] );
    void get_specular( double old_specular[] );
    static void increment_wire_res();
    static void decrement_wire_res();
    void draw();
    void get_location( long double &x, long double &y);
    void get_planet_name(char * out_str);
    int get_num_moons();
    Planet* add_moon(Planet_Info &info);
    long double get_radius();
private:
    static double wire_res;
    int texture_rows;
    int texture_cols;
    unsigned char * image;
    GLUquadricObj* sphere;
    GLUquadricObj* ring_obj;
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
public:
    long double phi;
private:
    //how fast the planet rotates
    long double rotation_s;
    long double tilt;
    float color[3];
    char texture_name[1024];
    char planet_name[128];
    long double x, y, z;
    //Lighting reflections
    GLfloat diffuse[4];
    GLfloat specular[4];
    GLfloat ambient[4];
    GLfloat emissivity[4];
    GLfloat shininess;
    //animation functions for setting OpenGL variables
    //and draw modes
    void animate_wire();
    void animate_flat();
    void animate_smooth();
    void animate_texture();

    Planet *parent;
    Planet **moons;
    int num_moons;
    int allocated_moons;
    int rings;
    int ring_cols;
    int ring_rows;
    unsigned char* ring_image;
    long double inner_r;
    long double outer_r;
    char ring_texture[1024];
};


#endif
