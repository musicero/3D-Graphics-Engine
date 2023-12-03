#ifndef LAB2APPLICATION_H
#define LAB2APPLICATION_H

#include "GLFWApplication.h"

#include <string>
class Lab2Application : public GLFWApplication {
public:
	const std::string name;
	const std::string version;

	Lab2Application(const std::string& name, const std::string& version);


	unsigned Run();
};

#endif
