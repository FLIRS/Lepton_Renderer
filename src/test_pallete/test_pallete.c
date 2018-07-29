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


#define APP_TEX_W          256
#define APP_TEX_H          1
#define APP_TEX_WH         (APP_TEX_W * APP_TEX_H)
#define APP_TEX_TYPE       GL_UNSIGNED_SHORT
#define APP_TEX_FORMAT     GL_ALPHA
#define APP_TEX_UNIT       0
#define APP_TEX_MAG_FILTER GL_NEAREST
//#define APP_TEX_MAG_FILTER GL_LINEAR


#define APP_PAL_W          256
#define APP_PAL_H          1
#define APP_PAL_C          4
#define APP_PAL_WHC        (APP_PAL_W * APP_PAL_H * APP_PAL_C)
#define APP_PAL_TYPE       GL_UNSIGNED_BYTE
#define APP_PAL_FORMAT     GL_RGBA
#define APP_PAL_UNIT       1
#define APP_PAL_MAG_FILTER GL_NEAREST
//#define APP_PAL_MAG_FILTER GL_LINEAR



void read_image (uint16_t * pixmap, int offset)
{
	for (size_t i = 0; i < APP_TEX_WH; i = i + 1)
	{
		pixmap [i] = 256 * (i + offset);
	}
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
	//config.win_flags = SDL_WINDOW_OPENGL;
	config.win_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN;
	config.shader_name_fragment = "src/common/cam2d.glfs";
	config.shader_name_vertex = "src/common/cam2d.glvs";
	
	struct Application app;
	app_init (&app, &config);
	
	uint16_t raw [APP_TEX_WH];
	uint8_t heatmap [APP_PAL_WHC];
	size_t heatmap_count = APP_PAL_WHC;
	{
		int r = pix_load (heatmap, &heatmap_count, "src/common/heatmap.txt", sizeof (uint8_t));
		ASSERT (r == 0);
	}
	
	//Generate texture id for Lepton and Pallete.
	GLuint tex [2];
	glGenTextures (2, tex);
	
	//Setup Lepton texture format.
	glActiveTexture (GL_TEXTURE0 + APP_TEX_UNIT);
	glBindTexture (GL_TEXTURE_2D, tex [APP_TEX_UNIT]);
	glUniform1i_name (app.program, "tex", APP_TEX_UNIT);
	glTexImage2D (GL_TEXTURE_2D, 0, APP_TEX_FORMAT, APP_TEX_W, APP_TEX_H, 0, APP_TEX_FORMAT, APP_TEX_TYPE, raw);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, APP_TEX_MAG_FILTER);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap (GL_TEXTURE_2D);

	//Setup Pallete texture format.
	glActiveTexture (GL_TEXTURE0 + APP_PAL_UNIT);
	glBindTexture (GL_TEXTURE_2D, tex [APP_PAL_UNIT]);
	glUniform1i_name (app.program, "pallete", APP_PAL_UNIT);
	glTexImage2D (GL_TEXTURE_2D, 0, APP_PAL_FORMAT, APP_PAL_W, APP_PAL_H, 0, APP_PAL_FORMAT, APP_PAL_TYPE, heatmap);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, APP_PAL_MAG_FILTER);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap (GL_TEXTURE_2D);
	

	int iii = 0;
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
		
		
		read_image (raw, iii);
		iii++;
		glBindTexture(GL_TEXTURE_2D, tex [0]);
		glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, APP_TEX_W, APP_TEX_H, APP_TEX_FORMAT, APP_TEX_TYPE, raw);
		//TODO: Find out why we need to bind the pallete texture.
		glBindTexture (GL_TEXTURE_2D, tex [1]);
		app_draw (&app);

		
	}
	
main_quit:


	app_quit (&app);
	return 0;
}
