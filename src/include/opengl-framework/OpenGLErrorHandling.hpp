#pragma once

#include <GL/glew.h>
#include <iostream>

class OpenGLErrorHandling
{
public:
	static void clearOpenGLErrors()
	{
		for( GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError() )
		{
			// Ignore them
		}
	}

	static void checkOpenGL()
	{
		GLenum error = GL_NO_ERROR;
		for( GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError() )
		{
			std::cerr << "OpenGL error: " << error << ", " << glewGetErrorString( error ) << std::endl;
		}
	}
};