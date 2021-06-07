#include "InitGLFW.h"

InitGLFW::InitGLFW(const std::string& windowName, const int& width, const int& height)
{
	//Initiate GLFW
	glfwInit();

	//set glfw to not work with opengl
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	try {

		this->window = initWIndow(windowName, width, height);

	}
	catch (const std::runtime_error& e) {
		printf("GLFW ERROR: %s\n", e.what());
		exit(999);

	}
}

GLFWwindow* InitGLFW::initWIndow(const std::string& windowName, const int& width, const int& height)
{
	GLFWwindow* window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
	if (!window) {
		throw std::runtime_error("Faile to Create GLFW Window!!! ");
	}

}

