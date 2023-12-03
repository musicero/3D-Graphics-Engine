#ifndef GLFWAPPLICATION_H
#define GLFWAPPLICATION_H

#include <string> //unsure if this is needed...

class GLFWwindow;
class GLFWApplication {
public:
	const std::string name;
	const std::string version;
	GLFWwindow* window;

	GLFWApplication(const std::string &name, const std::string &version);

	virtual unsigned Init(); 
	virtual unsigned Run() = 0; 
	
};

#endif
