/************************************************************************
* File: Planet.cpp
* Author: Chris Smith, Ian Carlson
* Description: Src file that implements the Planet class from the Planet.h
* As well as the functions that Dr. Weiss created for reading bmp images.
* Date: 20 Nov 2014
************************************************************************/

#include "../include/Planet.h"


/************************************************************************
   Function:Planet::Planet
   Author: Chris Smith, Ian Carlson
   Description: Constructor for the planet class that takes an Planet info argument and a Ptr argument
   Parameters: info is the information of the planet to draw correctly
               ptr is a pointer to the parent of the object so that a moon would have a parent but a planet wouldn't
   Returns: None
   Description: Constructor for the planet class that takes a Planet info argument and a ptr argument
 ************************************************************************/
Planet::Planet( Planet_Info &info, Planet* ptr = NULL  ):r(info.r), orbital_r(info.o_r), theta(info.th), orbital_v(info.o_v),phi(info.phi), rotation_s(info.r_s), tilt(info.t), num_moons(info.moons), rings(info.rings)
{
    //Copies the array of colors into class variable
    int i;
    for( i = 0; i < 3; i++)
        color[i] = info.color[i];
    //copys the file name of the texture image
    strcpy(texture_name, info.texture);
    if ( !LoadBmpFile(texture_name, texture_rows, texture_cols, image ) )
    {
        printf("failed to load texture.\n");
        return;
    }
    //if the planet has rings it stores the extra information
    strcpy(planet_name, info.name);
    if( 1 == rings )
    {
        inner_r = info.inner_r;
        outer_r = info.outer_r;
        strcpy(ring_texture, info.ring_texture);
        if(!LoadBmpFile(ring_texture, ring_rows, ring_cols, ring_image))
        {
            return;
        }
    }
    //if planet has moons an array of moon objects is created
    moons = new Planet* [num_moons];
    allocated_moons = 0;
    //if the object is a moon it has a parent planet
    parent = ptr;
    //initialises all lighting effects for planets
    ambient[0] = .5;
    ambient[1] = .5;
    ambient[2] = .5;
    ambient[3] = 1;
    diffuse[0]= 0.6;
    diffuse[1]= 0.6;
    diffuse[2]= 0.6;
    diffuse[3]= 1;
    specular[0] = .2;
    specular[1] = .2;
    specular[2] = .2;
    specular[3] = 1;
    //if the planet is the sun
    if( orbital_r == 0)
    {
        emissivity[0] = 1;
        emissivity[1] = 1;
        emissivity[2] = 0;
        emissivity[3] = 1;
    }
    else
    {
        emissivity[0] = 0;
        emissivity[1] = 0;
        emissivity[2] = 0;
        emissivity[3] = 1;
    }
    shininess = 0;
}
/************************************************************************
   Function:Planet::Planet
   Author: Chris Smith, Ian Carlson
   Parameters:None
   Returns: None
   Description: Deconstructor for the planet class that deallocates memory
 ************************************************************************/
Planet::~Planet()
{
    for( int i = 0; i < allocated_moons; i++)
        delete moons[i];
    delete []moons;
    delete []image;
}
/************************************************************************
   Function:Planet::Planet
   Author: Chris Smith, Ian Carlson
   Parameters: out_str is the name of the planet to be given back to caller
   Returns: None
   Description:Returns the caller the planet's name
 ************************************************************************/
void Planet::get_planet_name(char * out_str)
{
    strcpy(out_str,planet_name);
}
/************************************************************************
   Function:Planet::add_moon
   Author: Chris Smith, Ian Carlson
   Parameters: info is thee information of the planet to draw correctly
   Returns: Planet*
   Description: Constructor for the Moon class that tells the constructor that it is the parent
 ************************************************************************/
Planet* Planet::add_moon(Planet_Info &info )
{
    moons[allocated_moons] = new Planet(info, this);
    allocated_moons +=1;
    return moons[ allocated_moons - 1 ];
}
/************************************************************************
   Function:Planet::get_location
   Author: Chris Smith, Ian Carlson
   Parameters: x,y (in,out) are the x and y locations of the planet to be returned
   Description: calculates the location of the planet or the moon if it has a parent
 ************************************************************************/
void  Planet::get_location( long double &x, long double &y )
{
    if(NULL !=  parent)
    {
        parent->get_location(x,y);
        x += (orbital_r*cos(M_PI*(theta+parent->theta)/180.0));
        y += (orbital_r*sin(M_PI*(theta+parent->theta)/180.0));
    }
    else
    {
        x = (orbital_r*cos(M_PI*theta/180));
        y = (orbital_r*sin(M_PI*theta/180));
    }
}
/************************************************************************
   Function:Planet::Planet
   Author: Chris Smith, Ian Carlson
   Parameters: None
   Returns: None
   Description: Calculates the location for the planets at next time step
 ************************************************************************/
