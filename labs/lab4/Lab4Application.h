#ifndef LAB4APPLICATION_H
#define LAB4APPLICATION_H

#include "GLFWApplication.h"
#include "glad/glad.h"

#include <string>
class Lab4Application : public GLFWApplication {
public:
	const std::string name;
	const std::string version;

	Lab4Application(const std::string& name, const std::string& version);
	GLuint LoadTextures(const std::string& file, GLuint slot);
	GLuint LoadCubeMap(const std::string& file, GLuint slot);

	unsigned Run();
};

#endif
