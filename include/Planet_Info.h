#ifndef PLANET_INFO_H
#define PLANET_INFO_H

struct Planet_Info
{
    char name[128];
    long double r;
    long double o_r;
    long double th;
    long double o_v;
    long double phi;
    long double r_s;
    long double t;
    double color[3]; 
    char texture[1024];
    int moons;
    int rings;
    long double inner_r;
    long double outer_r;
    char ring_texture[1024];
};


#endif
