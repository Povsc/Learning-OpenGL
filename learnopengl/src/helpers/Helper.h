#pragma once
#include <vector>
#include <glm/glm.hpp>

// TODO: think of a file name + namespace

std::vector<glm::vec2> groupFloatsVec2(const std::vector<float>& floats) {
	std::vector<glm::vec2> vectors;

	for (int i = 0; i < floats.size(); i += 2) {
		vectors.emplace_back(floats[i], floats[i + 1]);
	}

	return vectors;
}

std::vector<glm::vec3> groupFloatsVec3(const std::vector<float>& floats) {
	std::vector<glm::vec3> vectors;

	for (int i = 0; i < floats.size(); i += 3) {
		vectors.emplace_back(floats[i], floats[i + 1], floats[i + 2]);
	}

	return vectors;
}

