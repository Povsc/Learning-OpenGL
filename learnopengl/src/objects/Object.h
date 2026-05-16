#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "../Shader.h"


class Object {
public:
	Object(glm::vec3 pos = glm::vec3(0.),
		glm::vec3 scale = glm::vec3(1.),
		glm::quat rot = glm::quat(1., 0., 0., 0.),
		std::shared_ptr<Shader> shader = nullptr) :
		pos(std::move(pos)),
		scale(std::move(scale)),
		rot(std::move(rot)),
		model_(std::make_shared<glm::mat4>(1.0)),
		shader_(shader)
	{
		updateModel();
	}

	virtual ~Object() {};
	virtual void Draw(const glm::mat4& view, const glm::mat4& projection) const = 0;

	void updatePosition(glm::vec3 pos) {
		pos = pos;
		updateModel();
	}

	void updateSacle(glm::vec3 scale) {
		scale = scale;
		updateModel();
	}

	void updateRotation(glm::quat rot) {
		rot = rot;
		updateModel();
	}

	glm::vec3 getPosition() {
		return pos;
	}

	glm::vec3 getScale() {
		return scale;
	}

	glm::quat getRotation() {
		return rot;
	}

	glm::vec3 pos;
	glm::vec3 scale;
	glm::quat rot;

protected:

	void updateModel() {
		auto translationMat = glm::translate(glm::mat4(1.0f), pos);
		auto rotationMat = glm::mat4_cast(rot);
		auto scaleMat = glm::scale(glm::mat4(1.0f), scale);

		*model_ = translationMat * rotationMat * scaleMat;
	}

	std::shared_ptr<glm::mat4> model_;
	std::shared_ptr<Shader> shader_;
};