#pragma once

#include <string>

#include "Window.hpp"

/**

*/
class OpenGLFramework
{
public:
	OpenGLFramework( std::string const & _vertexShaderFile,
					 std::string const & _fragmentShaderFile );
	~OpenGLFramework();

	void initialize();
	bool isInitialized() const;

	Window * createWindow( std::string const & _title,
						   GLuint const & _pixelWidth,
						   GLuint const & _pixelHeight );

	void shutdown();

private:
	void initializeOpenGL();

	GLuint buildShaderProgram( std::string const & _vertexShaderFile,
							   std::string const & _fragmentShaderFile );

	GLuint buildShader( std::string const & _shaderFile,
						GLuint const & _shaderType );
	void compileShader( GLuint const & _shaderId, std::string const & _shaderCodeString );
	std::string readShaderBuildLog( GLuint const & _shaderId ) const;
	std::string readShaderFile( std::string const _shaderFile ) const;

	GLuint linkShaderProgramm( GLuint const & _vertexShader, GLuint const & _fragmentShader );
	std::string readProgramBuildLog( GLuint const & _programId ) const;

	std::string const m_vertexShaderFile;	//!< The vertex shader file path.
	std::string const m_fragmentShaderFile;	//!< The fragment shader file path.
	GLuint m_shaderProgram;					//!< The shader program to render the triangles.
};