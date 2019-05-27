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
#include "pixel.h"


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

#define APP_WIN_X SDL_WINDOWPOS_UNDEFINED
#define APP_WIN_Y SDL_WINDOWPOS_UNDEFINED
#define APP_WIN_W 1920
#define APP_WIN_H 1080
#define APP_WIN_FLAGS SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN
#define APP_WIN_NAME "Test Pallete"
#define APP_GL_SHADERF "src/cam2d.glfs"
#define APP_GL_SHADERV "src/cam2d.glvs"

void read_image (uint16_t * pixmap, uint16_t offset)
{
	for (int i = 0; i < APP_TEX_WH; i++)
	{
		int v = 256 * (i + offset);
		pixmap [i] = (uint16_t)v;
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

	GLuint vbo;
	GLuint ebo;
	GLuint program;
	
	program = glCreateProgram ();XGL_ASSERT_ERROR;
	xgl_attach_shaderfile (program, APP_GL_SHADERF, GL_FRAGMENT_SHADER);
	xgl_attach_shaderfile (program, APP_GL_SHADERV, GL_VERTEX_SHADER);
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
	
	uint16_t raw [APP_TEX_WH];
	uint8_t heatmap [APP_PAL_WHC];
	size_t heatmap_count = APP_PAL_WHC;
	{
		int r = pix_load (heatmap, &heatmap_count, "src/heatmap.txt", sizeof (uint8_t));
		ASSERT (r == 0);
	}
	
	//Generate texture id for Lepton and Pallete.
	GLuint tex [2];XGL_ASSERT_ERROR;
	glGenTextures (2, tex);XGL_ASSERT_ERROR;
	
	//Setup Lepton texture format.
	glActiveTexture (GL_TEXTURE0 + APP_TEX_UNIT);XGL_ASSERT_ERROR;
	glBindTexture (GL_TEXTURE_2D, tex [APP_TEX_UNIT]);XGL_ASSERT_ERROR;
	xgl_uniform1i_set (program, "tex", APP_TEX_UNIT);
	glTexImage2D (GL_TEXTURE_2D, 0, APP_TEX_FORMAT, APP_TEX_W, APP_TEX_H, 0, APP_TEX_FORMAT, APP_TEX_TYPE, raw);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, APP_TEX_MAG_FILTER);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); XGL_ASSERT_ERROR;
	glGenerateMipmap (GL_TEXTURE_2D);XGL_ASSERT_ERROR;

	//Setup Pallete texture format.
	glActiveTexture (GL_TEXTURE0 + APP_PAL_UNIT);XGL_ASSERT_ERROR;
	glBindTexture (GL_TEXTURE_2D, tex [APP_PAL_UNIT]);XGL_ASSERT_ERROR;
	xgl_uniform1i_set (program, "pallete", APP_PAL_UNIT);
	glTexImage2D (GL_TEXTURE_2D, 0, APP_PAL_FORMAT, APP_PAL_W, APP_PAL_H, 0, APP_PAL_FORMAT, APP_PAL_TYPE, heatmap);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, APP_PAL_MAG_FILTER);XGL_ASSERT_ERROR;
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);XGL_ASSERT_ERROR;
	glGenerateMipmap (GL_TEXTURE_2D);XGL_ASSERT_ERROR;
	

	uint16_t iii = 0;
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
					default:
					break;
				}
				break;
				
				default:
				break;
			}
		}
		
		
		read_image (raw, iii);
		iii++;
		glBindTexture(GL_TEXTURE_2D, tex [0]);XGL_ASSERT_ERROR;
		glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, APP_TEX_W, APP_TEX_H, APP_TEX_FORMAT, APP_TEX_TYPE, raw);XGL_ASSERT_ERROR;
		//TODO: Find out why we need to bind the pallete texture.
		glBindTexture (GL_TEXTURE_2D, tex [1]);XGL_ASSERT_ERROR;
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
