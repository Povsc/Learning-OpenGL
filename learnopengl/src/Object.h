#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Shader.h"


class Object {
public:
	Object(glm::vec3 pos = glm::vec3(0.),
		glm::vec3 scale = glm::vec3(1.),
		glm::quat rot = glm::quat(1., 0., 0., 0.),
		std::shared_ptr<Shader> shader = nullptr) :
		pos_(std::move(pos)),
		scale_(std::move(scale)),
		rot_(std::move(rot)),
		model_(std::make_shared<glm::mat4>(1.0)),
		shader_(shader)
	{
		updateModel();
	}

	virtual ~Object() {};
	virtual void Draw(const glm::mat4& view, const glm::mat4& projection) const = 0;

	void updatePosition(glm::vec3 pos) {
		pos_ = pos;
		updateModel();
	}

	void updateSacle(glm::vec3 scale) {
		scale_ = scale;
		updateModel();
	}

	void updateRotation(glm::quat rot) {
		rot_ = rot;
		updateModel();
	}

	glm::vec3 getPosition() {
		return pos_;
	}

	glm::vec3 getScale() {
		return scale_;
	}

	glm::quat getRotation() {
		return rot_;
	}


protected:

	void updateModel() {
		auto trans = glm::translate(glm::mat4(1.0f), pos_);
		auto rot = glm::mat4_cast(rot_);
		auto sca = glm::scale(glm::mat4(1.0f), scale_);

		*model_ = trans * rot * sca;
	}

	glm::vec3 pos_;
	glm::vec3 scale_;
	glm::quat rot_;
	std::shared_ptr<glm::mat4> model_;
	std::shared_ptr<Shader> shader_;
};