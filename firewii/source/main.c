
/****************************************************************************
  Fire animation demo for Wii.
  Written by Jespa in 2009
  Based on Asphyxia VGA tutorial #19, written by Denthor a long long time ago.
  Version 1.0 (a.k.a. Just works!). Can be heavily optimized.
****************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <gccore.h>
#include <grrlib.h>
#include <wiiuse/wpad.h>


Mtx GXmodelView2D;

//This vector stores the RGBA color used to draw the fire.
u32 fireColors[256];

//fireMatrix keep the current state of simulation. Each dot in the matrix stores the temperature of air
//using a range of 200 values
#define fireMatrix_W 320
#define fireMatrix_H 235

//Declare the matrix and add two extra rows for padding
u32 fireMatrix2[(fireMatrix_H+2)*fireMatrix_W];
//fireMatrix is a pointer to beginning of usefull data
u32 *fireMatrix = fireMatrix2 + 2*fireMatrix_W;

//Temperature decreasing factor on each simulation step
int fireCombustion = 3;

//Flag to indicate a temporally extra hot fire.
int extraFire=0;

//Draw fireMatrix using the colors given in fireColors
//The matrix will be scaled so each point plot 4 pixels in the screen.
void fireMatrix_Draw(int sx, int sy)
{
    int x, y;
    sy += 2*fireMatrix_H;
    u32 *dot = fireMatrix;
    for (y=0; y< fireMatrix_H; y++)
    {
        for (x=0; x< fireMatrix_W; x++)
        {
            u32 color = fireColors[*dot];

            //Plot a pixel and the pixel above of it
            GRRLIB_Plot(sx+x,sy-2*y-1,color);
            GRRLIB_Plot(sx+x,sy-2*y,color);

            //Instead of plot aside the previous pixels, we plot in the
	    //other half of screen, so we dont see huge 4x4 pixel blocks
            GRRLIB_Plot(sx+fireMatrix_W+x,sy-2*y-1,color);
            GRRLIB_Plot(sx+fireMatrix_W+x,sy-2*y,color);

            //next dot in the matrix
            dot++;

        }//for
    }//for

}//void fireMatrix_Draw(int sx, int sy)


//Makes one step of fire animation.
void fireMatrix_Anim()
{
    int x, y;
    register int acc;

    //Botton line is build using random noise. If extraFire is set, adds extra hot to each dot.
    for (x=0; x< fireMatrix_W; x++)
    {
        if (extraFire > 0)
            fireMatrix[-x] += rand() & 255;
        else
            fireMatrix[-x] = rand() & 255;
    }

    //Rest of matrix is build from interpolation. Each dot temperature is averaged
    //with its three inferior neighbours and a little noise is added.
    //If extraFire is set, adds extra hot to each dot.
    u32 *dot = fireMatrix;
    u32 *dot_10 = dot - fireMatrix_W-1;
    u32 *dot_11 = dot - fireMatrix_W;
    u32 *dot_12 = dot - fireMatrix_W+1;
    for (y=0; y<fireMatrix_H;y++)
    {
        for (x=0; x< fireMatrix_W; x++)
        {
            //Adds the value of this dot, plus 3 neighbours, plus extra noise.
            acc = *dot + *dot_10 + *dot_11 + *dot_12 +
                    (rand() & ((extraFire > 0)?31:15));

            //Divide the result by 4, to compute a valid average
            acc >>= 2;

            //Decrease the value due to combustion
            if (acc > fireCombustion)
                acc -= fireCombustion;
            else if (acc > 0)
                acc--;

            //Check for overflows due to noise.
            if (acc > 199)
                acc = 199;

            //Save this temperature
            *dot = acc;

            //points to next dot
            dot++;
            dot_10++;
            dot_11++;
            dot_12++;

        }//for

    }//for

    //Decrease the value of extraFire.
    if (extraFire > 0)
        extraFire--;

}//void fireMatrix_Anim()


//Create the RGBA colors used to draw the fireMatrix.
void fireColors_Init()
{

    int conta=0;
    int i=0;
    register u32 color;

    //First section. From (0,0,0) to (63,33,0) in 126 steps
    for (i=0; i<126;i++)
    {
        if ((i & 1) && (conta < 63) )
            conta++;

        color = 0x000000FF;                          //A
        color |= conta << 24;                        //R
        color |= ((conta>30)?(conta-30):0)  << 16;   //G
        color |= ((conta>8)?((63-conta)>>2):0) << 8; //B
        fireColors[i] = color;
    } //for

    //Second section. From (63,33,0) to (126,64,63)
    conta=0;
    for (i=126; i<255;i++)
    {
        if ((i & 1) && (conta < 63) )
            conta++;

        color = 0x000000FF;             //A
        color |= (63+conta) << 24;      //R
        color |= (33+conta/2) << 16;    //G
        color |= conta << 8;            //B
        fireColors[i] = color;
    }//for

} //fireColors_Init


//Draw the fireColors values. This is only for testing and debug.
void fireColors_Draw(int sx, int sy, int width)
{
    int c;    
    for (c=0; c<255; c++)
        GRRLIB_Line(sx,sy+c,sx+width, sy+c, fireColors[c]);
}//void fireColors_Draw(int sx, int sy)


int x= 50;
int y = 100;

//The one and only... main function!!!
int main() {

    u32 wpaddown;
    u32 timeout=0;

    //Init libraries
    GRRLIB_Init();
    WPAD_Init();

    //Init the fireMatrix and fireColors structures
    fireColors_Init();
    memset(fireMatrix, 0, sizeof(fireMatrix) );

    //Clear the screen
    GRRLIB_FillScreen(0x000000FF);

    //Animation loop. It comes with two different flavours
    while(timeout++ < 400) { //Exit after a given number of iteration.
    //while(1) {                  //Run until user press home button in the pad.

        //Read state of pad 0
        WPAD_ScanPads();
        wpaddown = WPAD_ButtonsDown(0);

        //Animate and draw the fireMatrix
        fireMatrix_Anim();
        fireMatrix_Draw((640-2*fireMatrix_W)>>1, (480-2*fireMatrix_H)>>1);

        //Draw the fireColors values. This is only for testing and debug.
        //fireColors_Draw(1, 100, 50);

        //Ask GRRLIB to show show changes on screen
        GRRLIB_Render();

        //Miramos si el mando está siendo agitado

        //Check for button activity in wpad
        if (wpaddown)
        {
            //Any button activity will restart the count to timeout
            timeout=0;

            //HOME button will exit from the animation loop
            if(wpaddown & WPAD_BUTTON_HOME)
                break;

            //UP button will decrease the fireCombustion factor
            if(wpaddown & WPAD_BUTTON_UP)
                if (fireCombustion>1)
                    fireCombustion--;

            //DOWN button will increase the fireCombustion factor.
            if(wpaddown & WPAD_BUTTON_DOWN)
                fireCombustion++;

            //LEFT button will reset the fireCombustion factor
            if(wpaddown & WPAD_BUTTON_LEFT)
                fireCombustion=3;

            //A button will add extraFire for a time period
            if(wpaddown & WPAD_BUTTON_A)
                extraFire+=7;

        }//if (wpaddown)

    }//Animation loop end.

    // Be a good boy, clear the memory allocated by GRRLIB
    GRRLIB_Exit();
    exit(0);  

}// int main()