void Planet::animate()
{
    theta += (orbital_v *time_step);
    while( theta > 360 )
        theta -= 360;

    phi += ( rotation_s * time_step );
    while( phi > 360 )
        phi -= 360;

    for( int i = 0; i < allocated_moons; i++ )
        moons[i]->animate();



}
/************************************************************************
   Function:Planet::draw
   Author: Chris Smith, Ian Carlson
   Parameters:None
   Returns: None
   Description: Draws the planets and then the corresponding shading model
 ************************************************************************/
void Planet::draw( )
{
    emissivity[2] = 0;
    // Draw the Planet
    glColor3f(1,1,1);
    glMaterialfv( GL_FRONT, GL_AMBIENT, ambient );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, shininess );

    //draw orbital track--------------
    glPushMatrix();
    glMaterialfv( GL_FRONT, GL_EMISSION, color );
    //translate to the sun to draw the orbital torus
    //the torus draws 90 degrees to everything else for some reason
    glRotatef(90.0,1.0,0.0,0.0);
    glTranslatef(-orbital_r,0.0,0.0);
    glutWireTorus(orbital_r,orbital_r,100,1);
    glPopMatrix();
    //--------------------------------

    //place planet--------------------------------
    glPushMatrix();
    glRotatef( theta, 0.0, 0.0, 1.0 );
    glTranslatef( orbital_r, 0.0,0.0 );
    glMaterialfv(GL_FRONT, GL_EMISSION, emissivity);
    // Second, rotate the planet on its axis.
    glPushMatrix();
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
        emissivity[2] = 1;
        if(0 == orbital_r)
            glMaterialfv(GL_FRONT, GL_EMISSION,emissivity );
        animate_texture();
    }
    glPopMatrix();

    //draw text----------------------
    glPushMatrix();
    glMaterialfv(GL_FRONT, GL_EMISSION, color);
    glRotatef( phi, 0, 0, 1 );
    glTranslatef( 0.0, 0.0, r * 2.0 );
    glRotatef( 90, 1, 0, 0 );
    glScalef( 100, 100, 100 );
    glColor3f( color[0], color[1], color[2] );
    glutStrokeString(GLUT_STROKE_ROMAN, (const unsigned char *)planet_name);
    glPopMatrix();
    //------------------------------

    glPushMatrix();
    for( int i = 0; i < allocated_moons; i++)
    {
        moons[i]->draw();
    }
    glPopMatrix();
    glPopMatrix();
}
/************************************************************************
   Function:Planet::set_time_step
   Author: Chris Smith, Ian Carlson
   Description: Constructor for the planet class that takes an Planet info argument and a Ptr argument
   Parameters: new_time_step - value to set the time step to
   Returns: None
   Description: sets the time step
 ************************************************************************/
void Planet::set_time_step( const long double &new_time_step )
{
    time_step = new_time_step;
}
/************************************************************************
   Function:Planet::increment_time_step
   Author: Chris Smith, Ian Carlson
   Parameters: info is the information of the planet to draw correctly
   Returns: None
   Description: Adds or subtracts from time_step for slowdowns or increases
 ************************************************************************/
void Planet::increment_time_step( const long double &increment )
{
    time_step += increment;
    if( time_step  < 0 )
        time_step = 0;
}
/************************************************************************
   Function:Planet::set_draw_mode
   Author: Chris Smith, Ian Carlson
   Parameters: new_draw_mode - passed in via the right click menu
   Returns: None
   Description: Sets the draw mode
 ************************************************************************/
void Planet::set_draw_mode( const char &new_draw_mode )
{
    draw_mode = new_draw_mode;
}
//gets the draw mode
char Planet::get_draw_mode()
{
    return draw_mode;
}

/************************************************************************
   Function:Planet::animate_wire
   Author: Chris Smith, Ian Carlson
   Parameters:None
   Returns: None
   Description: Draws the planet with wires and flat shading
 ************************************************************************/
void Planet::animate_wire()
{
    glShadeModel( GL_FLAT );
    // Third, draw the earth as a wireframe sphere.
    glColor3f( color[0], color[1], color[2] );
    glutWireSphere(r,wire_res,wire_res );
    if(rings)
    {
        glDisable( GL_CULL_FACE);
        ring_obj = gluNewQuadric();
        glColor3fv(color);
        gluQuadricDrawStyle( ring_obj, GLU_FILL );
        gluQuadricNormals( ring_obj, GLU_FLAT);
        gluDisk( ring_obj,inner_r, outer_r,wire_res,wire_res);
        glEnable(GL_CULL_FACE);
    }
}
/************************************************************************
   Function:Planet::animate_flat
   Author: Chris Smith, Ian Carlson
   Parameters: None
   Returns: None
   Description: Draws the planet with flat shading
 ************************************************************************/
