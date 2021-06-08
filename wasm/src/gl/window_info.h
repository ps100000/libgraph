#ifndef _WINDOW_INFO_STRUCT_H_
#define _WINDOW_INFO_STRUCT_H_

#include <SDL.h>

struct window_info{
	SDL_Window* window;
	int w;
	int h;
};

#endif