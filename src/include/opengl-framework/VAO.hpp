#pragma once

#include <GL/glew.h>

struct VAO
{
	VAO() : 
		id( 0u ),
		vertexBufferId( 0u ),
		colorBufferId( 0u )
	{

	}

	GLuint id;
	GLuint vertexBufferId;
	GLuint colorBufferId;
};