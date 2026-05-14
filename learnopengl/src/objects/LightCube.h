#pragma once
#include "Object.h"
#include "../Mesh.h"

class LightCube : public Object
{
public:
	LightCube(std::shared_ptr<Shader> shader,
		glm::vec3 color = glm::vec3(1.),
		glm::vec3 pos = glm::vec3(0.),
		glm::vec3 scale = glm::vec3(1.),
		glm::quat rot = glm::quat(1., 0., 0., 0.)) :
		Object(std::move(pos), std::move(scale), std::move(rot), shader),
		color_(color),
		mesh_(std::move(cubeMesh()))
	{
	}

	void Draw(const glm::mat4& view, const glm::mat4& projection) const override {
		shader_->use();
		shader_->setVec3("color", color_);
		mesh_.Draw(view, projection);
	}

private:
	Mesh cubeMesh() {
		// a lot of hard coded BS, good for now
		std::vector<float> flPos{ //x,  y,  z,
									0.0,0.0,1.0,
									1.0,0.0,1.0,
									1.0,1.0,1.0,
									0.0,1.0,1.0,
									1.0,0.0,0.0,
									0.0,0.0,0.0,
									0.0,1.0,0.0,
									1.0,1.0,0.0,
									0.0,1.0,1.0,
									1.0,1.0,1.0,
									1.0,1.0,0.0,
									0.0,1.0,0.0,
									0.0,0.0,0.0,
									1.0,0.0,0.0,
									1.0,0.0,1.0,
									0.0,0.0,1.0,
									1.0,0.0,1.0,
									1.0,0.0,0.0,
									1.0,1.0,0.0,
									1.0,1.0,1.0,
									0.0,0.0,0.0,
									0.0,0.0,1.0,
									0.0,1.0,1.0,
									0.0,1.0,0.0,

		};
		std::vector<float> flTex{ //u, v,
									0.0,0.0,
									1.0,0.0,
									1.0,1.0,
									0.0,1.0,
									0.0,0.0,
									1.0,0.0,
									1.0,1.0,
									0.0,1.0,
									0.0,0.0,
									1.0,0.0,
									1.0,1.0,
									0.0,1.0,
									0.0,0.0,
									1.0,0.0,
									1.0,1.0,
									0.0,1.0,
									0.0,0.0,
									1.0,0.0,
									1.0,1.0,
									0.0,1.0,
									0.0,0.0,
									1.0,0.0,
									1.0,1.0,
									0.0,1.0,
		};
		std::vector<float> flNor{ //nx, ny, nz,
									0.0,0.0,1.0,
									0.0,0.0,1.0,
									0.0,0.0,1.0,
									0.0,0.0,1.0,
									0.0,0.0,-1.0,
									0.0,0.0,-1.0,
									0.0,0.0,-1.0,
									1.0,1.0,0.0,
									0.0,1.0,0.0,
									0.0,1.0,0.0,
									0.0,1.0,0.0,
									0.0,1.0,0.0,
									0.0,-1.0,0.0,
									0.0,-1.0,0.0,
									0.0,-1.0,0.0,
									0.0,-1.0,0.0,
									1.0,0.0,0.0,
									1.0,0.0,0.0,
									1.0,0.0,0.0,
									1.0,0.0,0.0,
									-1.0,0.0,0.0,
									-1.0,0.0,0.0,
									-1.0,0.0,0.0,
									-1.0,0.0,0.0,
		};

		auto vertices = mesh::assembleVertices(std::move(flPos), std::move(flTex), std::move(flNor));
		std::vector<unsigned int> indices{ //  i0,i1,i2,
												0,1,2,
												0,2,3,
												4,5,6,
												4,6,7,
												8,9,10,
												8,10,11,
												12,13,14,
												12,14,15,
												16,17,18,
												16,18,19,
												20,21,22,
												20,22,23,

		};

		return Mesh(std::move(vertices), std::move(indices), {}, shader_, model_);
	}

	glm::vec3 color_;
	const Mesh mesh_;
};