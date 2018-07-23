/*
https://askubuntu.com/questions/244133/how-do-i-get-egl-and-opengles-libraries-for-ubuntu-running-on-virtualbox
https://gist.github.com/SuperV1234/5c5ad838fe5fe1bf54f9
https://www.haroldserrano.com/blog/how-to-apply-textures-to-a-character-in-ios
https://www.sallyx.org/sally/c/opengl/fps
http://www.learnopengles.com/tag/linmath-h/
https://github.com/KhronosGroup/Vulkan-LoaderAndValidationLayers/blob/master/demos/cube.c
https://wiki.libsdl.org/SDL_RegisterEvents
*/

#define GL_GLEXT_PROTOTYPES 1
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengles2.h>

#include <stdio.h>
#include <sys/stat.h>
//getopt()
#include <unistd.h>

#include "debug.h"
#include "common_gl.h"
#include "vertex.h"

#define APP_SHORT_NAME "Short app name"
#define APP_WINDOW_X SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_Y SDL_WINDOWPOS_UNDEFINED
#define APP_WINDOW_HEIGHT 1080
#define APP_WINDOW_WIDTH 1920
#define APP_WINDOW_ASPECT (APP_WINDOW_WIDTH / APP_WINDOW_HEIGHT)
#define APP_WINDOW_FOV (45.0f * M_PI / 180.0f)



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


GLuint app_create_texture (GLuint program)
{
	ASSERT (program != 0);
	uint32_t width = 100;
	uint32_t height = 100;
	
	GLuint texture;
	glGenTextures (1, &texture);
	glActiveTexture (GL_TEXTURE0);
	glBindTexture (GL_TEXTURE_2D, texture);
	glTexImage2D (GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, NULL);
	ASSERT_GL;
	
	{
		GLint r = glGetUniformLocation (program, "tex");
		ASSERT (r >= 0);
		ASSERT_GL;
		glUniform1i (r, 0);
	}
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap (GL_TEXTURE_2D);
	ASSERT_GL;
	
	return texture;
}


void app_update_texture (GLuint program, uint8_t * data, uint32_t w, uint32_t h)
{
	ASSERT (program != 0);
	ASSERT (data != NULL);
/*
void glTexSubImage2D
(
	GLenum target,
	GLint level,
	GLint xoffset,
	GLint yoffset,
	GLsizei width,
	GLsizei height,
	GLenum format,
	GLenum type,
	const GLvoid * pixels
);
*/
	for (uint32_t i = 0; i < w * h; i = i + 1)
	{
		data [i] = data [i] * 3;
	}
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, w, h, GL_ALPHA, GL_UNSIGNED_BYTE, data);
	ASSERT_GL;
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
	
	SDL_Window * window;
	SDL_GLContext context;
	//const Uint8 * keyboard;
	GLuint program;
	GLuint texture;
	GLuint m_vbo;
	GLuint m_ebo;
    
	{
		int r = SDL_Init (SDL_INIT_VIDEO);
		ASSERT_F (r == 0, "SDL_Init: %s", SDL_GetError());
	}
	
	
	window = SDL_CreateWindow (APP_SHORT_NAME, APP_WINDOW_X, APP_WINDOW_Y, APP_WINDOW_WIDTH, APP_WINDOW_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
	ASSERT_F (window != NULL, "SDL_CreateWindow: %s", SDL_GetError());
	
	//keyboard = SDL_GetKeyboardState (NULL);
	
	// Initialize rendering context
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_MINOR_VERSION, 1);
	SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
	
	context = SDL_GL_CreateContext (window);
	ASSERT_F (context != NULL, "SDL_GL_CreateContext: %s", SDL_GetError());

	// Use VSYNC
	SDL_GL_SetSwapInterval (1);
	
	struct Shader shaders [2] = 
	{
		{0, GL_VERTEX_SHADER, "src/cam2d.glvs"},
		{0, GL_FRAGMENT_SHADER, "src/cam2d.glfs"}
	};
	program = gl_create_program (shaders, 2);
	glUseProgram (program);
	app_create_texture (program);
	
	// Populate vertex buffer
	glGenBuffers (1, &m_vbo);
	glBindBuffer (GL_ARRAY_BUFFER, m_vbo);
	glBufferData (GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

	// Populate element buffer
	glGenBuffers (1, &m_ebo);
	glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData (GL_ELEMENT_ARRAY_BUFFER, sizeof(indicies), indicies, GL_STATIC_DRAW);
	gl_setup_vertex_layout (program);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	uint8_t data [100*100];
	for (int i = 0; i < 100*100; i = i + 1)
	{
		data [i] = i;
	}
	
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
		app_update_texture (program, data, 100, 100);
		glClear (GL_COLOR_BUFFER_BIT);
		glDrawElements (GL_TRIANGLES, 6, GL_UNSIGNED_INT, NULL);
		SDL_GL_SwapWindow (window);
	}
	
main_quit:

	printf ("Exiting...\n");
	glUseProgram (0);
	glDisableVertexAttribArray (0);
	glDeleteProgram (program);
	glDeleteTextures (1, &texture);
	glDeleteBuffers (1, &m_ebo);
	glDeleteBuffers (1, &m_vbo);

	SDL_GL_DeleteContext (context);
	SDL_DestroyWindow (window);
	TRACE_F ("SDL_Quit%s", "");
	SDL_Quit();
	return 0;
}
