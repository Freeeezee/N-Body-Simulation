#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <iostream>
#include <sstream>

#include "OpenGLFramework.hpp"
#include "OpenGLFrameworkException.hpp"
#include "OpenGLErrorHandling.hpp"

OpenGLFramework::OpenGLFramework( std::string const & _vertexShaderFile,
								  std::string const & _fragmentShaderFile ) :
	m_vertexShaderFile( _vertexShaderFile ),
	m_fragmentShaderFile( _fragmentShaderFile ),
	m_shaderProgram( 0u )
{

}

OpenGLFramework::~OpenGLFramework()
{
	shutdown();
}

void OpenGLFramework::initialize()
{
	// glew
	glewExperimental = GL_TRUE;

	// glfw
	if( GL_FALSE == glfwInit() )
	{
		std::cerr << "Unable to initialize glfw library." << std::endl;

		throw OpenGLFrameworkException( "Unable to initialize glfw library." );
	}
}

bool OpenGLFramework::isInitialized() const
{
	return 0 != m_shaderProgram;
}

Window * OpenGLFramework::createWindow( std::string const & _title,
										GLuint const & _pixelWidth,
										GLuint const & _pixelHeight )
{
	Window * window = Window::getInstance();

	if( !window->isOpen() )
	{
		window->open( _title, _pixelWidth, _pixelHeight );

		GLuint const programId = buildShaderProgram( m_vertexShaderFile, m_fragmentShaderFile );
		window->setShaderProgram( programId );

		window->setEyePoint( glm::vec4( 0.0f, 0.0f, 500.0f, 1.0f ) );
	}

	else
	{
		std::cerr << "Unable to open a new window. Only one window is supported." << std::endl;
	}

	return window;
}

void OpenGLFramework::shutdown()
{
	if( isInitialized() )
	{
		glDeleteProgram( m_shaderProgram );
		OpenGLErrorHandling::checkOpenGL();
	}
}

GLuint OpenGLFramework::buildShaderProgram( std::string const & _vertexShaderFile,
											std::string const & _fragmentShaderFile )
{
	GLuint const vertexShaderId = buildShader( _vertexShaderFile, GL_VERTEX_SHADER );
	GLuint const fragmentShaderId = buildShader( _fragmentShaderFile, GL_FRAGMENT_SHADER );

	return linkShaderProgramm( vertexShaderId, fragmentShaderId );
}

GLuint OpenGLFramework::buildShader( std::string const & _shaderFile, GLuint const & _shaderType )
{
	std::cout << "Building shader " << _shaderType << " ..." << std::endl;
	
	GLuint const shaderId = glCreateShader( _shaderType );
	OpenGLErrorHandling::checkOpenGL();

	std::string const shaderCode = readShaderFile( _shaderFile );

	std::cout << "Compiling shader " << _shaderType << " ..." << std::endl;

	compileShader( shaderId, shaderCode );

	return shaderId;
}

void OpenGLFramework::compileShader( GLuint const & _shaderId, std::string const & _shaderCodeString )
{
	char const * shaderCode = _shaderCodeString.c_str();
	glShaderSource( _shaderId, 1, &shaderCode, NULL );
	OpenGLErrorHandling::checkOpenGL();

	glCompileShader( _shaderId );
	OpenGLErrorHandling::checkOpenGL();

	GLint isSuccessful = GL_FALSE;
	glGetShaderiv( _shaderId, GL_COMPILE_STATUS, &isSuccessful );
	OpenGLErrorHandling::checkOpenGL();

	if( GL_FALSE == isSuccessful )
	{
		std::string const buildLog = readShaderBuildLog( _shaderId );
		std::cerr << buildLog << std::endl;

		throw OpenGLFrameworkException( "Unable to compile shader! Check the build log for more informations." );
	}
}

std::string OpenGLFramework::readShaderBuildLog( GLuint const & _shaderId ) const
{
	GLint compileLogLength = 0;
	glGetShaderiv( _shaderId, GL_INFO_LOG_LENGTH, &compileLogLength );
	OpenGLErrorHandling::checkOpenGL();

	char * errorMessageBuffer = new char[ compileLogLength ];
	glGetShaderInfoLog( _shaderId, compileLogLength, NULL, errorMessageBuffer );
	OpenGLErrorHandling::checkOpenGL();

	std::string const errorMessage( errorMessageBuffer, compileLogLength );
	delete[] errorMessageBuffer;

	return errorMessage;
}

std::string OpenGLFramework::readShaderFile( std::string const _shaderFile ) const
{
	std::ifstream fileStream( _shaderFile, std::ios::in );
	if( fileStream.is_open() )
	{
		std::stringstream codeStream;
		codeStream << fileStream.rdbuf();

		std::string const code = codeStream.str();
		fileStream.close();
		return code;
	}
	else
	{
		std::stringstream errorStream;
		errorStream << "Unable to open " << _shaderFile << ". Please check your file path.";

		throw OpenGLFrameworkException( errorStream.str() );
	}
}

GLuint OpenGLFramework::linkShaderProgramm( GLuint const & _vertexShader,
											GLuint const & _fragmentShader )
{
	std::cout << "Linking shader program ..." << std::endl;

	GLuint const programId = glCreateProgram();
	OpenGLErrorHandling::checkOpenGL();

	glAttachShader( programId, _vertexShader );
	OpenGLErrorHandling::checkOpenGL();

	glAttachShader( programId, _fragmentShader );
	OpenGLErrorHandling::checkOpenGL();

	glLinkProgram( programId );
	OpenGLErrorHandling::checkOpenGL();

	GLint isSuccessful = GL_FALSE;
	glGetProgramiv( programId, GL_LINK_STATUS, &isSuccessful );
	OpenGLErrorHandling::checkOpenGL();

	if( GL_FALSE == isSuccessful )
	{
		std::string const & buildLog = readProgramBuildLog( programId );
		std::cerr << buildLog << std::endl;

		throw OpenGLFrameworkException( "Unable to link shader program! Check the build log for more informations." );
	}

	else
	{
		glDetachShader( programId, _vertexShader );
		OpenGLErrorHandling::checkOpenGL();

		glDetachShader( programId, _fragmentShader );
		OpenGLErrorHandling::checkOpenGL();

		glDeleteShader( _vertexShader );
		OpenGLErrorHandling::checkOpenGL();

		glDeleteShader( _fragmentShader );
		OpenGLErrorHandling::checkOpenGL();

		std::cout << "The shader program was successfully built." << std::endl;

		return programId;
	}
}

std::string OpenGLFramework::readProgramBuildLog( GLuint const & _programId ) const
{
	GLint compileLogLength = 0;
	glGetProgramiv( _programId, GL_INFO_LOG_LENGTH, &compileLogLength );
	OpenGLErrorHandling::checkOpenGL();

	char * errorMessageBuffer = new char[ compileLogLength ];
	glGetProgramInfoLog( _programId, compileLogLength, NULL, errorMessageBuffer );
	OpenGLErrorHandling::checkOpenGL();

	std::string const errorMessage( errorMessageBuffer, compileLogLength );
	delete[] errorMessageBuffer;

	return errorMessage;
}
