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
    if ( !LoadBmpFile(texture_name, texture_rows, texture_cols, image ) )
    {
        printf("failed to load texture.\n");
        return;
    }
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
    moons = new Planet* [num_moons];
    allocated_moons = 0;
    parent = ptr;

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

Planet::~Planet()
{
    for( int i = 0; i < allocated_moons; i++)
        delete moons[i];
    delete []moons;
    delete []image;
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

void  Planet::get_location( long double &x, long double &y )
{
    x = 0;
    y = 0;
    if(NULL !=  parent)
    {
        char name[1000];
        parent->get_planet_name(name);
        parent -> get_location(x,y);
    }
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
        phi -= 360;

 for( int i = 0; i < allocated_moons; i++ ) 
    moons[i]->animate();



}

void Planet::draw( )
{
    // Draw the Planet
    // First position it around the sun. Use DayOfYear to determine its position.
    glColor3f(1,1,1);
    glMaterialfv( GL_FRONT, GL_AMBIENT, ambient );
    glMaterialfv( GL_FRONT, GL_DIFFUSE, diffuse );
    glMaterialfv( GL_FRONT, GL_SPECULAR, specular );
    glMaterialf( GL_FRONT, GL_SHININESS, shininess );
    glPushMatrix();
    glMaterialfv( GL_FRONT, GL_EMISSION, color );
    //translate to the sun to draw the orbital torus
    //the torus draws 90 degrees to everything else for some reason
    glRotatef(90.0,1.0,0.0,0.0);
    glTranslatef(-orbital_r,0.0,0.0);
    glutWireTorus(orbital_r,orbital_r,100,1);
    glPopMatrix();
    glRotatef( theta, 0.0, 0.0, 1.0 );
    glTranslatef( orbital_r, 0.0,0.0 );
    glMaterialfv(GL_FRONT, GL_EMISSION, emissivity); 
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
    glMaterialfv(GL_FRONT, GL_EMISSION, color);
    glRotatef( phi, 0, 0, 1 );
    glTranslatef( 0.0, 0.0, r * 2.0 );
    glRotatef( 90, 1, 0, 0 );
    glScalef( 100, 100, 100 );
    glColor3f( color[0], color[1], color[2] );
    glutStrokeString(GLUT_STROKE_ROMAN, (const unsigned char *)planet_name);
    glPopMatrix();

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
    glShadeModel( GL_FLAT );
    // Third, draw the earth as a wireframe sphere.
    glColor3f( color[0], color[1], color[2] );
    glutWireSphere(r,100,100 );
    if(rings)
    {
        glDisable( GL_CULL_FACE);
        ring_obj = gluNewQuadric();
        glColor3fv(color);
        gluQuadricDrawStyle( ring_obj, GLU_FILL );
        gluQuadricNormals( ring_obj, GLU_FLAT);
        gluDisk( ring_obj,inner_r, outer_r,100,1000); 
        glEnable(GL_CULL_FACE);
    }
}
void Planet::animate_flat()
{
    glShadeModel( GL_FLAT );
    sphere = gluNewQuadric( );
    glColor3f( color[0], color[1], color[2] );
    gluQuadricDrawStyle( sphere, GLU_FILL );
    gluQuadricNormals( sphere, GLU_FLAT );
    gluSphere( sphere, r, 64, 64 );
    if(rings)
    {
        glDisable( GL_CULL_FACE);
        ring_obj = gluNewQuadric();
        glColor3fv(color);
        gluQuadricDrawStyle( ring_obj, GLU_FILL );
        gluQuadricNormals( ring_obj, GLU_FLAT);
        gluCylinder( ring_obj,inner_r, outer_r,1,100,1000); 
        glEnable( GL_CULL_FACE );
    }
}
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
        gluCylinder( ring_obj,inner_r, outer_r,1,100,1000); 
        glEnable( GL_CULL_FACE );
    }
}

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
    gluSphere( sphere, r, 64, 64 );
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
        gluCylinder( ring_obj,inner_r, outer_r,1,100,1000); 
        glEnable( GL_CULL_FACE );
    }
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

long double Planet::get_radius()
{
    return orbital_r;
}
   
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
    {           // If starts with "BM" for "BitMap"
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
// Rows are word aligned
inline int GetNumBytesPerRow( int NumCols )
{
    return ( ( 3 * NumCols + 3 ) >> 2 ) << 2;
}


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

// skip over given number of bytes in input file
void skipChars( FILE* infile, int numChars )
{
    for ( int i = 0; i < numChars; i++ )
    {
        fgetc( infile );
    }
}
/*******STATIC VARIABLES FOR CLASS**********/
long double Planet::time_step = 0;
char Planet::draw_mode = 0;
