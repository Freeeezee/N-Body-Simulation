#include "Camera.hpp"
#include <algorithm>
#define _USE_MATH_DEFINES
#define GLM_ENABLE_EXPERIMENTAL
#include <math.h>
#include <iostream>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/trigonometric.hpp>

#include "CameraType.hpp"

#define NEAR_PLANE_MINIMUM 1.0f
#define NEAR_FAR_DISTANCE_MINIMUM 1.0f

/**
	Constructor.
	Initializes the initial view and up direction and the eye point.
	The horizontal axis is implicit defined by the cross
	product with view and up.

	The initial pitch and yaw angle is 0.0f.

	The initial window width is 1.0f.
	The initial window height is 1.0f.
	The initial near clipping plane is 0.0f.
	The initial far clipping plane is 1.0f.

	@param	_type			The camera type.
	@param	_eyePoint		The initial eye point. Default is 0.0f, 0.0f, 0.0f.
	@param	_viewDirection	The initial view direction. Default is 0.0f, 0.0f, -1.0f.
	@param	_upDirection	The initial up direction. Default is 0.0f, 1.0f, 0.0f.
*/
Camera::Camera( CameraType const & _type,
				glm::vec4 const & _eyePoint,
				glm::vec4 const & _viewDirection,
				glm::vec4 const & _upDirection ) :
	m_type( _type ),

	m_windowWidth( 1.0f ),
	m_windowHeight( 1.0f ),
	m_nearClippingPlane( NEAR_PLANE_MINIMUM ),
	m_farClippingPlane( NEAR_PLANE_MINIMUM + NEAR_FAR_DISTANCE_MINIMUM ),

	m_pitch( 0.0f ),
	m_yaw( 0.0f ),
	m_eyePoint( _eyePoint ),
	m_viewDirection( _viewDirection ),
	m_upDirection( _upDirection )
{

}

/**
	Changes the eye point of the camera to the passed eye point.

	@param	_eyePoint	The new eye point for the camera.
*/
void Camera::setEyePoint( glm::vec4 const & _eyePoint )
{
	m_eyePoint = glm::vec4( _eyePoint.x, _eyePoint.y, _eyePoint.z, 1.0f );

	std::cout << "Eye point (" << m_eyePoint.x << ", " << m_eyePoint.y << ", ";
	std::cout << m_eyePoint.z << ", " << m_eyePoint.w << " )" << std::endl;
}

/**
	Translates the eye point of the camera with the passed vector.

	@param	_translation	The translation vector of the camera.
*/
void Camera::translateEyePoint( glm::vec4 const & _translation )
{
	setEyePoint( m_eyePoint + _translation );
}

/**
	Sets the near and far clipping plane of the camera.

	The near clipping plane should be >= 0.0f. If not the
	method sets the near clipping plane to 0.0f.
	The far clipping plane should be >= 1 + the near clipping plane.
	If not the method sets the far clipping plane to
	near clipping plane + 1.0f.

	@param	_near	The near clipping plane.
	@param	_far	The far clipping plane.
*/
void Camera::setDistancePlanes( GLfloat const & _near, GLfloat const & _far )
{
	m_nearClippingPlane = std::max( NEAR_PLANE_MINIMUM, _near );
	m_farClippingPlane = std::max( m_nearClippingPlane + NEAR_FAR_DISTANCE_MINIMUM, _far );
}

/**
	Sets the size of the projection plane.
	For this camera model, the projection plane is equal to the
	near plane.

	Uses the max method to set width and height to a minimum of 1.0f.

	If the camera is a perspective camera, the field of view will
	be affected by the width and height in combination with the
	near clipping plane distance.

	@param	_width	The new width for the projection plane.
	@param	_height	The new height for the projection plane.
*/
void Camera::setWindowSize( GLfloat const & _width, GLfloat const & _height )
{
	m_windowWidth = std::max( 1.0f, _width );
	m_windowHeight = std::max( 1.0f, _height );
}

/**
	Adds the passed pitch angle to the cameras' pitch angle.

	@param	_rotation	The additional pitch angle.
*/
void Camera::rotatePitch( DegreeAngle const & _rotation )
{
	m_pitch = m_pitch.toFloat() + _rotation.toFloat();
}

/**
	Adds the passed yaw angle to the cameras' yaw angle.

	@param	_rotation	The additional yaw angle.
*/
void Camera::rotateYaw( DegreeAngle const & _rotation )
{
	m_yaw = m_yaw.toFloat() + _rotation.toFloat();
}

/**
	Resets the rotation angles.
*/
void Camera::resetAngles()
{
	m_pitch = 0.0f;
	m_yaw = 0.0f;
}

/**
	@return	Returns the type of the camera.
*/
CameraType const Camera::type() const
{
	return m_type;
}

