#include <vector>

#include "Camera.h"
#include "objects/Model.h"
#include "objects/LightCube.h"
#include "Window.h"


// Weird way (?) to force computer to use NVIDIA or AMD dedicated GPU
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

int main() {
	float lastFrame = 0.0f;
	float deltaTime = 0.0f;
	auto camera = std::make_shared<Camera>(std::move(glm::vec3(0.0f, 0.0f, 3.0f)));
	Window window(camera);

	if (window.shouldClose()) {
		std::cout << "Error initializing window;\n";
		return -1;
	}

	// OpenGL global parameters
	glEnable(GL_DEPTH_TEST);

	// temporary way pre-create shaders per object-type
	auto modelShader = std::make_shared<Shader>("shaders/texture.vert", "shaders/texture.frag");
	auto lightShader = std::make_shared<Shader>("shaders/light.vert", "shaders/light.frag");

	// Test models -- TODO: eventually would be good to load models in background
	std::vector<Model> models;
	models.emplace_back("assets/gltf/real-time_bones_demo_phoenix_bird/", modelShader, glm::vec3(-10, 5, 0), glm::vec3(0.01, 0.01, 0.01));
	models.emplace_back("assets/gltf/dusty_old_bookshelf_free/", modelShader);
	models.emplace_back("assets/gltf/survival_guitar_backpack/", modelShader, glm::vec3(-5, 0, 0), glm::vec3(0.01, 0.01, 0.01));

	std::vector<LightCube> lights;
	lights.emplace_back(lightShader, glm::vec3(0.5, 0.8, 1.), glm::vec3(0., -2., 2.));
	lights.emplace_back(lightShader, glm::vec3(0.9, 0.5, 1.), glm::vec3(0., 2., 2.));

	while (!window.shouldClose()) {
		window.processInput();

		// rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera->GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), float(window.width) / float(window.height), 0.10f, 10000.0f);

		//float time = window.getTime();
		//models[0].updatePosition(glm::vec3(-10, 5, 0) + glm::vec3(time, -time, 0));
		//models[1].updateSacle(glm::vec3(fmod(time, 10), fmod(time * 1.5, 10), time));
		//models[2].updateRotation(glm::quat(time * 0.1, time * 0.1, 0, 0));

		for (const auto& light : lights) {
			light.Draw(view, projection);
		}

		for (const auto& model : models) {
			model.Draw(view, projection);
		}

		window.swapBuffers();
	}

	return 0;
}