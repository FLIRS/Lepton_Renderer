#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <stdio.h>
#include <sys/stat.h>
//getopt()
#include <unistd.h>

#include "debug.h"
#include "common.h"
#include "xgl.h"
#include "convert.h"
#include "map.h"

#define APP_TEX_COUNT 1

#define APP_SHARED_W 160
#define APP_SHARED_H 120
#define APP_SHARED_WH (APP_SHARED_W * APP_SHARED_H)

#define APP_RENDER_C          3
#define APP_RENDER_TYPE       GL_UNSIGNED_BYTE
#define APP_RENDER_FORMAT     GL_RGB
#define APP_RENDER_INTFORMAT  GL_RGB
#define APP_RENDER_UNIT       0
#define APP_RENDER_MAG_FILTER GL_NEAREST
#define APP_RENDER_MAG_FILTER GL_LINEAR


#define APP_WIN_X SDL_WINDOWPOS_UNDEFINED
#define APP_WIN_Y SDL_WINDOWPOS_UNDEFINED
#define APP_WIN_W 1920
#define APP_WIN_H 1080
#define APP_WIN_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
#define APP_WIN_NAME "Test background filter"
#define APP_SHADERF "src/rgb.glfs"
#define APP_SHADERV "src/rgb.glvs"

#define APP_SHOW_INPUT 0
#define APP_SHOW_BG 1
#define APP_SHOW_FG 2
#define APP_SHOW_BGFG 3


void bgfilter (uint16_t src [], uint16_t bg [], uint16_t fg [], uint32_t n)
{
	float k = 0.02f;
	while (n--)
	{
		bg [n] = k*(float)src [n] + (1.0f-k)*(float)bg [n];
		int32_t d = (int32_t)bg [n] - (int32_t)src [n];
		fg [n] = abs (d);
	}
}


void scale (uint16_t src [], uint16_t des [], uint32_t n)
{
	uint16_t min = UINT16_MAX;
	uint16_t max = 0;
	find_range_u16v (src, n, &min, &max);
	while (n--)
	{
		des [n] = (uint16_t)map_lin_int ((int)src [n], (int)min, (int)max, 0, UINT16_MAX - 1);
	}
}


