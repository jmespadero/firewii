/*===========================================
        GRRLIB 4.0.0
        Code     : NoNameNo
        Additional Code : Crayon
        GX hints : RedShade

        How To use Display an image file

============================================
Download and Help Forum : http://grrlib.santo.fr
============================================*/
#include "../../../GRRLIB/GRRLIB/GRRLIB.h"

#include <stdlib.h>
#include <wiiuse/wpad.h>

#include "gfx/pirate.h"


Mtx GXmodelView2D;


int main() {
    u32 wpaddown;

    // Load the image file to a texture structure
    GRRLIB_texImg tex_pirate = GRRLIB_LoadTexture(pirate);


    GRRLIB_Init();
    WPAD_Init();

int dx=1;
int x= 0;
int y = 100;

    while(1) {
        WPAD_ScanPads();
        wpaddown = WPAD_ButtonsDown(0);

        //We fill the screen in white(RGBA format)
        GRRLIB_FillScreen(0xFFFFFFFF);

		/*
		//El coche fantastico!!!
		x += dx;
		GRRLIB_Plot(x,y,0x000000FF);
		GRRLIB_Plot(x,y+1,0x000000FF);
		GRRLIB_Plot(x+1,y,0x000000FF);
		GRRLIB_Plot(x+1,y+1,0x000000FF);
		if (x <= 0) dx = 1;
		if (x >= 200) dx = -1;
		//*/
		
        //Display the image
        GRRLIB_DrawImg(150, 150, tex_pirate, 0, 1, 1, 0xFFFFFFFF);

        GRRLIB_Render();
        if(wpaddown & WPAD_BUTTON_HOME) {
            exit(0);
        }
    }

    // Be a good boy, clear the memory allocated by GRRLIB
    GRRLIB_Exit(); 
    free(tex_pirate.data);
    return 0;
}