/**
	@return	Returns the current eye point.
*/
glm::vec4 const Camera::eyePoint() const
{
	return m_eyePoint;
}

/**
	@return	Returns the current view direction. Is normalized to 1.
			The view system is a right-handed coordinate system.
*/
glm::vec4 const Camera::viewDirection() const
{
	glm::vec3 const upDirection = glm::vec3( m_upDirection );
	glm::vec3 const viewDirection = glm::vec3( m_viewDirection );
	glm::vec3 const horizontalDirection = glm::cross( upDirection, viewDirection );

	glm::quat const yawRotationQuaterion = glm::angleAxis( glm::radians( m_yaw.toFloat() ), upDirection );
	glm::vec3 const rotatedHorizontalDirection = yawRotationQuaterion * horizontalDirection;

	glm::quat const pitchRotationQuaternion = glm::angleAxis( glm::radians( m_pitch.toFloat() ), rotatedHorizontalDirection );
	glm::quat const completeRotationQuaterion = pitchRotationQuaternion * yawRotationQuaterion;

	glm::vec3 const rotated = glm::normalize( completeRotationQuaterion * viewDirection );
	return glm::vec4( rotated, 0.0f );
}

/**
	@return	Returns the current up direction. Is normalized to 1.
			The view system is a right-handed coordinate system.
*/
glm::vec4 const Camera::upDirection() const
{
	glm::vec3 const upDirection = glm::vec3( m_upDirection );
	glm::vec3 const viewDirection = glm::vec3( m_viewDirection );
	glm::vec3 const horizontalDirection = glm::cross( upDirection, viewDirection );

	glm::quat const yawRotation = glm::angleAxis( glm::radians( m_yaw.toFloat() ), upDirection );
	glm::vec3 const rotatedHorizontalDirection = yawRotation * horizontalDirection;
	glm::quat const pitchRotation = glm::angleAxis( glm::radians( m_pitch.toFloat() ), rotatedHorizontalDirection );

	glm::vec3 const rotated = glm::normalize( pitchRotation * upDirection );
	return glm::vec4( rotated, 0.0f );
}

/**
	@return	Returns the horizontal direction of the view system.
			The view system is a right-handed coordinate system.
*/
glm::vec4 const Camera::horizontalDirection() const
{
	glm::vec3 upDirection = glm::vec3( Camera::upDirection() );
	glm::vec3 viewDirection = glm::vec3( Camera::viewDirection() );

	glm::vec3 horizontal = glm::normalize( glm::cross( upDirection, viewDirection ) );
	return glm::vec4( horizontal, 0.0f );
}

/**
	@return	Returns the left clipping plane.
*/
GLfloat const Camera::leftClippingPlane() const
{
	return -m_windowWidth / 2.0f;
}

/**
	@return	Returns the right clipping plane.
*/
GLfloat const Camera::rightClippingPlane() const
{
	return m_windowWidth / 2.0f;
}

/**
	@return	Returns the bottom clipping plane.
*/
GLfloat const Camera::bottomClippingPlane() const
{
	return -m_windowHeight / 2.0f;
}

/**
	@return	Returns the top clipping plane.
*/
GLfloat const Camera::topClippingPlane() const
{
	return m_windowHeight / 2.0f;
}

/**
	@return	Returns the current near clipping plane.
*/
GLfloat const Camera::nearClippingPlane() const
{
	return m_nearClippingPlane;
}

/**
	@return	Returns the current far clipping plane.
*/
GLfloat const Camera::farClippingPlane() const
{
	return m_farClippingPlane;
}

/**
	Calculates the view matrix of the camera.
	The view matrix can transform each point or vector from the world
	system to the view system of this camera.

	@return	Returns the view matrix.
*/
glm::mat4x4 const Camera::viewMatrix() const
{
	glm::vec3 const eyePoint = glm::vec3( Camera::eyePoint() );
	glm::vec3 const viewDirection = glm::vec3( Camera::viewDirection() );
	glm::vec3 const upDirection = glm::vec3( Camera::upDirection() );

	return glm::lookAt( eyePoint, eyePoint + viewDirection, upDirection );
}

/**
	Calculates the projection matrix for this camera.

	Depending on the camera type, the method calculates
	a perspective frustum matrix or an orthographic frustum
	matrix.

	@return	Returns the projection matrix.
*/
glm::mat4x4 const Camera::projectionMatrix() const
{
	if( CameraType::PERSPECTIVE == type() )
	{
		return glm::perspectiveFov( 90.0f, m_windowWidth, m_windowHeight, nearClippingPlane(), farClippingPlane() );
	}

	else
	{
		return glm::ortho( leftClippingPlane(),
						   rightClippingPlane(),
						   bottomClippingPlane(),
						   topClippingPlane(),
						   nearClippingPlane(),
						   farClippingPlane() );
	}
}