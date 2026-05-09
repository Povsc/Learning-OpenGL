#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


enum class Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// Default values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float SPEED = 2.5f;
constexpr float SPEED_MAX = 100000.0f;
constexpr float SPEED_MIN = 0.1f;
constexpr float SENSITIVITY = 0.1f;

class Camera {
public:
	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) :
		position_(position),
		worldUp_(up),
		front_(0.0f, 0.0f, -1.0f),
		yaw_(yaw),
		pitch_(pitch),
		movementSpeed_(SPEED),
		mouseSensitivity_(SENSITIVITY) {
		updateCameraVectors();
	}

	Camera(float posx, float posy, float posz, float upx, float upy, float upz, float yaw = YAW, float pitch = PITCH) :
		position_(posx, posy, posz),
		worldUp_(upx, upy, upz),
		front_(0.0f, 0.0f, -1.0f),
		yaw_(yaw),
		pitch_(pitch),
		movementSpeed_(SPEED),
		mouseSensitivity_(SENSITIVITY) {
		updateCameraVectors();
	}

	glm::mat4 GetViewMatrix() {
		return glm::lookAt(position_, front_ + position_, up_);
	}

	void ProcessKeyboard(Movement direction, float deltaTime) {
		float camSpeed = movementSpeed_ * deltaTime;
		if (direction == Movement::FORWARD)
			position_ += camSpeed * front_;
		if (direction == Movement::BACKWARD)
			position_ -= camSpeed * front_;
		if (direction == Movement::RIGHT)
			position_ += camSpeed * right_;
		if (direction == Movement::LEFT)
			position_ -= camSpeed * right_;
		if (direction == Movement::UP)
			position_ += camSpeed * up_;
		if (direction == Movement::DOWN)
			position_ -= camSpeed * up_;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, bool constraintPitch = true) {
		xoffset *= mouseSensitivity_;
		yoffset *= mouseSensitivity_;
		yaw_ += xoffset;
		pitch_ += yoffset;

		if (constraintPitch) {
			if (pitch_ > 89.0f)
				pitch_ = 89.0f;
			if (pitch_ < -89.0f)
				pitch_ = -89.0f;
		}
		updateCameraVectors();
	}

	void ProcessMouseScroll(float yoffset) {
		movementSpeed_ += yoffset;
		if (movementSpeed_ > SPEED_MAX)
			movementSpeed_ = SPEED_MAX;
		if (movementSpeed_ < SPEED_MIN)
			movementSpeed_ = SPEED_MIN;
	}

private:
	void updateCameraVectors() {
		glm::vec3 front;
		front.x = cos(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		front.y = sin(glm::radians(pitch_));
		front.z = sin(glm::radians(yaw_)) * cos(glm::radians(pitch_));
		front_ = glm::normalize(front);
		right_ = glm::normalize(glm::cross(front_, worldUp_));
		up_ = glm::normalize(glm::cross(right_, front_));
	}

	glm::vec3 position_;
	glm::vec3 front_;
	glm::vec3 up_;
	glm::vec3 right_;
	glm::vec3 worldUp_;
	float yaw_;
	float pitch_;
	float movementSpeed_;
	float mouseSensitivity_;

};