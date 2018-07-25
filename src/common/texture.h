#pragma once

//void glTexImage2D(GLenum target,GLint level,GLint internalFormat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const GLvoid * data);
//GLint glGetUniformLocation(GLuint program,const GLchar *name);
#include <SDL2/SDL_opengles2.h>

struct Texture
{
	//Used for glGenTextures, glBindTexture
	GLuint id;
	
	//The texture variable name of the shader program.
	//Used for glGetUniformLocation
	char * name;

	//A texture unit consists of the texture enable state,
	//texture matrix stack, texture environment and currently bound
	//texture. Modifying any of these states has an effect only on
	//the active texture unit.
	//Used for glActiveTexture, glUniform1i
	int unit;

	//Image format.
	//Used for glTexImage2D, glTexSubImage2D
	int w;
	int h;
	GLenum format;
	GLenum type;
	
	//Image data.
	void * data;
};


void tex_create (struct Texture * tex, GLuint program)
{
	GLint max_image_units;
	glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS, &max_image_units);
	ASSERT (tex->unit >= 0);
	ASSERT (tex->unit < max_image_units);
	TRACE_F ("GL_MAX_TEXTURE_IMAGE_UNITS %i", max_image_units);
	
	//glActiveTexture selects which texture unit subsequent texture state calls will affect. 
	//The number of texture units an implementation supports is implementation dependent, but must be at least 80. 
	glActiveTexture (GL_TEXTURE0 + tex->unit);
	
	//glTex* functions will modify current bound texture.
	glGenTextures (1, &(tex->id));
	glBindTexture (GL_TEXTURE_2D, tex->id);
	
	glTexImage2D (GL_TEXTURE_2D, 0, tex->format, tex->w, tex->h, 0, tex->format, tex->type, NULL);
	ASSERT_GL;
	
	{
		//TODO: Do we need to store the UniformLocation integer?
		GLint r = glGetUniformLocation (program, tex->name);
		ASSERT (r >= 0);
		ASSERT_GL;
		glUniform1i (r, tex->unit);
	}
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap (GL_TEXTURE_2D);
	ASSERT_GL;
}


void tex_update (struct Texture * tex)
{
	glBindTexture (GL_TEXTURE_2D, tex->id);
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, tex->w, tex->h, tex->format, tex->type, tex->data);
	ASSERT_GL;
}











struct Pallete
{
	//Used for glGenTextures, glBindTexture
	GLuint id;
	
	//The texture variable name of the shader program.
	//Used for glGetUniformLocation
	char * name;

	//A texture unit consists of the texture enable state,
	//texture matrix stack, texture environment and currently bound
	//texture. Modifying any of these states has an effect only on
	//the active texture unit.
	//Used for glActiveTexture, glUniform1i
	int unit;

	//Image format.
	//Used for glTexImage2D, glTexSubImage2D
	int w;
	GLenum format;
	GLenum type;
	
	//Image data.
	void * data;
};


void pallete_create (struct Pallete * tex, GLuint program)
{
	GLint max_image_units;
	glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS, &max_image_units);
	ASSERT (tex->unit >= 0);
	ASSERT (tex->unit < max_image_units);
	TRACE_F ("GL_MAX_TEXTURE_IMAGE_UNITS %i", max_image_units);
	
	//glActiveTexture selects which texture unit subsequent texture state calls will affect. 
	//The number of texture units an implementation supports is implementation dependent, but must be at least 80. 
	glActiveTexture (GL_TEXTURE0 + tex->unit);
	
	//glTex* functions will modify current bound texture.
	glGenTextures (1, &(tex->id));
	glBindTexture (GL_TEXTURE_2D, tex->id);
	
	glTexImage2D (GL_TEXTURE_2D, 0, tex->format, tex->w, 1, 0, tex->format, tex->type, NULL);
	ASSERT_GL;
	
	{
		//TODO: Do we need to store the UniformLocation integer?
		GLint r = glGetUniformLocation (program, tex->name);
		ASSERT (r >= 0);
		ASSERT_GL;
		glUniform1i (r, tex->unit);
	}
	
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
	glGenerateMipmap (GL_TEXTURE_2D);
	

	ASSERT_GL;
}


void pallete_update (struct Pallete * tex)
{
	glBindTexture (GL_TEXTURE_2D, tex->id);
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, tex->w, 1, tex->format, tex->type, tex->data);
	ASSERT_GL;
}