int main(int argc, char *argv[])
{
	while (1)
	{
		int c = getopt (argc, argv, "");
		if (c == - 1) {break;}
		switch (c)
		{
			default:
			break;
		}
	}
	
	{int r = SDL_Init (SDL_INIT_VIDEO); ASSERT_F (r == 0, "SDL_Init: %s", SDL_GetError());}
	
	SDL_Window * window;
	SDL_GLContext context;
	
	window = SDL_CreateWindow (APP_WIN_NAME, APP_WIN_X, APP_WIN_Y, APP_WIN_W, APP_WIN_H, APP_WIN_FLAGS);
	ASSERT_F (window, "SDL_CreateWindow: %s", SDL_GetError());
	
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);
	//SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_ACCELERATED_VISUAL, 1);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	context = SDL_GL_CreateContext (window);
	ASSERT_F (context, "SDL_GL_CreateContext: %s", SDL_GetError());
	SDL_GL_SetSwapInterval (1);// Use VSYNC

	printf ("%s\n", glGetString(GL_VERSION) );

	GLuint vbo;
	GLuint ebo;
	GLuint program;
	
	program = glCreateProgram ();XGL_ASSERT_ERROR;
	xgl_attach_shaderfile (program, APP_SHADERF, GL_FRAGMENT_SHADER);
	xgl_attach_shaderfile (program, APP_SHADERV, GL_VERTEX_SHADER);
	glLinkProgram (program);XGL_ASSERT_ERROR;
	glUseProgram (program);XGL_ASSERT_ERROR;
	xgl_program_print (program);
	
	float vertices [] = 
	{   //x      y     s      t
		-1.0f, -1.0f, 0.0f,  1.0f, // BL
		-1.0f,  1.0f, 0.0f,  0.0f, // TL
		 1.0f,  1.0f, 1.0f,  0.0f, // TR
		 1.0f, -1.0f, 1.0f,  1.0f  // BR
	};
	const GLint indicies [] = {0, 1, 2, 0, 2, 3};
	
	glGenBuffers (1, &(vbo));XGL_ASSERT_ERROR;
	glBindBuffer (GL_ARRAY_BUFFER, vbo);XGL_ASSERT_ERROR;
	glBufferData (GL_ARRAY_BUFFER, sizeof (vertices), vertices, GL_STATIC_DRAW);XGL_ASSERT_ERROR;
	glGenBuffers (1, &(ebo));XGL_ASSERT_ERROR;
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, ebo);XGL_ASSERT_ERROR;
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indicies), indicies, GL_STATIC_DRAW);XGL_ASSERT_ERROR;
	{
		GLint pos_attr_loc = glGetAttribLocation (program, "in_Position");
		ASSERT (pos_attr_loc >= 0);
		printf ("pos_attr_loc %i \n", pos_attr_loc);
		glVertexAttribPointer ((GLuint)pos_attr_loc, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray ((GLuint)pos_attr_loc);
		GLint tex_attr_loc = glGetAttribLocation (program, "in_Texcoord");
		ASSERT (tex_attr_loc >= 0);
		printf ("tex_attr_loc %i \n", tex_attr_loc);
		glVertexAttribPointer ((GLuint)tex_attr_loc, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2*sizeof(GLfloat)));
		glEnableVertexAttribArray ((GLuint)tex_attr_loc);
	}
	glUseProgram (program);XGL_ASSERT_ERROR;
	glEnable (GL_BLEND);XGL_ASSERT_ERROR;
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);XGL_ASSERT_ERROR;
	
	uint16_t img_input [APP_SHARED_WH] = {0};
	uint16_t img_bg [APP_SHARED_WH] = {0};
	uint16_t img_fg [APP_SHARED_WH] = {0};
	uint16_t img_render0 [APP_SHARED_WH] = {0};
	uint8_t img_render [APP_SHARED_WH * APP_RENDER_C] = {0};
	
	//Generate texture id for Lepton and Pallete.
	GLuint tex [APP_TEX_COUNT];
	glGenTextures (APP_TEX_COUNT, tex);XGL_ASSERT_ERROR;
	
	
	
	//Setup Lepton texture format.
	glActiveTexture (GL_TEXTURE0 + APP_RENDER_UNIT);XGL_ASSERT_ERROR;
	glBindTexture (GL_TEXTURE_2D, tex [APP_RENDER_UNIT]);XGL_ASSERT_ERROR;
	xgl_uniform1i_set (program, "tex", APP_RENDER_UNIT);
	glTexImage2D (GL_TEXTURE_2D, 0, APP_RENDER_INTFORMAT, APP_SHARED_W, APP_SHARED_H, 0, APP_RENDER_FORMAT, APP_RENDER_TYPE, img_render);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, APP_RENDER_MAG_FILTER);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); XGL_ASSERT_ERROR;
	glGenerateMipmap (GL_TEXTURE_2D);XGL_ASSERT_ERROR;
	int show = APP_SHOW_FG;
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
				default:
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
				case SDLK_1:
					show = APP_SHOW_INPUT;
					break;
				case SDLK_2:
					show = APP_SHOW_BG;
					break;
				case SDLK_3:
					show = APP_SHOW_FG;
					break;
				case SDLK_4:
					show = APP_SHOW_BGFG;
					break;
				default:
					break;
				}
				break;
				
			default:
				break;
			}
		}
		
		

		{int r = read (STDIN_FILENO, img_input, sizeof (img_input)); ASSERT_F (r == sizeof (img_input), "read () error. Read %d of %d", r, sizeof (img_input));}

		switch (show)
		{
		case APP_SHOW_FG:
			bgfilter (img_input, img_bg, img_fg, APP_SHARED_WH);
			scale (img_fg, img_render0, APP_SHARED_WH);
			convert_u16c1_u8c3 (img_render0, img_render, APP_SHARED_WH);
			break;
		case APP_SHOW_BG:
			bgfilter (img_input, img_bg, img_fg, APP_SHARED_WH);
			scale (img_bg, img_render0, APP_SHARED_WH);
			convert_u16c1_u8c3 (img_render0, img_render, APP_SHARED_WH);
			break;
		case APP_SHOW_BGFG:
			bgfilter (img_input, img_bg, img_fg, APP_SHARED_WH);
			scale (img_bg, img_render0, APP_SHARED_WH);
			convert_u16c1_u8c3_c (img_render0, img_render, APP_SHARED_WH, 0);
			scale (img_fg, img_render0, APP_SHARED_WH);
			convert_u16c1_u8c3_c (img_render0, img_render, APP_SHARED_WH, 1);
			scale (img_input, img_render0, APP_SHARED_WH);
			convert_u16c1_u8c3_c (img_render0, img_render, APP_SHARED_WH, 2);
			break;
		default:
			scale (img_input, img_render0, APP_SHARED_WH);
			convert_u16c1_u8c3 (img_render0, img_render, APP_SHARED_WH);
			break;
		}
		
		//Set all corner pixel red.
		img_render [0] = 255;
		img_render [(APP_SHARED_W-1)*APP_RENDER_C] = 255;
		img_render [(APP_SHARED_WH-1)*APP_RENDER_C] = 255;
		img_render [(APP_SHARED_WH-APP_SHARED_W)*APP_RENDER_C] = 255;
		
		glBindTexture(GL_TEXTURE_2D, tex [0]);XGL_ASSERT_ERROR;
		glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, APP_SHARED_W, APP_SHARED_H, APP_RENDER_FORMAT, APP_RENDER_TYPE, img_render);XGL_ASSERT_ERROR;
		glClear (GL_COLOR_BUFFER_BIT);XGL_ASSERT_ERROR;
		glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);XGL_ASSERT_ERROR;
		SDL_GL_SwapWindow (window);
	}
	
main_quit:
	glUseProgram (0);
	glDisableVertexAttribArray (0);
	glDeleteProgram (program);
	glDeleteBuffers (1, &(ebo));
	glDeleteBuffers (1, &(vbo));
	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	SDL_Quit();
	return 0;
}
