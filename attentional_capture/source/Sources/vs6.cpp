// vs6_framework: a cross-platform general purpose psychophysical experiment creation tool
// by: louis
// on:19jan2010
// last modify: 19jan2010
// version: 6.0
// dependency: sdl(vs6), sdl-image, cairo and pango

#pragma warning(disable:4996) //suppress depreceted warning
#pragma warning(disable:4244) //suppress type conversion warning

#include "vs6.h"
#include "vs6_functions.h"
#include "vs6_exptcfg.h"

int main(int argc, char* argv[])
{
	//specific to win32: set directx as the default
	#ifdef _WIN32  
	putenv("SDL_VIDEODRIVER=directx");
	#endif

	//initialize random number
	init_genrand((unsigned long)time(0));

	//init display
	vs::initdisplay();

	//run experiment body
	exptmain();
	vs::showbuf("cls",2);

	//clear buffers
	vs::freeallbuf();
	vs::freeallimg();

	//close display
	vs::closedisplay();

	return 0;
}

