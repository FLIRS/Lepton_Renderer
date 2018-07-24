#pragma once

//void glTexImage2D(GLenum target,GLint level,GLint internalFormat,GLsizei width,GLsizei height,GLint border,GLenum format,GLenum type,const GLvoid * data);
//GLint glGetUniformLocation(GLuint program,const GLchar *name);
#include <SDL2/SDL_opengles2.h>

struct Texture
{
	GLuint id;
	char * name;
	int w;
	int h;
	GLenum format;
	void * data;
	int unit;
};


void tex_create (struct Texture * tex, GLuint program)
{
	glGenTextures (1, &(tex->id));
	glActiveTexture (GL_TEXTURE0 + tex->unit);
	glBindTexture (GL_TEXTURE_2D, tex->id);
	
	glTexImage2D (GL_TEXTURE_2D, 0, tex->format, tex->w, tex->h, 0, tex->format, GL_UNSIGNED_BYTE, NULL);
	ASSERT_GL;
	
	{
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


void tex_update (struct Texture * tex)
{
	glTexSubImage2D (GL_TEXTURE_2D, 0, 0, 0, tex->w, tex->h, tex->format, GL_UNSIGNED_BYTE, tex->data);
	ASSERT_GL;
}
