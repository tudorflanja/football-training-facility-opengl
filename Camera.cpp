#include "Camera.hpp"

namespace gps {

	//Camera constructor
	Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUpDirection) {
		//TODO

		this->cameraPosition = cameraPosition;
		this->cameraTarget = cameraTarget;
		this->cameraFrontDirection = glm::normalize(cameraPosition - cameraTarget); //cameraDirection
		this->cameraRightDirection = glm::normalize(glm::cross(cameraUpDirection, cameraFrontDirection));
		this->cameraUpDirection = glm::cross(cameraFrontDirection, cameraRightDirection);

	}

	//return the view matrix, using the glm::lookAt() function
	glm::mat4 Camera::getViewMatrix() {
		//TODO

	   //return glm::mat4();
		return glm::lookAt(cameraPosition, cameraPosition + cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f));

	}

	glm::vec3 Camera::getCameraPosition() {
		//TODO
		return this->cameraPosition;

	}

	glm::vec3 Camera::getCameraTarget()
	{
		return cameraTarget;
	}

	//update the camera internal parameters following a camera move event
	void Camera::move(MOVE_DIRECTION direction, float speed) {
		//TODO
		if (direction == MOVE_LEFT) {
			this->cameraPosition += speed * this->cameraRightDirection;
			this->cameraTarget += speed * this->cameraRightDirection;
		}
		else if (direction == MOVE_RIGHT) {
			this->cameraPosition -= speed * this->cameraRightDirection;
			this->cameraTarget -= speed * this->cameraRightDirection;
		}
		else if (direction == MOVE_BACKWARD) {
			this->cameraPosition -= speed * this->cameraFrontDirection;
			this->cameraTarget -= speed * this->cameraFrontDirection;
		}
		else if (direction == MOVE_FORWARD) {
			this->cameraPosition += speed * this->cameraFrontDirection;
			this->cameraTarget += speed * this->cameraFrontDirection;
		}
		
	}

	// update the camera internal parameters following a camera rotate event
	// yaw - camera rotation around the Y axis
	// pitch - camera rotation around the X axis
	void Camera::rotate(float pitch, float yaw) {
		//TODO
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		cameraFrontDirection = glm::normalize(front);
	}

	void Camera::scenePreview(float angle) {
		// set the camera
		this->cameraPosition = glm::vec3(-9.0, 600.0, 225.0);

		// rotate with specific angle around Y axis
		glm::mat4 r = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));

		// compute the new position of the camera 
		// previous position * rotation matrix
		this->cameraPosition = glm::vec4(r * glm::vec4(this->cameraPosition, 1));
		this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
		cameraRightDirection = glm::normalize(glm::cross(cameraFrontDirection, glm::vec3(0.0f, 1.0f, 0.0f)));
	}
}