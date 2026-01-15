#include "Window.hpp"
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <utility>
#include "OpenGLErrorHandling.hpp"

/**
	The window instance must be initialized with null.
*/
Window * Window::m_instance = NULL;

/**
	Initializes the window instance and returns always the same object.

	@return Returns the window instance.
*/
Window * Window::getInstance()
{
	if( NULL == m_instance )
	{
		m_instance = new Window();
	}

	return m_instance;
}

/**
	Constructor.
	Initializes the window and the context ressources.
*/
Window::Window() :
	m_window( NULL ),
	m_shaderProgram( 0u ),
	m_camera( CameraType::PERSPECTIVE )
{
	glewExperimental = GL_TRUE;
	if( GL_FALSE == glfwInit() )
	{
		std::cerr << "Unable to initialize glfw library." << std::endl;
	}

	else
	{
		m_camera.setDistancePlanes( 1.0f, 10000.0f );
	}
}

/**
	Destructor.
	Closes the window and frees all allocated ressources.
*/
Window::~Window()
{
	close();

	glfwTerminate();
}

/**
	Opens the window with the passed pixel width and height.

	@param _title The title for the window.
	@param _pixelWidth The width of the window in pixels.
	@param _pixelHeight The height of the window in pixels.
*/
void Window::open( std::string const & _title,
				   GLuint const & _pixelWidth,
				   GLuint const & _pixelHeight)
{
	if( isOpen() )
	{
		std::cerr << "Unable to open a window. Only one window can be open." << std::endl;
	}

	else
	{
		glfwWindowHint( GLFW_SAMPLES, 4 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
		glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
		glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

		m_window = glfwCreateWindow( static_cast< int >( _pixelWidth ),
									 static_cast< int >( _pixelHeight ),
									 _title.c_str(), NULL, NULL );
		setActive();
		
		GLenum const glewError = glewInit();
		if( GLEW_OK != glewError )
		{
			close();
			std::cerr << "Unable to initialize glew library: ";
			std::cerr << glewGetErrorString( glewError ) << std::endl;
		}

		else
		{
			std::cout << "Glew is initialized for window " << _title.c_str() << "." << std::endl;
			OpenGLErrorHandling::clearOpenGLErrors();
		}

		if( !glewIsSupported( "GL_VERSION_3_3" ) )
		{
			close();
			std::cerr << "OpenGL 3.3 is not supported!" << std::endl;
		}

		else
		{
			std::cout << "OpenGL 3.3 is supported." << std::endl;
		}

		m_camera.setWindowSize( static_cast< GLfloat > ( _pixelWidth ),
								static_cast< GLfloat >( _pixelHeight ) );
		glfwSetKeyCallback( m_window, Window::handleKeyEvent );
		glfwSetWindowSizeCallback( m_window, Window::handleResizeEvent );
		glfwSetFramebufferSizeCallback( m_window, Window::handleFramebufferResizeEvent );
	}
}

/**
	@return	Returns true, if the window is open. False, otherwise.
*/
GLboolean Window::isOpen() const
{
	return NULL != m_window;
}

/**
	Closes the window.
	You can reopen the window by calling open().
*/
void Window::close()
{
	if( isOpen() )
	{
		glfwSetKeyCallback( m_window, NULL );
		glfwSetWindowSizeCallback( m_window, NULL );
		glfwSetFramebufferSizeCallback( m_window, NULL );

		glfwSetWindowShouldClose( m_window, GL_FALSE );
		
		m_window = NULL;
	}
}

void Window::setShaderProgram( GLuint const & _shaderProgram )
{
	m_shaderProgram = _shaderProgram;
}

/**
	Creates a new vao object.
	Can be used multiple times.

	@return	Returns the opengl identifier for this vao.
*/
VAO Window::createVAO()
{
	setActive();

	if( isOpen() )
	{
		glEnable( GL_DEPTH_TEST );
		OpenGLErrorHandling::checkOpenGL();

		VAO vao;

		glGenVertexArrays( 1, &vao.id );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( vao.id );
		OpenGLErrorHandling::checkOpenGL();

		glGenBuffers( 1, &vao.vertexBufferId );
		OpenGLErrorHandling::checkOpenGL();

		glBindBuffer( GL_ARRAY_BUFFER, vao.vertexBufferId );
		OpenGLErrorHandling::checkOpenGL();

		glEnableVertexAttribArray( 0 );
		OpenGLErrorHandling::checkOpenGL();

		glGenBuffers( 1, &vao.colorBufferId );
		OpenGLErrorHandling::checkOpenGL();

		glBindBuffer( GL_ARRAY_BUFFER, vao.colorBufferId );
		OpenGLErrorHandling::checkOpenGL();

		glEnableVertexAttribArray( 1 );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( 0 );
		OpenGLErrorHandling::checkOpenGL();

		return vao;
	}

	else
	{
		std::cerr << "Unable to create VAO. First you need to open a window." << std::endl;

		return VAO();
	}
}

void Window::destroyVAO( VAO const & _vao )
{
	setActive();

	if( isOpen() && GL_TRUE == glIsVertexArray( _vao.id ) )
	{
		glDeleteVertexArrays( 1, &_vao.id );
		OpenGLErrorHandling::checkOpenGL();
	}
}

/**
	Sets the new eye point of the window camera to the passed point.

	@param _eyePoint The new eye point.
*/
void Window::setEyePoint( glm::vec4 const & _eyePoint )
{
	m_camera.setEyePoint( _eyePoint );
}

/**
	@return Returns the width of the window.
*/
GLuint Window::getWidth() const
{
	if( isOpen() )
	{
		int width = 0;
		glfwGetWindowSize( m_window, &width, NULL );
		return static_cast< GLuint >( width );
	}

	else
	{
		return 0;
	}
}

/**
	@return Returns the window height.
*/
GLuint Window::getHeight() const
{
	if( isOpen() )
	{
		int height = 0;
		glfwGetWindowSize( m_window, NULL, &height );
		return static_cast< GLuint >( height );
	}

	else
	{
		return 0;
	}
}

/**
	Activates the current window context to be used by OpenGL.
*/
void Window::setActive()
{
	if( isOpen() )
	{
		glfwMakeContextCurrent( m_window );
	}
}

/**
	Clears the current depth and color buffer.
*/
void Window::resetBuffer()
{
	if( isOpen() )
	{
		setActive();
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		OpenGLErrorHandling::checkOpenGL();
	}
}

/**
	Draws the current buffer to the monitor and clears the old buffer to be
	ready for rendering.
*/
void Window::swapBuffer()
{
	if( isOpen() )
	{
		setActive();

		glfwSwapBuffers( m_window );

		glfwPollEvents();

		if( GL_TRUE == glfwWindowShouldClose( m_window ) )
		{
			close();
		}
	}
}

void Window::sendRenderData( VAO const & _vao,
							 std::vector<GLfloat> const & _vertices,
							 std::vector<GLfloat> const & _colors )
{
	setActive();

	if( !isOpen() || GL_FALSE == glIsVertexArray( _vao.id ) )
	{
		std::cerr << "Unable to send render data to the GPU. Window is not open or VAO not created." << std::endl;
	}

	else
	{
		glBindVertexArray( _vao.id );
		OpenGLErrorHandling::checkOpenGL();

		glBindBuffer( GL_ARRAY_BUFFER, _vao.vertexBufferId );
		OpenGLErrorHandling::checkOpenGL();

		glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * _vertices.size(), &_vertices[ 0 ], GL_STATIC_DRAW );
		OpenGLErrorHandling::checkOpenGL();

		glEnableVertexAttribArray( 0 );
		OpenGLErrorHandling::checkOpenGL();

		glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, ( void * ) NULL );
		OpenGLErrorHandling::checkOpenGL();

		glBindBuffer( GL_ARRAY_BUFFER, _vao.colorBufferId );
		OpenGLErrorHandling::checkOpenGL();

		glBufferData( GL_ARRAY_BUFFER, sizeof( GLfloat ) * _colors.size(), &_colors[ 0 ], GL_STATIC_DRAW );
		OpenGLErrorHandling::checkOpenGL();

		glEnableVertexAttribArray( 1 );
		OpenGLErrorHandling::checkOpenGL();

		glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, ( void * ) NULL );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( 0 );
		OpenGLErrorHandling::checkOpenGL();
	}
}

