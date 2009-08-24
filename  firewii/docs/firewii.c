#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <ogc/tpl.h>

static u32 *xfb = NULL;
static GXRModeObj *rmode = NULL;

/*
GXColor fireColors[256];

*/
int scanSize=320;
#define putPixel(x,y,color) (xfb[scanSize*y+x]=color)
int y = 60;
int x = 30;

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
//---------------------------------------------------------------------------------

	int i;

	// Initialise the video system
	VIDEO_Init();
	
	// This function initialises the attached controllers
	WPAD_Init();
	
	// Obtain the preferred video mode from the system
	// This will correspond to the settings in the Wii menu
	rmode = VIDEO_GetPreferredMode(NULL);
	
	//Render mode configuration for 524 lines,interlaced,antialiased PAL mode
	//rmode = &TVPal524IntAa;

	// Allocate memory for the display in the uncached region
	xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
	
	// Initialise the console, required for printf
	console_init(xfb,20,20,rmode->fbWidth,rmode->xfbHeight,rmode->fbWidth*VI_DISPLAY_PIX_SZ);
	//Jespa: Normalmente fbWidth=640, xfbHeight=480, VI_DISPLAY_PIX_SZ=2
	
	// Set up the video registers with the chosen mode
	VIDEO_Configure(rmode);
	
	// Tell the video hardware where our display memory is
	VIDEO_SetNextFramebuffer(xfb);
	
	// Make the display visible
	VIDEO_SetBlack(FALSE);

	// Flush the video register changes to the hardware
	VIDEO_Flush();

	// Wait for Video setup to complete
	VIDEO_WaitVSync();
	if(rmode->viTVMode&VI_NON_INTERLACE) VIDEO_WaitVSync();

	

	printf("\x1b[2;0H");//Put cursor at 0,2 position
    printf("W=%d   H=%d   VI_DISPLAY_PIX_SZ=%d",rmode->fbWidth,rmode->xfbHeight,VI_DISPLAY_PIX_SZ);

	scanSize=rmode->fbWidth/VI_DISPLAY_PIX_SZ;

int dx=1;

    //Bucle principal de la animacion
	while(1) {

		// Wait for the next frame
		VIDEO_WaitVSync();
/*		
		for (i=0; i< 33; i++)
		{
			//Pinta un punto
			putPixel(x+i,y,COLOR_WHITE);
		}
		for (i=32; i< 66; i+=2)
		{
			//Pinta un punto
			putPixel(x+i,y,COLOR_YELLOW);
			putPixel(x+i,y+24,COLOR_RED);
		}
		for (i=66; i< 100; i+=3)
		{
			//Pinta un punto
			putPixel(x+i,y,COLOR_WHITE);
		}
		
		//el coche fantástico
		putPixel(x,y,COLOR_BLACK);
		if (x<20) dx=1;
		if (x>200) dx=-1;
		x+=dx;
		putPixel(x,y,COLOR_RED);
*/
		
		
		// Call WPAD_ScanPads each loop, this reads the latest controller states
		WPAD_ScanPads();

		// WPAD_ButtonsDown tells us which buttons were pressed in this loop
		// this is a "one shot" state which will not fire again until the button has been released
		u32 pressed = WPAD_ButtonsDown(0);

		// We return to the launcher application via exit
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);


		if ( pressed & WPAD_BUTTON_UP )
		{
			for (i=0; i< 100; i++) putPixel(x+i,y,COLOR_BLACK);
			y-=1;
		}

		if ( pressed & WPAD_BUTTON_DOWN )
		{
			for (i=0; i< 100; i++) putPixel(x+i,y,COLOR_BLACK);
			y+=1;
		}
		
		if ( pressed & WPAD_BUTTON_LEFT )
		{
			for (i=0; i< 100; i++) putPixel(x+i,y,COLOR_BLACK);
			x-=1;
		}

		if ( pressed & WPAD_BUTTON_RIGHT )
		{
			for (i=0; i< 100; i++) putPixel(x+i,y,COLOR_BLACK);
			x+=1;
		}

	}

	//Libera memoria
	free(fireMatrix);
	
	return 0;
}
