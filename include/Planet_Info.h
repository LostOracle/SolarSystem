#ifndef PLANET_INFO_H
#define PLANET_INFO_H

struct Planet_Info
{
    char name[128];
    long double r;
    long double o_r;
    long double th;
    long double o_v;
    long double r_s;
    long double t;
    double color[3]; 
    char texture[1024];
};


#endif
