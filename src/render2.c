#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>

#include <SDL2/SDL_opengles2.h>

#include <stdio.h>
#include <sys/stat.h>
//getopt()
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "common_gl.h"
#include "vertex.h"
#include "pixel.h"
#include "app.h"
#include "texture.h"


#define APP_TEX_W 160
#define APP_TEX_H 120
#define APP_TEX_WH (APP_TEX_W * APP_TEX_H)



void read_image (uint16_t * pixmap)
{
	int const size = APP_TEX_WH * sizeof (uint16_t);
	int r = read (STDIN_FILENO, pixmap, size);
	ASSERT_F (r == size, "read () error. Read %d of %d", r, size);
	
	//printf ("r %i\n", pixmap [0]);
}



int main(int argc, char *argv[])
{
	while (1)
	{
		int C = getopt (argc, argv, "");
		if (C == - 1) {break;}
		switch (C)
		{
			default:
			break;
		}
	}
	
	struct Config config;
	config.app_name = "Renderer";
	config.win_x = SDL_WINDOWPOS_UNDEFINED;
	config.win_y = SDL_WINDOWPOS_UNDEFINED;
	config.win_w = 1920;
	config.win_h = 1080;
	config.win_flags = SDL_WINDOW_OPENGL;
	//config.win_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	config.shader_name_fragment = "src/cam2d.glfs";
	config.shader_name_vertex = "src/cam2d.glvs";
	
	struct Application app;
	app_init (&app, &config);
	
	
	//struct Pixel_ABGR8888 image [APP_TEX_W * APP_TEX_H];
	uint16_t raw [APP_TEX_WH];
	for (size_t i = 0; i < APP_TEX_WH; i = i + 1)
	{
		raw [i] = i;
	}
	struct Texture tex;
	tex.data = raw;
	tex.unit = 0;
	tex.name = "tex";
	tex.w = APP_TEX_W;
	tex.h = APP_TEX_H;
	tex.format = GL_ALPHA;
	tex.type = GL_UNSIGNED_SHORT;
	tex_create (&tex, app.program);
	



	
	
	while (1)
	{
		SDL_Event event;
		while (SDL_PollEvent (&event))
		{
			switch (event.type)
			{
				case SDL_QUIT:
				goto main_quit;
				
				case SDL_WINDOWEVENT:
				switch (event.window.event)
				{
					case SDL_WINDOWEVENT_CLOSE:
					printf ("SDL_WINDOWEVENT_CLOSE");
					break;
				}
				break;
				
				case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
					case SDLK_ESCAPE:
					event.type = SDL_QUIT;
					SDL_PushEvent (&event);
					break;
				}
				break;
			}
		}
		
		for (size_t i = 0; i < COUNTOF (raw); i = i + 1)
		{
			//raw [i] = raw [i] * 3;
			//printf ("%i ", raw [i]);
		}
		
		read_image (raw);
		
		{
			uint16_t min = UINT16_MAX;
			uint16_t max = 0;
			find_range_u16v (raw, APP_TEX_WH, &min, &max);
			map_lin_u16v (raw, raw, APP_TEX_WH, min, max, 0, UINT16_MAX - 1);
		}
		tex_update (&tex);
		app_draw (&app);

	}
	
main_quit:

	glDeleteTextures (1, &(tex.id));
	app_quit (&app);
	return 0;
}