void Planet::animate_flat()
{
    glShadeModel( GL_FLAT );
    sphere = gluNewQuadric( );
    glColor3f( color[0], color[1], color[2] );
    gluQuadricDrawStyle( sphere, GLU_FILL );
    gluQuadricNormals( sphere, GLU_FLAT );
    gluSphere( sphere, r, wire_res, wire_res );
    if(rings)
    {
        glDisable( GL_CULL_FACE);
        ring_obj = gluNewQuadric();
        glColor3fv(color);
        gluQuadricDrawStyle( ring_obj, GLU_FILL );
        gluQuadricNormals( ring_obj, GLU_FLAT);
        gluCylinder( ring_obj,inner_r, outer_r,1,wire_res,wire_res);
        glEnable( GL_CULL_FACE );
    }
}
/************************************************************************
   Function:Planet::animate_smooth
   Author: Chris Smith, Ian Carlson
   Description: Constructor for the planet class that takes an Planet info argument and a Ptr argument
   Parameters:None
   Returns: None
   Description: Draws the planet with smooth shading
 ************************************************************************/
void Planet::animate_smooth()
{
    glShadeModel( GL_SMOOTH );
    sphere = gluNewQuadric( );
    glColor3f( color[0], color[1], color[2] );
    gluQuadricDrawStyle( sphere, GLU_FILL );
    gluQuadricNormals( sphere, GLU_SMOOTH );
    gluSphere( sphere, r, 64, 64 );
    if(rings)
    {
        glDisable( GL_CULL_FACE );
        ring_obj = gluNewQuadric();
        glColor3fv(color);
        gluQuadricDrawStyle( ring_obj, GLU_FILL );
        gluQuadricNormals( ring_obj, GLU_SMOOTH);
        gluCylinder( ring_obj,inner_r, outer_r,1,wire_res,wire_res);
        glEnable( GL_CULL_FACE );
    }
}
/************************************************************************
   Function:Planet::animate_texture
   Author: Chris Smith, Ian Carlson
   Description: Constructor for the planet class that takes an Planet info argument and a Ptr argument
   Parameters: None
   Returns: None
   Description: Draws the planet with its corresponding texture map
 ************************************************************************/
void Planet::animate_texture()
{
    glShadeModel( GL_SMOOTH );

    glEnable(GL_TEXTURE_2D);
    // Pixel alignment: each row is word aligned (aligned to a 4 byte boundary)
    // Therefore, no need to call glPixelStore( GL_UNPACK_ALIGNMENT, ... );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, texture_cols, texture_rows, GL_RGB, GL_UNSIGNED_BYTE, image );

// generate GLU quadric sphere with surface normals and texture coordinates
    sphere = gluNewQuadric( );
    gluQuadricDrawStyle( sphere, GLU_FILL );
    gluQuadricNormals( sphere, GLU_SMOOTH );
    gluQuadricTexture( sphere, GL_TRUE );
    gluSphere( sphere, r, wire_res, wire_res );
    glDisable(GL_TEXTURE_2D);
    if(rings)
    {
        glDisable( GL_CULL_FACE);
        glEnable(GL_TEXTURE_2D);
        // Pixel alignment: each row is word aligned (aligned to a 4 byte boundary)
        // Therefore, no need to call glPixelStore( GL_UNPACK_ALIGNMENT, ... );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, ring_cols, ring_rows, GL_RGB, GL_UNSIGNED_BYTE, ring_image );

        ring_obj = gluNewQuadric();
        glColor3fv(color);
        gluQuadricDrawStyle( ring_obj, GLU_FILL );
        gluQuadricNormals( ring_obj, GLU_SMOOTH);
        gluQuadricTexture(ring_obj, GL_TRUE );
        gluCylinder( ring_obj,inner_r, outer_r,1,wire_res,wire_res);
        glDisable(GL_TEXTURE_2D);
        glEnable( GL_CULL_FACE );
    }
}
/************************************************************************
   Function:Planet::increment_wire_res
   Author: Chris Smith, Ian Carlson
   Parameters: none
   Returns: increments teh resolution of the wireframes
   Description: returns the orbital radius from the sun
 ************************************************************************/

void Planet::increment_wire_res()
{
    wire_res +=2;
    if(wire_res > 200 )
        wire_res = 200;
}


/************************************************************************
   Function:Planet::decrement_wire_res
   Author: Chris Smith, Ian Carlson
   Parameters: none
   Returns: none
   Description: decrements the wire resolution
 ************************************************************************/

