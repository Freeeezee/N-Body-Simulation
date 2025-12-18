#pragma once

#include <GL/glew.h>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include "CameraType.hpp"
#include "DegreeAngle.hpp"

/**
	Represents a simple camera with its own
	coordinate system defined by three axes.

	The axes are defined by three euler angles.
	The initial value for each angle is 0.0f.
	This produces a vector system with
	- view direction = 0, 0, -1
	- up direction = 0, 1, 0

	The camera is in right-handed coordinate system.

	The implementation uses a person specific point of view.
	The yaw rotation is always around the initial up direction.
*/
class Camera
{
public:
	Camera( CameraType const & _cameraType,
			glm::vec4 const & _eyePoint = glm::vec4( 0.0f, 0.0f, 0.0f, 1.0f ),
			glm::vec4 const & _viewDirection = glm::vec4( 0.0f, 0.0f, -1.0f, 0.0f ),
			glm::vec4 const & _upDirection = glm::vec4( 0.0f, 1.0f, 0.0f, 0.0f ) );

	void setEyePoint( glm::vec4 const & _eyePoint );
	void translateEyePoint( glm::vec4 const & _translation );

	void setDistancePlanes( GLfloat const & _near, GLfloat const & _far );
	void setWindowSize( GLfloat const & _width, GLfloat const & _height );

	void rotatePitch( DegreeAngle const & _rotation );
	void rotateYaw( DegreeAngle const & _rotation );
	void resetAngles();

	CameraType const type() const;

	glm::vec4 const eyePoint() const;
	glm::vec4 const viewDirection() const;
	glm::vec4 const upDirection() const;
	glm::vec4 const horizontalDirection() const;

	GLfloat const leftClippingPlane() const;
	GLfloat const rightClippingPlane() const;
	GLfloat const bottomClippingPlane() const;
	GLfloat const topClippingPlane() const;
	GLfloat const nearClippingPlane() const;
	GLfloat const farClippingPlane() const;

	glm::mat4x4 const viewMatrix() const;
	glm::mat4x4 const projectionMatrix() const;

private:
	CameraType const m_type;		//!< The camera type.

	GLfloat m_windowWidth;			//!< The width of the window. Equal to the projection plane width.
	GLfloat m_windowHeight;			//!< The height of the window. Equal to the projection plane height.
	GLfloat m_nearClippingPlane;	//!< The eye point to near plane distance.
	GLfloat m_farClippingPlane;		//!< The eye point to far plane distance.

	DegreeAngle m_pitch;			//!< The pitch angle of the rotation.
	DegreeAngle m_yaw;				//!< The yaw angle of the rotation.
	glm::vec4 m_eyePoint;			//!< The current eye point.
	glm::vec4 const m_viewDirection;//!< The view direction.
	glm::vec4 const m_upDirection;	//!< The up direction.
};