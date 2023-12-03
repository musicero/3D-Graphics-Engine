#ifndef LAB3APPLICATION_H
#define LAB3APPLICATION_H

#include "GLFWApplication.h"

#include <string>
class Lab3Application : public GLFWApplication {
public:
	const std::string name;
	const std::string version;

	Lab3Application(const std::string& name, const std::string& version);


	unsigned Run();
};

#endif
