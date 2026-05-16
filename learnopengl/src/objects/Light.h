#pragma once
#pragma once
#include "Object.h"

class Light : public Object
{
public:
	Light(std::shared_ptr<Shader> shader,
		glm::vec3 color = glm::vec3(1.),
		glm::vec3 pos = glm::vec3(0.),
		glm::vec3 scale = glm::vec3(1.),
		glm::quat rot = glm::quat(1., 0., 0., 0.)) :
		Object(std::move(pos), std::move(scale), std::move(rot), shader),
		color(color) {
	}

	glm::vec3 color;
};