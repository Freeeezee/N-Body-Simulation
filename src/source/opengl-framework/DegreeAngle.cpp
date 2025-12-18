#include "DegreeAngle.hpp"
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>

static GLfloat const INTERVAL_MINIMUM = 0.0f;
static GLfloat const INTERVAL_MAXIMUM = 360.0f;

/**
	Constructor.
	Initializes the degree angle with the
	passed angle.

	@param _initialAngle	The initial degree angle.
*/
DegreeAngle::DegreeAngle( GLfloat const & _initialAngle ) :
	m_degreeAngle( _initialAngle )
{
	restoreInterval();
}

/**
	Assignment-operator.
	Copies the angle value of the passed angle.

	@param _newAngle	The new angle.
*/
void DegreeAngle::operator=( DegreeAngle const & _newAngle )
{
	m_degreeAngle = _newAngle.toFloat();
}

/**
	Assignment-operator.
	Calls the set method and passes the _newAngle.

	@param _newAngle	The new angle.
*/
void DegreeAngle::operator=( GLfloat const & _newAngle )
{
	m_degreeAngle = _newAngle;
	restoreInterval();
}

/**
	Plus operator.
	Adds the passed angle to this angle and returns the
	result. This angle will not be modified.
	The passed angle must be in degree format.

	@param	_angle	The angle to add.
	@return			Returns the result.
*/
DegreeAngle DegreeAngle::operator+( DegreeAngle const & _angle ) const
{
	return DegreeAngle( toFloat() + _angle.toFloat() );
}

/**
	Minus operator.
	Substracts the passed angle from this angle and returns the
	result. This angle will not be modified.
	The passed angle must be in degree format.

	@param	_angle	The angle to add.
	@return			Returns the result.
*/
DegreeAngle DegreeAngle::operator-( DegreeAngle const & _angle ) const
{
	return DegreeAngle( toFloat() - _angle.toFloat() );
}

/**
	@return		Returns the value of this angle.
*/
GLfloat DegreeAngle::toFloat() const
{
	return m_degreeAngle;
}

/**
	Converts the value of this angle to radian form
	and returns the result.

	@return		The radian value of this angle.
*/
GLfloat DegreeAngle::toRadian() const
{
	GLfloat pi = static_cast<GLfloat>( M_PI );
	return m_degreeAngle / 180.0f * pi;
}

/**
	If the angle is < 0 or >= 360, the function
	modifies the angle to match the interval
	between 0 and 360.
*/
void DegreeAngle::restoreInterval()
{
	if( std::abs( m_degreeAngle ) >= INTERVAL_MAXIMUM )
	{
		GLint multiplicator = static_cast<GLint>( m_degreeAngle / INTERVAL_MAXIMUM );
		m_degreeAngle -= multiplicator * INTERVAL_MAXIMUM;
	}

	else if( m_degreeAngle < INTERVAL_MINIMUM )
	{
		m_degreeAngle += INTERVAL_MAXIMUM;
	}
}