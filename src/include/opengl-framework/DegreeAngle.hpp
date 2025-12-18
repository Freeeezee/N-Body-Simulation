#pragma once

#include <GL/glew.h>

/**
	This class represents a simple degree angle
	between 0 and 359 degrees.
*/
class DegreeAngle
{
public:
	DegreeAngle( GLfloat const & _initialAngle = 0.0f );

	void operator=( DegreeAngle const & _newAngle );
	void operator=( GLfloat const & _newAngle );

	DegreeAngle operator+( DegreeAngle const & _angle ) const;
	DegreeAngle operator-( DegreeAngle const & _angle ) const;

	GLfloat toFloat() const;
	GLfloat toRadian() const;

private:
	void restoreInterval();

	GLfloat m_degreeAngle;	//!< The degree angle float value.
};