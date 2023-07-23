#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector> // why?

enum Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default values
const float YAW =       -90.0f;
const float PITCH =       0.0f;
const float SPEED =       2.5f;
const float SPEED_MAX = 100.0f;
const float SPEED_MIN =   0.1f;
const float SENSITIVITY = 0.1f;

class Camera {
public:
	glm::vec3 Position;
	glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	float Yaw;
	float Pitch;
	float MovementSpeed = SPEED;
	float MouseSensitivity = SENSITIVITY;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) {
		Position = position;
		WorldUp = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	Camera(float posx, float posy, float posz, float upx, float upy, float upz, float yaw = YAW, float pitch = PITCH) {
		Position = glm::vec3(posx, posy, posz);
		WorldUp = glm::vec3(upx, upy, upz);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(Position, Front + Position, Up);
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {
		float camSpeed = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			Position += camSpeed * Front;
		if (direction == BACKWARD)
			Position -= camSpeed * Front;
		if (direction == RIGHT)
			Position += camSpeed * Right;
		if (direction == LEFT)
			Position -= camSpeed * Right;
		if (direction == UP)
			Position += camSpeed * Up; // Should I make this worldUp?
		if (direction == DOWN)
			Position -= camSpeed * Up; // Should I make this worldUp?
	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool constraintPitch = true) {
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;
		Yaw += xoffset;
		Pitch += yoffset;

		if (constraintPitch) {
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset) {
		MovementSpeed += yoffset;
		if (MovementSpeed > SPEED_MAX)
			MovementSpeed = SPEED_MAX;
		if (MovementSpeed < SPEED_MIN)
			MovementSpeed = SPEED_MIN;
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		Front = glm::normalize(front);
		Right = glm::normalize(glm::cross(Front, WorldUp));
		Up = glm::normalize(glm::cross(Right, Front));
	}
};