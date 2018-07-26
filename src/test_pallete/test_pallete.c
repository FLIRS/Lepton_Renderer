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
#define APP_TEX_H 420
#define APP_TEX_WH (APP_TEX_W * APP_TEX_H)




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
	//config.win_flags = SDL_WINDOW_OPENGL;
	config.win_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	config.shader_name_fragment = "src/common/cam2d.glfs";
	config.shader_name_vertex = "src/common/cam2d.glvs";
	
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
	
	
	
	uint8_t heatmap [256 * 4];
	size_t heatmap_count = COUNTOF (heatmap);
	{
		int r = pix_load (heatmap, &heatmap_count, "src/common/heatmap.txt", sizeof (uint8_t));
		ASSERT (r == 0);
	}
	
	TRACE_F ("%i", heatmap_count);
	
	struct Pallete pal;
	pal.data = heatmap;
	pal.unit = 1;
	pal.name = "pallete";
	pal.w = 256;
	pal.format = GL_RGBA;
	pal.type = GL_UNSIGNED_BYTE;
	pallete_create (&pal, app.program);

	


	
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
		
		tex_update (&tex);
		pallete_update (&pal);
		app_draw (&app);

		
	}
	
main_quit:

	glDeleteTextures (1, &(tex.id));
	app_quit (&app);
	return 0;
}
