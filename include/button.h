#ifndef classbutton
#define classbutton

#include <cstdint>

class button
{
private:
    /* data */
    bool bSim = false;
public:
    bool bPressedBlue = false;
    bool bPressedRed = false;

public:
    button(/* args */){}
    ~button(){}

    void setup( bool bSimulation );
    void loop( void );

    void getButton( void );
};

#endif