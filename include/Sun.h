#ifndef SUN_H_
#define SUN_H_

class Sun::public Planet
{
    public:
            Sun();
            ~Sun();

            //Sun animate function
            void animate();

    private:
            illumination[3];
};

#endif
