#include <GLFW/glfw3.h>
#include "Camera.h"

namespace {
	constexpr unsigned int DEFAULT_WIDTH = 800;
	constexpr unsigned int DEFAULT_HEIGHT = 600;
}

class Window {
public:
	Window(std::shared_ptr<Camera> camera, unsigned int width = DEFAULT_WIDTH, unsigned int height = DEFAULT_HEIGHT) :
		camera_(camera),
		width(width),
		height(height),
		lastX_(0.),
		lastY_(0.),
		firstMouse_(true),
		error_(false),
		lastFrame(0.)
	{
		// Initialize window object
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window_ = glfwCreateWindow(width, height, "LearnOpenGL", NULL, NULL);
		if (window_ == NULL) {
			std::cout << "FAILED TO CREATE GLFW WINDOW\n";
			glfwTerminate();
			error_ = true;
			return;
		}
		glfwMakeContextCurrent(window_);

		// annoying way to get around the callback signatures
		glfwSetWindowUserPointer(window_, this);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
			std::cout << "FAILED TO INITIALIZE GLAD\n";
			error_ = true;
			return;
		}

		// Actually open window
		glViewport(0, 0, width, height);
		glfwSetFramebufferSizeCallback(window_, framebuffer_size_callback);
		glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPosCallback(window_, mouse_callback);
		glfwSetScrollCallback(window_, scroll_callback);
	}

	~Window() {
		glfwTerminate();
	}

	void processInput() {
		float deltaTime = getDeltaTime();

		if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window_, true);

		if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS)
			camera_->ProcessKeyboard(Movement::FORWARD, deltaTime);
		if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS)
			camera_->ProcessKeyboard(Movement::BACKWARD, deltaTime);
		if (glfwGetKey(window_, GLFW_KEY_Q) == GLFW_PRESS)
			camera_->ProcessKeyboard(Movement::DOWN, deltaTime);
		if (glfwGetKey(window_, GLFW_KEY_E) == GLFW_PRESS)
			camera_->ProcessKeyboard(Movement::UP, deltaTime);
		if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS)
			camera_->ProcessKeyboard(Movement::LEFT, deltaTime);
		if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS)
			camera_->ProcessKeyboard(Movement::RIGHT, deltaTime);

		// TODO: what was this about again?
		if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
			// do stuff
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPosCallback(window_, mouse_callback);
		}
		else {
			glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursorPosCallback(window_, nullptr);
			firstMouse_ = true;
		}
	}

	bool shouldClose() {
		return error_ || glfwWindowShouldClose(window_);
	}

	void swapBuffers() {
		// swap buffers and check and call events
		glfwSwapBuffers(window_);
		glfwPollEvents();

	}

	float getTime() {
		return glfwGetTime();
	}

	float getDeltaTime() {
		float currentFrame = glfwGetTime();
		float deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		return deltaTime;
	}

	unsigned int width;
	unsigned int height;



private:
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
		glViewport(0, 0, width, height);

		// get member instance (i.e. this) from the GLFWwindow object
		auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (instance) {
			instance->height = height;
			instance->width = width;

		}

		// updates frames while resizing window requires multithreading
	}

	static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
		// get member instance (i.e. this) from the GLFWwindow object
		auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (instance) {
			if (instance->firstMouse_) {
				instance->lastX_ = xpos;
				instance->lastY_ = ypos;
				instance->firstMouse_ = false;
			}
			float xOffset = (xpos - instance->lastX_);
			float yOffset = (instance->lastY_ - ypos);
			instance->lastX_ = xpos;
			instance->lastY_ = ypos;
			instance->camera_->ProcessMouseMovement(xOffset, yOffset);
		}
	}

	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
		// get member instance (i.e. this) from the GLFWwindow object
		auto* instance = static_cast<Window*>(glfwGetWindowUserPointer(window));
		if (instance) {
			instance->camera_->ProcessMouseScroll(yoffset);
		}
	}

	GLFWwindow* window_;
	std::shared_ptr<Camera> camera_;
	float lastX_, lastY_; // why even initialize?
	bool firstMouse_;
	bool error_;
	float lastFrame;
};