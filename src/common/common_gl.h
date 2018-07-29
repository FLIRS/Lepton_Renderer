#pragma once

#include <SDL2/SDL_opengles2.h>

#include "debug.h"


#define ASSERT_GL assert_gl (__FILE__,__LINE__)

char const * glGetError_str (GLenum e)
{
	switch (e)
	{
	case GL_INVALID_OPERATION:              return "GL_INVALID_OPERATION";
	case GL_INVALID_ENUM:                   return "GL_INVALID_ENUM";
	case GL_INVALID_VALUE:                  return "GL_INVALID_VALUE";
	case GL_INVALID_FRAMEBUFFER_OPERATION:  return "GL_INVALID_FRAMEBUFFER_OPERATION";
	case GL_OUT_OF_MEMORY:                  return "GL_OUT_OF_MEMORY";
	}
	return "GL_<UNKNOWN>";
}


void assert_gl (const char *file, int line)
{
	GLenum e = glGetError ();
	if (e == GL_NO_ERROR) {return;}
	while (1)
	{
		fprintf (stderr, "%s:%i %s (%i)", file, line, glGetError_str (e), e);
		e = glGetError ();
		if (e == GL_NO_ERROR) {break;}
	}
	exit (1);
}



void glUniform1i_name (GLuint program, GLchar const * name, GLint v0)
{
	GLint location;
	location = glGetUniformLocation (program, name);
	ASSERT (location >= 0);
	glUniform1i (location, v0);
}
