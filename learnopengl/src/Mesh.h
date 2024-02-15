#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector> // why?
#include <string>
#include "Shader.h"

struct Vertex
{
	glm::vec3 Position;
	glm::vec2 TexCoords;
	glm::vec3 Normal;
	// Probably adding these later on
	// 
	//glm::vec3 Tangent;
	//glm::vec3 Bitangent;
	//bone stuff for animation?
};

struct Texture
{
	unsigned int id;
	std::string filepath;
	std::string type; // Provisory types: diffuse, specular, normal, height
};

class Mesh // TODO: make destructor to delete VBO/VAO/EBO? Good idea?
{
public:
	// properties
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;
	glm::mat4 model;

	unsigned int VAO;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures, glm::mat4 model = glm::mat4(1.0f)) {
		this->indices = indices;
		this->textures = textures;
		this->vertices = vertices;
		this->model = model;

		unsigned int VBO, EBO;
		// create VAO/VBO/EBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		// bind them
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// TODO: When to not make static draw?
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// configure VAO
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
		glEnableVertexAttribArray(2);
		// TODO: Add extra vertex info (normals, etc)

		glBindVertexArray(0); // Don't really need to "unbind" this 
	}

	void Draw(Shader &shader, glm::mat4 view, glm::mat4 projection) {
		// TODO: figure out shaders (prob similar to textures)

		// bind appropriate textures abiding by our provisory texture types
		unsigned short diffuseCount = 1;
		unsigned short specularCount = 1;
		unsigned short normalCount = 1;
		unsigned short heightCount = 1;

		for (unsigned short i = 0; i < textures.size(); i++) {
			std::string name = textures[i].type.c_str();
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
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}

		shader.use();
		// precompute MVP
		glm::mat4 MVP = projection * view * this->model;
		shader.setMat4("MVP", MVP);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	}
};