#pragma once
#include <GLFW/glfw3.h>
#include <string>
#include <stdexcept>


class InitGLFW
{
public:
	// Constructor
	InitGLFW(const std::string& windowName, const int& width, const int& height);

	// Utility
	// - Get
	GLFWwindow* getWindow();

	// - Set


private:

	// Variables
	GLFWwindow* window;

	// Create Method
	GLFWwindow* initWIndow(const std::string& windowName, const int& width, const int& height);

	// Callback Function
	void scrollCallback(GLFWwindow* w, double x, double y);
	void keyCallback(GLFWwindow* w, int key, int scancode, int action, int mode);
	void mouseButtonCallback(GLFWwindow* w, int button, int action, int mode);
	void cursorPosCallback(GLFWwindow* w, double xp, double yp);
	void framebufferSizeCallback(GLFWwindow* w, int width, int height);

	

};

