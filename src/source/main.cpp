#include "OpenGLFramework.hpp"
#include <iostream>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "OpenGLFrameworkException.hpp"
#include <GLFW/glfw3.h>

std::vector<GLfloat> generatePoints()
{
	GLsizei const size = 6 * 3;

	GLfloat const points[ size ] = {
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		2.0f, 2.0f, 0.0f,
		3.0f, 3.0f, 0.0f,
		4.0f, 4.0f, 0.0f,
		5.0f, 5.0f, 0.0f,
	};

	std::vector<GLfloat> mesh;
	for( int index = 0; index < size; index++ )
	{
		mesh.push_back( points[ index ] );
	}

	return mesh;
}

std::vector<GLfloat> generatePointColors()
{
	GLsizei const size = 18;

	GLfloat const colorArray[ size ] = {
		0.0f, 0.0f, 0.0f,
		0.5f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f,
		0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
	};

	std::vector<GLfloat> colors;
	for( int index = 0; index < size; index++ )
	{
		colors.push_back( colorArray[ index ] );
	}

	return colors;
}

std::vector<GLfloat> generateTriangles()
{
	GLsizei const size = 27;

	GLfloat const vertices[ size ] = {
		0.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,

		-2.0f, -1.0f, -2.0f,
		-2.0f, -1.0f, 2.0f,
		2.0f, -1.0f, 2.0f,

		-2.0f, -1.0f, -2.0f,
		2.0f, -1.0f, 2.0f,
		2.0f, -1.0f, -2.0f
	};

	std::vector<GLfloat> mesh;
	for( int index = 0; index < size; index++ )
	{
		mesh.push_back( vertices[ index ] );
	}

	return mesh;
}

std::vector<GLfloat> generateColorData()
{
	GLsizei const size = 27;

	GLfloat const raw[ size ] = {
		0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f,

		0.3f, 0.3f, 0.3f,
		0.3f, 0.3f, 0.3f,
		0.3f, 0.3f, 0.3f,

		0.3f, 0.3f, 0.3f,
		0.3f, 0.3f, 0.3f,
		0.3f, 0.3f, 0.3f
	};

	std::vector<GLfloat> colors;

	for( int index = 0; index < size; index++ )
	{
		colors.push_back( raw[ index ] );
	}

	return colors;
}

int main( int _argc, char ** _argv )
{
	try
	{
		OpenGLFramework opengl( "shader/vertex.glsl", "shader/fragment.glsl" );
		opengl.initialize();

		Window * window = opengl.createWindow( "Sample", 800, 600 );
		VAO const triangleVAO = window->createVAO();
		VAO const pointVAO = window->createVAO();

		clock_t lastInterval = clock();

		while( window->isOpen() )
		{
			window->resetBuffer();

			// Run your algorithms here and generate or manipulate the render data
			std::vector<GLfloat> const triangles = generateTriangles();
			std::vector<GLfloat> const triangleColors = generateColorData();
			std::vector<GLfloat> const points = generatePoints();
			std::vector<GLfloat> const pointColors = generatePointColors();

			window->sendRenderData( pointVAO, points, pointColors );
			window->sendRenderData( triangleVAO, triangles, triangleColors );

			// Rotate the object
			static GLfloat rotationAngle = 0.0f;
			const clock_t now = clock();
			const GLfloat deltaTime = static_cast<GLfloat>(now - lastInterval) / CLOCKS_PER_SEC; // seconds
			lastInterval = now;

			rotationAngle += deltaTime * 200.0f; // 20 degrees per second

			glm::mat4x4 const scalingMatrix = glm::scale( glm::mat4( 1.0f ), glm::vec3( 100.0f, 100.0f, 100.0f ) );
			glm::mat4x4 rotationMatrix = glm::rotate(glm::mat4(1.0f), glm::radians(rotationAngle), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4x4 const modelMatrix = rotationMatrix * scalingMatrix;

			window->renderTriangles( triangleVAO, triangles.size() / 3, modelMatrix );
			window->renderPoints( pointVAO, points.size(), scalingMatrix );

			window->swapBuffer();
		}
	}

	catch( OpenGLFrameworkException const exception )
	{
		std::cerr << exception.what() << std::endl;
	}
}