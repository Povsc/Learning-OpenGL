#pragma once
#include <json/json.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>


#include "Mesh.h"
#include <iostream>

using json = nlohmann::json;

struct TexToLoad {
	unsigned int texID;
	const char* type;
};

class Model
{
public:
	Model(const char* directory) {
		// create json
		this->directory = directory;
		std::string fileStr = directory + std::string("scene.gltf");
		const char* file = fileStr.c_str();
		std::string text = readFile(file);
		JSON = json::parse(text);

		// get bin data
		data = getData();

		// begin recurse
		traverseNode(0);
	}

	void Draw(Shader& shader, glm::mat4 view, glm::mat4 projection) {
		for (Mesh mesh : meshes) {
			mesh.Draw(shader, view, projection);
		}
	}

private:
	const char* directory;
	json JSON;
	std::vector<unsigned char> data;
	std::vector<Mesh> meshes;
	std::vector<Texture> texturesLoaded;

	void loadMesh(unsigned int indMesh, glm::mat4 matrix) {
		// get accessor indices
		unsigned int posAcc = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["POSITION"];
		unsigned int texAcc = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["TEXCOORD_0"]; // what happens when I need more UVs?
		unsigned int indAcc = JSON["meshes"][indMesh]["primitives"][0]["indices"];
		unsigned int norAcc = JSON["meshes"][indMesh]["primitives"][0]["attributes"]["NORMAL"];
		unsigned int matAcc = JSON["meshes"][indMesh]["primitives"][0].value("material", 0);

		// use indices to get all components
		std::vector<float> flPos = getFloats(posAcc);
		std::vector<float> flTex = getFloats(texAcc);
		std::vector<float> flNor = getFloats(norAcc);
		std::vector<Texture> textures = getTextures(matAcc);

		// group components
		std::vector<glm::vec3> positions = groupFloatsVec3(flPos);
		std::vector<glm::vec2> texCoords = groupFloatsVec2(flTex);
		std::vector<glm::vec3> normals = groupFloatsVec3(flNor);

		// combine into Mesh
		std::vector<unsigned int> indices = getIndices(indAcc);
		std::vector<Vertex> vertices = assembleVertices(positions, texCoords, normals);

		// make meshes
		meshes.push_back(Mesh(vertices, indices, textures, matrix));
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
			}
			matNode = glm::make_mat4(matValues);
		}
		
		// array to store values 
		float values[4];

		//get translation if it exists
		glm::vec3 translation = glm::vec3(0.0f);
		if (node.find("translation") != node.end()) {
			for (unsigned int i = 0; i < node["translation"].size(); i++) {
				values[i] = node["translation"][i];
			}
			translation = glm::make_vec3(values);
		}

		//get scale if it exists
		glm::vec3 scale = glm::vec3(1.0f);
		if (node.find("scale") != node.end()) {
			for (unsigned int i = 0; i < node["scale"].size(); i++) {
				values[i] = node["scale"][i];
			}
			scale = glm::make_vec3(values);
		}

		//get rotation if it exists
		glm::quat quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		if (node.find("rotation") != node.end()) {
			for (unsigned int i = 0; i < node["rotation"].size(); i++) {
				values[(i + 3) % 4] = node["rotation"][i]; // mod stuff to account for diff order of components
			}
			quaternion = glm::make_quat(values);
		}

		// initialize matrices
		glm::mat4 trans = glm::mat4(1.0f);
		glm::mat4 sca = glm::mat4(1.0f);

		// apply transformations
		trans = glm::translate(trans, translation);
		glm::mat4 rot = glm::mat4_cast(quaternion);
		sca = glm::scale(sca, scale);


		glm::mat4 matNextNode = trans * rot * sca * matrix * matNode;

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
		bytesText = readFile((this->directory + uri).c_str());

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
		else throw std::invalid_argument("INVALID TYPE: must be scalar, vec2, vec3, or vec4\n");

		// Get bytes from data
		unsigned int length = count * 4 * dim;
		for (int i = byteOffset; i < byteOffset + length; i += 4) {
			unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] }; // little-endian?
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

		// componentTypes can be: 5125 -> uint; 5123 -> ushort; 5122 -> short
		unsigned int type = JSON["accessors"][accessorID]["componentType"];

		// Get properties from the bufferview if it exists
		if (buffViewInd != 0) {
			json bufferView = JSON["bufferViews"][buffViewInd];
			byteOffset += bufferView["byteOffset"];
		}

		switch (type) {
		case 5125: // unsigned int 
			for (int i = byteOffset; i < byteOffset + count * 4; i += 4)
			{
				unsigned char bytes[] = { data[i], data[i + 1], data[i + 2], data[i + 3] }; // little-endian?
				unsigned int val;
				std::memcpy(&val, bytes, sizeof(unsigned int));
				indices.push_back(val);
			}
			break;
		case 5123: // unsigned short
			for (int i = byteOffset; i < byteOffset + count * 2; i += 2)
			{
				unsigned char bytes[] = { data[i], data[i + 1] };
				unsigned short val;
				std::memcpy(&val, bytes, sizeof(unsigned short));
				indices.push_back(val);
			}
			break;
		case 5122: // short
			for (int i = byteOffset; i < byteOffset + count * 2; i += 2)
			{
				unsigned char bytes[] = { data[i], data[i + 1] };
				short val;
				std::memcpy(&val, bytes, sizeof(short));
				indices.push_back(val);
			}
			break;
		default:
			throw std::invalid_argument("INVALID TYPE: must be uint, ushort, or short\n");
			break;
		}

		return indices;
	}

	/* TODO: Cover more cases!
	Very incomplete function, only getting base color texture, but can easily expand to also allow for other textures
	(e.g. normal, specular), as well as different TEXCOORD mappings. Also where PBR information is stored. Doesn't make 
	sense to make this too robust right now, but can easily be improved in the future. */
	std::vector<Texture> getTextures(unsigned int accessorID) {
		// store textures/texture info
		std::vector<TexToLoad> toLoad;
		std::vector<Texture> textures;

		// Get Texture IDs
		// base color texture is stored in PBR for some reason
		unsigned int baseColorID = JSON["materials"][accessorID]["pbrMetallicRoughness"]["baseColorTexture"]["index"];
		TexToLoad baseColor;
		baseColor.texID = baseColorID;
		baseColor.type = "diffuse";
		toLoad.push_back(baseColor);

		// TODO: Add more potential texture types

		unsigned int pathID;
		unsigned int sampID;
		for (TexToLoad tex : toLoad) { // right now this loop is silly because we're only retrieving one texture per mesh
			// get image path
			unsigned int pathID = JSON["textures"][tex.texID]["source"];
			int sampID = JSON["textures"][tex.texID].value("sampler", -1);
			std::string path = this->directory + JSON["images"][pathID].value("uri", ""); // not sure why this is preventing from crashing
			//const char* path = pathStr.c_str();

			// check if texture is already loaded
			bool skip = false;
			for (Texture texture : texturesLoaded) {
				if (std::strcmp(path.data(), texture.filepath.data()) == 0) {
					textures.push_back(texture);
					skip = true;
					break;
				}
			}
			if (!skip) { // load it if not
				Texture texture;
				texture.id = loadTexture(path.c_str(), sampID);
				texture.filepath = path;
				texture.type = "diffuse";
				texturesLoaded.push_back(texture);
				textures.push_back(texture);
			}
		}
		return textures;
	}

	unsigned int loadTexture(const char* path, int sampID) {
		GLenum magFilter;
		GLenum minFilter;
		GLenum WrapS;
		GLenum WrapT;

		// get parameters from sampler if sampler ID exists
		if (sampID != -1) {
			// get image wrap/filtering options
			unsigned int iMagFilter = JSON["samplers"][sampID].value("magFilter", 9729);
			unsigned int iMinFilter = JSON["samplers"][sampID].value("minFilter", 9987);
			unsigned int iWrapS = JSON["samplers"][sampID].value("wrapS", 10497);
			unsigned int iWrapT = JSON["samplers"][sampID].value("wrapT", 10497);

			// get magFilter;
			switch (iMagFilter) {
			case 9728:
				magFilter = GL_NEAREST;
				break;
			case 9729:
				magFilter = GL_LINEAR;
				break;
			default:
				throw std::invalid_argument("ERROR: could not retrieve magFilter\n");
				break;
			}

			// get minFilter
			switch (iMinFilter) {
			case 9728:
				minFilter = GL_NEAREST;
				break;
			case 9729:
				minFilter = GL_LINEAR;
				break;
			case 9984:
				minFilter = GL_NEAREST_MIPMAP_NEAREST;
				break;
			case 9985:
				minFilter = GL_LINEAR_MIPMAP_NEAREST;
				break;
			case 9986:
				minFilter = GL_LINEAR_MIPMAP_NEAREST;
				break;
			case 9987:
				minFilter = GL_LINEAR_MIPMAP_LINEAR;
				break;
			default:
				throw std::invalid_argument("ERROR: could not retrieve minFilter\n");
				break;
			}

			// get WrapS
			switch (iWrapS) {
			case 33071:
				WrapS = GL_CLAMP_TO_EDGE;
				break;
			case 33648:
				WrapS = GL_MIRRORED_REPEAT;
				break;
			case 10497:
				WrapS = GL_REPEAT;
				break;
			default:
				throw std::invalid_argument("ERROR: could not retrieve WarpS\n");
				break;
			}

			// get WrapT
			switch (iWrapT) {
			case 33071:
				WrapT = GL_CLAMP_TO_EDGE;
				break;
			case 33648:
				WrapT = GL_MIRRORED_REPEAT;
				break;
			case 10497:
				WrapT = GL_REPEAT;
				break;
			default:
				throw std::invalid_argument("ERROR: could not retrieve WrapT\n");
				break;
			}
		}
		else {
			magFilter = GL_LINEAR;
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			WrapS = GL_REPEAT;
			WrapT = GL_REPEAT;
		}

		// generate texture
		unsigned int ID;
		glGenTextures(1, &ID);

		//stbi_set_flip_vertically_on_load(true); 

		// retrieve data
		int width, height, nrChannels;
		unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
		if (data) {
			GLint channels;
			switch (nrChannels) { // retrieve number of channels
			case 1:
				channels = GL_RED;
				break;
			case 3:
				channels = GL_RGB;
				break;
			case 4:
				channels = GL_RGBA;
				break;
			default:
				throw std::invalid_argument("INVALID NUMBER OF IMAGE CHANNELS: must be 1, 3, or 4\n");
				break;
			}

			// bind texture
			glBindTexture(GL_TEXTURE_2D, ID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			// set texture parameters
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, WrapS);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, WrapT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		}
		else {
			std::cout << "Failed to load texture at path " << path << std::endl;
		}
		// free loaded image
		stbi_image_free(data); // free image memory
		return ID;
	}

	std::vector<Vertex> assembleVertices(std::vector<glm::vec3> position, std::vector < glm::vec2> TexCoords, std::vector<glm::vec3> normal) {
		std::vector<Vertex> vertices;
		Vertex vertex;

		for (int i = 0; i < position.size(); i++) {
			vertex.Position = position[i];
			vertex.TexCoords = TexCoords[i];
			vertex.Normal = normal[i];
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
			std::cout << "ERROR::MODEL_LOADING::FILE_NOT_SUCCESSFULLY_READ\n" << file << std::endl;
		}
	}

	std::vector<glm::vec2> groupFloatsVec2(std::vector<float> floats) {
		std::vector<glm::vec2> vectors;

		for (int i = 0; i < floats.size(); i += 2) {
			vectors.push_back(glm::vec2(floats[i], floats[i + 1]));
		}
		
		return vectors;
	}

	std::vector<glm::vec3> groupFloatsVec3(std::vector<float> floats) {
		std::vector<glm::vec3> vectors;

		for (int i = 0; i < floats.size(); i += 3) {
			vectors.push_back(glm::vec3(floats[i], floats[i + 1], floats[i + 2]));
		}

		return vectors;
	}
};