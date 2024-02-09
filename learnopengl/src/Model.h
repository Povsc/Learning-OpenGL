#pragma once
#include <json/json.h>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include <iostream>

using json = nlohmann::json;

class Model
{
public:
	Model(const char *file) {
		// create json
		std::string text = readFile(file);
		JSON = json::parse(text);

		// get bin data
		this->file = file;
		data = getData();

		// begin recurse
		traverseNode(0);
	}

	void Draw(Shader &shader) {
		for (Mesh mesh : meshes) {
			mesh.Draw(shader);
		}
	}

private:
	const char* file;
	json JSON;
	std::vector<unsigned char> data;

	std::vector<Mesh> meshes;

	void loadMesh(unsigned int indMesh, glm::mat4 matrix) {
		// get accessor indices
		unsigned int posAcc = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
		unsigned int texAcc = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"]; // what happens when I need more UVs?
		unsigned int indAcc = JSON["meshes"][indMesh]["primitives"][0]["indices"];

		// use indices to get all components
		std::vector<float> flPos = getFloats(posAcc);
		std::vector<float> flTex = getFloats(texAcc);
		

		// group components
		std::vector<glm::vec3> positions = groupFloatsVec3(flPos);
		std::vector<glm::vec2> texCoords = groupFloatsVec2(flTex);

		// combine into Mesh
		std::vector<unsigned int> indices = getIndices(indAcc);
		std::vector<Vertex> vertices = assembleVertices(positions, texCoords);

		// make meshes
		meshes.push_back(Mesh(vertices, indices, matrix));
	}

	void traverseNode(unsigned int nextNode, glm::mat4 matrix = glm::mat4(1.0f)) {
		// current node
		json node = JSON["nodes"][nextNode];

		// get matrix if it exists
		glm::mat4 matNode = glm::mat4(1.0f);
		if (node.find("matrix") != node.end()) {
			float matValues[16];
			for (int i = 0; i < node["matrix"].size(); i++) {
				matValues[i] = node["matrix"][i];
				matNode = glm::make_mat4(matValues);
			}
		}

		// TODO: do same for translation/rotation/scale

		glm::mat4 matNextNode = matrix * matNode;

		// load mesh if it exists
		if (node.find("mesh") != node.end()) {
			loadMesh(node["mesh"], matNextNode);
		}

		// traverse through children if they exist
		if (node.find("children") != node.end()) {
			for (unsigned int i = 0; i < node["children"].size(); i++) {
				traverseNode(node["children"][i], matNextNode);
			}
		}
	}

	std::vector<unsigned char> getData() {
		// place to store raw text
		std::string bytesText;
		std::string uri = JSON["buffers"][0]["uri"];

		// store raw text
		std::string fileStr = std::string(file);
		std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);
		bytesText = readFile((fileDirectory + uri).c_str());

		// transform raw text data into bytes and put in vector
		std::vector<unsigned char> data(bytesText.begin(), bytesText.end());
		return data;
	}

	std::vector<float> getFloats(unsigned int accessorID) {
		std::vector<float> floats;

		// Get properties from accessor
		unsigned int buffViewInd = JSON["accessors"][accessorID].value("bufferView", 0);
		unsigned int byteOffset = JSON["accessors"][accessorID].value("byteOffset", 0);
		unsigned int count = JSON["accessors"][accessorID]["count"];
		std::string type = JSON["accessors"][accessorID]["type"];

		// Get properties from the bufferview if it exists
		if (buffViewInd != 0) {
			json bufferView = JSON["bufferViews"][buffViewInd];
			byteOffset += bufferView["byteOffset"];
		}

		// Interpret type
		unsigned int dim;
		if (type == "SCALAR") dim = 1;
		else if (type == "VEC2") dim = 2;
		else if (type == "VEC3") dim = 3;
		else if (type == "VEC4") dim = 4;
		else throw std::invalid_argument("INVALID TYPE: must be scalar, vec2, vec3, or vec4");

		// Get bytes from data
		unsigned int length = count * 4 * dim;
		for (int i = byteOffset; i < byteOffset + length; i) {
			unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] }; // little-endian?
			float val;
			std::memcpy(&val, bytes, sizeof(float));
			floats.push_back(val);
		}

		return floats;
	}

	std::vector<unsigned int> getIndices(unsigned int accessorID) {
		std::vector<unsigned int> indices;

		// Get properties from accessor
		unsigned int buffViewInd = JSON["accessors"][accessorID].value("bufferView", 0);
		unsigned int byteOffset = JSON["accessors"][accessorID].value("byteOffset", 0);
		unsigned int count = JSON["accessors"][accessorID]["count"];
		unsigned int type = JSON["accessors"][accessorID]["componentType"];
		// componentTypes can be: 5125 -> uint; 5123 -> ushort; 5122 -> short

		// Get properties from the bufferview if it exists
		if (buffViewInd != 0) {
			json bufferView = JSON["bufferViews"][buffViewInd];
			byteOffset += bufferView["byteOffset"];
		}

		switch (type) {
		case 5125: //unsigned int 
			for (int i = byteOffset; i < byteOffset + count * 4; i)
			{
				unsigned char bytes[] = { data[i++], data[i++], data[i++], data[i++] }; // little-endian?
				unsigned int val;
				std::memcpy(&val, bytes, sizeof(unsigned int));
				indices.push_back(val);
			}
			break;
		case 5123:
			for (int i = byteOffset; i < byteOffset + count * 2; i)
			{
				unsigned char bytes[] = { data[i++], data[i++] }; // little-endian?
				unsigned short val;
				std::memcpy(&val, bytes, sizeof(unsigned short));
				indices.push_back(val);
			}
			break;
		case 5122:
			for (int i = byteOffset; i < byteOffset + count * 2; i)
			{
				unsigned char bytes[] = { data[i++], data[i++] }; // little-endian?
				short val;
				std::memcpy(&val, bytes, sizeof(short));
				indices.push_back(val);
			}
			break;
		default:
			throw std::invalid_argument("INVALID TYPE: must be uint, ushort, or short");
		}

		return indices;
	}

	// TODO: Accessor for textures

	std::vector<Vertex> assembleVertices(std::vector<glm::vec3> position, std::vector < glm::vec2> TexCoords) {
		std::vector<Vertex> vertices;
		Vertex vertex;

		for (int i = 0; i < position.size(); i++) {
			vertex.Position = position[i];
			vertex.TexCoords = TexCoords[i];
			vertices.push_back(vertex);
		}

		return vertices;
	}

	std::string readFile(const char *file) {
		std::ifstream is;

		// ensure ifstream can throw exceptions
		is.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try {
			is.open(file, std::ios::binary);
			std::stringstream tstream;
			// read file's buffer contents into streams
			tstream << is.rdbuf();
			// close file handlers
			is.close();
			// stream -> string
			return tstream.str();
		}
		catch (std::ifstream::failure e) {
			std::cout << "ERROR::MODEL_LOADING::FILE_NOT_SUCCESSFULLY_READ\n";
		}
	}

	std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floats) {
		std::vector<glm::vec2> vectors;

		for (int i = 0; i < floats.size(); i) {
			vectors.push_back(glm::vec2(floats[i++], floats[i++]));
		}
		
		return vectors;
	}

	std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floats) {
		std::vector<glm::vec3> vectors;

		for (int i = 0; i < floats.size(); i) {
			vectors.push_back(glm::vec3(floats[i++], floats[i++], floats[i++]));
		}

		return vectors;
	}
};