void Window::renderPoints( VAO const & _vao,
						   size_t const & _pointCount,
						   glm::mat4 const & _modelMatrix )
{
	setActive();

	if( !isOpen() || GL_FALSE == glIsVertexArray( _vao.id ) || 0u == m_shaderProgram )
	{
		std::cerr << "Unable to render points. Check your window and vao state." << std::endl;
	}

	else
	{
		Camera const camera = getCamera();
		glm::mat4x4 const viewMatrix = camera.viewMatrix();
		glm::mat4x4 const projectionMatrix = camera.projectionMatrix();

		glm::mat4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * _modelMatrix;

		glUseProgram( m_shaderProgram );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( _vao.id );
		OpenGLErrorHandling::checkOpenGL();

		GLuint matrixUniform = glGetUniformLocation( m_shaderProgram, "modelViewProjection" );
		OpenGLErrorHandling::checkOpenGL();

		glUniformMatrix4fv( matrixUniform, 1, GL_FALSE, glm::value_ptr( modelViewProjectionMatrix ) );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( _vao.id );
		OpenGLErrorHandling::checkOpenGL();

		glDrawArrays( GL_POINTS, 0, static_cast<GLsizei>( _pointCount ) );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( 0 );
		OpenGLErrorHandling::checkOpenGL();

		glUseProgram( 0 );
		OpenGLErrorHandling::checkOpenGL();
	}
}

