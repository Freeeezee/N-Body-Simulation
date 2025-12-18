#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <vector>
#include "Camera.hpp"
#include "VAO.hpp"

/**
	Implementation for a graphical context representing a
	whole window.
*/
class Window
{
public:
	static Window * getInstance();

	~Window();

	void open( std::string const & _title,
			   GLuint const & _pixelWidth,
			   GLuint const & _pixelHeight );
	GLboolean isOpen() const;
	void close();

	void setShaderProgram( GLuint const & _shaderProgram );

	VAO createVAO();
	void destroyVAO( VAO const & _vao );

	void setEyePoint( glm::vec4 const & _eyePoint );

	GLuint getWidth() const;
	GLuint getHeight() const;

	void setActive();
	void resetBuffer();
	void swapBuffer();

	void sendRenderData( VAO const & _vao,
						 std::vector<GLfloat> const & _vertices,
						 std::vector<GLfloat> const & _colors );

	void renderPoints( VAO const & _vao,
					   size_t const & _pointCount,
					   glm::mat4 const & _modelMatrix );
	void renderTriangles( VAO const & _vao,
						  size_t const & _triangleCount,
						  glm::mat4 const & _modelMatrix );

	Camera getCamera() const;

private:
	static void handleKeyEvent( GLFWwindow * _window,
								GLint _key,
								GLint _scancode,
								GLint _action,
								GLint _mods );
	static void handleResizeEvent( GLFWwindow * _window,
								   int _width,
								   int _height );
	static void handleFramebufferResizeEvent( GLFWwindow * _window,
											  int _width,
											  int _height );

	void handleKeyEvent( GLint const & _key,
						 GLint const & _action,
						 GLint const & _mods );
	void handleResizeEvent( GLuint const & _width,
							GLuint const & _height );
	void handleFramebufferResizeEvent( GLsizei const & _width,
									   GLsizei const & _height );

	Window();

	static Window * m_instance;	//!< The window instance.
	GLuint m_shaderProgram;		//!< The shader program to render.
	Camera m_camera;			//!< The camera instance.
	GLFWwindow * m_window;		//!< The glfw window instance. Can be null before initialization.
};