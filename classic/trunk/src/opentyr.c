#include "opentyr.h"
#include "vga256d.h"

#include "SDL.h"
#include <stdio.h>


int main( int argc, char *argv[] )
{
    SDL_Color col;

	/* TODO: DetectCFG */
	/* TODO: scanforepisodes */

	printf("\nWelcome to... >> OpenTyrian v0.1 <<\n\n");

	/* TODO: InitKeyboard */

	/* TODO: Not sure what these are about?
	 * recordfilenum = 1;
	 * playdemonum = 0; 
	 * playdemo = false; */

	/* TODO: LoadConfiguration */

	/* TODO: Tyrian originally checked availible memory here. */

	/* here ends line 92771 of TYRIAN2.PAS
	 * TODO: Finish it and stuff. */

    SDL_Init( SDL_INIT_VIDEO );

    JE_initvga256();

    col.r = 0xFF;
    col.g = 0xFF;
    col.b = 0x0;
    
    SDL_SetColors(VGAScreenSeg, &col, 1, 1);

    JE_rectangle(30, 30, 320-30, 200-30, 1);

    JE_ShowVGARetrace();

    SDL_SaveBMP(VGAScreenSeg, "sshot.bmp");
    SDL_Delay(3000);

    JE_closevga256();

    SDL_Quit();
	
	return 0;
}