/**
	Uses OpenGL to draw a mesh of triangles.

	@param	_modelMatrix	The matrix to manipulate the mesh before rendering.
*/
void Window::renderTriangles( VAO const & _vao,
							  size_t const & _triangleCount,
							  glm::mat4 const & _modelMatrix )
{
	setActive();

	if( !isOpen() || GL_FALSE == glIsVertexArray( _vao.id ) || 0u == m_shaderProgram )
	{
		std::cerr << "Unable to render triangles. Check your window and vao state." << std::endl;
	}

	else
	{
		Camera const camera = getCamera();
		glm::mat4x4 const viewMatrix = camera.viewMatrix();
		glm::mat4x4 const projectionMatrix = camera.projectionMatrix();

		glm::mat4x4 modelViewProjectionMatrix = projectionMatrix * viewMatrix * _modelMatrix;

		glUseProgram( m_shaderProgram );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( _vao.id );
		OpenGLErrorHandling::checkOpenGL();

		GLuint matrixUniform = glGetUniformLocation( m_shaderProgram, "modelViewProjection" );
		OpenGLErrorHandling::checkOpenGL();

		glUniformMatrix4fv( matrixUniform, 1, GL_FALSE, glm::value_ptr( modelViewProjectionMatrix ) );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( _vao.id );
		OpenGLErrorHandling::checkOpenGL();

		glDrawArrays( GL_TRIANGLES, 0, static_cast<GLsizei>( _triangleCount ) );
		OpenGLErrorHandling::checkOpenGL();

		glBindVertexArray( 0 );
		OpenGLErrorHandling::checkOpenGL();

		glUseProgram( 0 );
		OpenGLErrorHandling::checkOpenGL();
	}
}

/**
	@return Returns a copy of the camera instance.
*/
Camera Window::getCamera() const
{
	return Camera( m_camera );
}

void Window::setSpacebarHandler(std::function<void()> _handler) {
	m_spacebarHandler = std::move(_handler);
}

/**
	GLFW keyboard callback function.

	@param _window The window instance of the key event.
	@param _key The key enumeration ( GLFW_KEY_* ).
	@param _scancode Unused.
	@param _action The action of the event ( GLFW_PRESS or GLFW_RELEASE ).
	@param _mods Mods ( GLFW_MOD_SHIFT, GLFW_MOD_CONTROL, GLFW_MOD_ALT, GLFW_MOD_SUPER ).
*/
void Window::handleKeyEvent( GLFWwindow * _window,
							 GLint _key,
							 GLint _scancode,
							 GLint _action,
							 GLint _mods )
{
	Window * const window = getInstance();
	window->handleKeyEvent( _key, _action, _mods );
}