void Planet::decrement_wire_res()
{
    wire_res-=2;
    if( wire_res < 2 )
        wire_res = 2;
}
/************************************************************************
   Function:Planet::Planet
   Author: Chris Smith, Ian Carlson
   Parameters: none
   Returns: orbital_r the radius
   Description: returns the orbital radius from the sun
 ************************************************************************/
long double Planet::get_radius()
{
    return orbital_r;
}

/************************************************************************
   Function: LoadBmpFile
   Author: Dr. Weiss
   Parameters: filename - file that is the image
               NumRows - number of rows in image file
               NumCols - Number of columns in image file
               ImagePtr - points to the image and stores in memory
   Returns: bool succeful read of file
   Description: Reads in a bmp image file and stores it in memory
 ************************************************************************/
bool LoadBmpFile( const char* filename, int &NumRows, int &NumCols, unsigned char* &ImagePtr )
{
    FILE* infile = fopen( filename, "rb" );     // Open for reading binary data
    if ( !infile )
    {
        fprintf( stderr, "LoadBmpFile(): unable to open file: %s\n", filename );
        return false;
    }

    bool fileFormatOK = false;
    int bChar = fgetc( infile );
    int mChar = fgetc( infile );
    if ( bChar == 'B' && mChar == 'M' )
    {   // If starts with "BM" for "BitMap"
        skipChars( infile, 4 + 2 + 2 + 4 + 4 );         // Skip 4 fields we don't care about
        NumCols = readLong( infile );
        NumRows = readLong( infile );
        skipChars( infile, 2 );                 // Skip one field
        int bitsPerPixel = readShort( infile );
        skipChars( infile, 4 + 4 + 4 + 4 + 4 + 4 );     // Skip 6 more fields

        if ( NumCols > 0 && NumCols <= 100000 && NumRows > 0 && NumRows <= 100000
                && bitsPerPixel == 24 && !feof( infile ) )
        {
            fileFormatOK = true;
        }
    }

    if ( !fileFormatOK )
    {
        fclose ( infile );
        fprintf( stderr, "Not a valid 24-bit bitmap file: %s.\n", filename );
        return false;
    }

    // Allocate memory
    ImagePtr = new unsigned char[ NumRows * GetNumBytesPerRow( NumCols ) ];
    if ( !ImagePtr )
    {
        fclose ( infile );
        fprintf( stderr, "Unable to allocate memory for %i x %i bitmap: %s.\n",
                 NumRows, NumCols, filename );
        return false;
    }

    unsigned char* cPtr = ImagePtr;
    for ( int i = 0; i < NumRows; i++ )
    {
        int j;
        for ( j = 0; j < NumCols; j++ )
        {
            *( cPtr + 2 ) = fgetc( infile );    // Blue color value
            *( cPtr + 1 ) = fgetc( infile );    // Green color value
            *cPtr = fgetc( infile );        // Red color value
            cPtr += 3;
        }
        int k = 3 * NumCols;                    // Num bytes already read
        for ( ; k < GetNumBytesPerRow( NumCols ); k++ )
        {
            fgetc( infile );                // Read and ignore padding;
            *( cPtr++ ) = 0;
        }
    }

    if ( feof( infile ) )
    {
        fclose ( infile );
        fprintf( stderr, "Premature end of file: %s.\n", filename );
        return false;
    }

    fclose( infile );   // Close the file

    return true;
}
//Author: Dr. Weiss
// Rows are word aligned
inline int GetNumBytesPerRow( int NumCols )
{
    return ( ( 3 * NumCols + 3 ) >> 2 ) << 2;
}

//Author: Dr. Weiss
// read a 16-bit integer from the input file
short readShort( FILE* infile )
{
    unsigned char lowByte, hiByte;
    lowByte = fgetc( infile );          // Read the low order byte (little endian form)
    hiByte = fgetc( infile );           // Read the high order byte

    // Pack together
    short ret = hiByte;
    ret <<= 8;
    ret |= lowByte;
    return ret;
}

//Author: Dr. Weiss
// read a 32-bit integer from the input file
int readLong( FILE* infile )
{
    unsigned char byte0, byte1, byte2, byte3;
    byte0 = fgetc( infile );            // Read bytes, low order to high order
    byte1 = fgetc( infile );
    byte2 = fgetc( infile );
    byte3 = fgetc( infile );

    // Pack together
    int ret = byte3;
    ret <<= 8;
    ret |= byte2;
    ret <<= 8;
    ret |= byte1;
    ret <<= 8;
    ret |= byte0;
    return ret;
}

//Author: Dr. Weiss
// skip over given number of bytes in input file
void skipChars( FILE* infile, int numChars )
{
    for ( int i = 0; i < numChars; i++ )
    {
        fgetc( infile );
    }
}
/*******STATIC VARIABLES FOR CLASS**********/
long double Planet::time_step = 0.5;
char Planet::draw_mode = 0;
double Planet::wire_res = 64;
