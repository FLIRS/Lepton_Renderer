#pragma once

#include "vertex.h"
#include "shader.h"
#include "common_gl.h"

/*
#define APP_SHORT_NAME "Short app name"
#define APP_WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_HEIGHT 1080
#define APP_WINDOW_WIDTH 1920
#define APP_WINDOW_ASPECT (APP_WINDOW_WIDTH / APP_WINDOW_HEIGHT)
#define APP_WINDOW_FOV (45.0f * (M_PI / 180.0f))

#define APP_TEX_W 100
#define APP_TEX_H 100
#define APP_TEX_FORMAT GL_RGBA
#define APP_TEX_SHADER_NAME "tex"
*/

#define APP_TEX_UNIT 0


struct Application
{
	SDL_Window * window;
	SDL_GLContext context;
	GLuint vbo;
	GLuint ebo;
	GLuint program;
};


struct Config
{
	char const * app_name;
	char const * shader_name_vertex;
	char const * shader_name_fragment;
	int win_x;
	int win_y;
	int win_w;
	int win_h;
	uint32_t win_flags;
};


void app_init (struct Application * app, struct Config * config)
{
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init: %s", SDL_GetError());
	}
	
	
	app->window = SDL_CreateWindow (config->app_name, config->win_x, config->win_y, config->win_w, config->win_h, config->win_flags);
	ASSERT_F (app->window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	
	//const Uint8 * keyboard;
	//keyboard = SDL_GetKeyboardState (NULL);
	
	//Initialize rendering context
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	app->context = SDL_GL_CreateContext (app->window);
	ASSERT_F (app->context != NULL, "SDL_GL_CreateContext: %s", SDL_GetError());
	
	//Use VSYNC
	SDL_GL_SetSwapInterval (1);
	
	
	struct Shader shaders [2] = 
	{
		{0, GL_VERTEX_SHADER, config->shader_name_vertex},
		{0, GL_FRAGMENT_SHADER, config->shader_name_fragment}
	};
	
	struct Vertex verts [4] = 
	{
		//   x      y     s      t
		{ -1.0f, -1.0f, 0.0f,  1.0f }, // BL
		{ -1.0f,  1.0f, 0.0f,  0.0f }, // TL
		{  1.0f,  1.0f, 1.0f,  0.0f }, // TR
		{  1.0f, -1.0f, 1.0f,  1.0f } // BR
	};

	const GLint indicies[] = 
	{
		0, 1, 2, 0, 2, 3
	};
	
	app->program = program_create (shaders, 2);
	
	// Populate vertex buffer
	// Populate element buffer
	glGenBuffers (1, &(app->vbo));
	glBindBuffer (GL_ARRAY_BUFFER, app->vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof (verts), verts, GL_STATIC_DRAW);
	glGenBuffers (1, &(app->ebo));
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, app->ebo);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof (indicies), indicies, GL_STATIC_DRAW);
	gl_setup_vertex_layout (app->program);
	glUseProgram (app->program);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


void app_draw (struct Application * app)
{
	glClear (GL_COLOR_BUFFER_BIT);
	ASSERT_GL;
	glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
	ASSERT_GL;
	SDL_GL_SwapWindow (app->window);
	ASSERT_GL;
}


void app_quit (struct Application * app)
{
	glUseProgram (0);
	glDisableVertexAttribArray (0);
	glDeleteProgram (app->program);
	glDeleteBuffers (1, &(app->ebo));
	glDeleteBuffers (1, &(app->vbo));
	SDL_GL_DeleteContext (app->context);
	SDL_DestroyWindow (app->window);
	SDL_Quit();
}