/**
	GLFW window resize callback function.

	@param _window The window instance of the resize event.
	@param _width The new width of the window.
	@param _height The new height of the window.
*/
void Window::handleResizeEvent( GLFWwindow * _window,
								int _width,
								int _height )
{
	Window * const window = getInstance();
	window->handleResizeEvent( static_cast< GLuint >( _width ),
							   static_cast< GLuint >( _height ) );
}

/**
	Callback function to resize the framebuffer of the passed window instance.

	@param	_window	The window for the resize event.
	@param	_width	The new framebuffer width in pixels.
	@param	_height	The new framebuffer height in pixels.
*/
void Window::handleFramebufferResizeEvent( GLFWwindow * _window, int _width, int _height )
{
	Window * const window = getInstance();
	window->handleFramebufferResizeEvent( static_cast< GLsizei >( _width ),
										  static_cast< GLsizei >( _height ) );
}

/**
	Callback function to handle key events for this window.

	@param	_key	The key to handle.
	@param	_action	The action. For example GLFW_PRESS or GLFW_REPEAT.
*/
void Window::handleKeyEvent( GLint const & _key,
							 GLint const & _action,
							 GLint const & )
{
	if( isOpen() )
	{
		if( GLFW_PRESS == _action || GLFW_REPEAT == _action )
		{
			std::cout << "Handle key " << _key << "." << std::endl;

			switch( _key )
			{
				case GLFW_KEY_W:
					m_camera.translateEyePoint( m_camera.viewDirection() * 10.0f );
					break;

				case GLFW_KEY_S:
					m_camera.translateEyePoint( m_camera.viewDirection() * -10.0f );
					break;

				case GLFW_KEY_A:
					m_camera.translateEyePoint( m_camera.horizontalDirection() * 10.0f );
					break;

				case GLFW_KEY_D:
					m_camera.translateEyePoint( m_camera.horizontalDirection() * -10.0f );
					break;

				case GLFW_KEY_Q:
					m_camera.translateEyePoint( m_camera.upDirection() * 10.0f );
					break;

				case GLFW_KEY_E:
					m_camera.translateEyePoint( m_camera.upDirection() * -10.0f );
					break;

				case GLFW_KEY_LEFT:
					m_camera.rotateYaw( 5.0f );
					break;

				case GLFW_KEY_RIGHT:
					m_camera.rotateYaw( -5.0f );
					break;

				case GLFW_KEY_UP:
					m_camera.rotatePitch( 5.0f );
					break;

				case GLFW_KEY_DOWN:
					m_camera.rotatePitch( -5.0f );
					break;

				case GLFW_KEY_R:
					m_camera.setEyePoint( glm::vec4( 0.0f, 0.0f, 500.0f, 1.0f ) );
					m_camera.resetAngles();
					break;

				case GLFW_KEY_SPACE:
					m_spacebarHandler();
					break;

				default:
					break;
			}
		}
	}
}

/**
	Callback function for the resize event of this window.

	@param	_width	The new width of the window.
	@param	_height	The new height of the window.
*/
void Window::handleResizeEvent( GLuint const & _width,
								GLuint const & _height )
{
	if( isOpen() )
	{
		std::cout << "Window resized." << std::endl;
		m_camera.setWindowSize( static_cast< GLfloat >( _width ),
								static_cast< GLfloat >( _height ) );
	}
}

/**
	Callback function for the framebuffer resize event without window instance.

	@param	_width	The new framebuffer size in pixels.
	@param	_height	The new framebuffer size in pixels.
*/
void Window::handleFramebufferResizeEvent( GLsizei const & _width, GLsizei const & _height )
{
	if( isOpen() )
	{
		std::cout << "Framebuffer resized." << std::endl;
		glViewport( 0, 0, _width, _height );
	}
}
