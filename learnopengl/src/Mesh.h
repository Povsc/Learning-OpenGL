#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <string>
#include "Shader.h"

namespace mesh {
	struct Vertex {
		glm::vec3 position;
		glm::vec2 texCoords;
		glm::vec3 normal;
		// Probably adding these later on
		// 
		//glm::vec3 tangent;
		//glm::vec3 bitangent;
		//bone stuff for animation?
	};

	struct Texture {
		unsigned int id; // understand what OpenGL is doing under the hood, how is this cached
		std::string filepath;
		std::string type; // Provisory types: diffuse, specular, normal, height
	};

	class Mesh {
	public:
		Mesh(std::vector<Vertex>&& vertices,
			std::vector<unsigned int>&& indices,
			std::vector<Texture>&& textures,
			std::shared_ptr<glm::mat4> parentModel,
			glm::mat4&& model = glm::mat4(1.0f)) :
			indices_(std::move(indices)),
			textures_(std::move(textures)),
			vertices_(std::move(vertices)),
			model_(std::move(model)),
			parentModel_(parentModel)
		{

			unsigned int VBO, EBO;
			// create VAO/VBO/EBO
			glGenVertexArrays(1, &VAO_);
			glGenBuffers(1, &VBO);
			glGenBuffers(1, &EBO);

			// bind them
			glBindVertexArray(VAO_);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			// TODO: When to not make static draw?
			glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

			// configure VAO
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			glEnableVertexAttribArray(2);
			// TODO: Add extra vertex info (normals, etc)

			glBindVertexArray(0); // Don't really need to "unbind" this 
		}

		void Draw(const Shader& shader, const glm::mat4& view, const glm::mat4& projection) const {
			// bind appropriate textures abiding by our provisory texture types
			unsigned short diffuseCount = 1;
			unsigned short specularCount = 1;
			unsigned short normalCount = 1;
			unsigned short heightCount = 1;

			for (unsigned short i = 0; i < textures_.size(); i++) {
				std::string name = textures_[i].type.c_str();
				std::string number;

				if (name == "diffuse") {
					number = std::to_string(diffuseCount++);
				}
				else if (name == "specular") {
					number = std::to_string(specularCount++);
				}
				else if (name == "normal") {
					number = std::to_string(normalCount++);
				}
				else if (name == "height") {
					number = std::to_string(heightCount++);
				}

				// activate texture
				glActiveTexture(GL_TEXTURE0 + i);

				// now set the sampler to the correct texture unit
				glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), 0);

				// and finally bind the texture
				glBindTexture(GL_TEXTURE_2D, textures_[i].id);
			}

			shader.use();
			// precompute MVP
			glm::mat4 MVP = projection * view * (*parentModel_) * model_;
			shader.setMat4("MVP", MVP);
			glBindVertexArray(VAO_);
			glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, 0);
		}


	private:
		unsigned int VAO_;
		std::vector<mesh::Vertex> vertices_;
		std::vector<unsigned int> indices_;
		std::vector<Texture> textures_;
		glm::mat4 model_;
		std::shared_ptr<glm::mat4> parentModel_;

	};

} // namespace mesh