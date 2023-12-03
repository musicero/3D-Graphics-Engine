#include <iostream>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void error_callback(int error, const char* description);

int main(int argc, char **argv){
	
	// GLFW initialization code SECTION 2
	if(!glfwInit()){
		std::cout << "ERROR: glfwInit() failed!" << std::endl;
		return 0;
	}

	glfwSetErrorCallback(error_callback);
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(640, 480, "Mywindow", NULL, NULL);
	if (!window){
		std::cout << "Error: could not create window!" << std::endl;
		return 0;
	}
	
	// OpenGL initialization code SECTION 3
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	float triangleold[3*2] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.0f, 0.5f
	};

	float square[2*3*2] = {
		-0.5f, -0.5f,
		0.5f, -0.5f,
		0.5f, 0.5f,

		-0.5f, -0.5f,
		0.5f, 0.5f,
		-0.5f, 0.5f,
	};
	// openGL data transfer code SECTION 4
	
	
	// Create Vertex Array Object (VAO)
	GLuint vertexArrayId;
	glGenVertexArrays(1, &vertexArrayId);
	glBindVertexArray(vertexArrayId);

	// Generate Vertex Buffer Object (VBO)
	GLuint vertexBufferId;
	glGenBuffers(1, &vertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

	// Transfer data to GPU (Populate the vertex buffer)
	glBufferData(GL_ARRAY_BUFFER, sizeof(square), square, GL_STATIC_DRAW);

	// Set the layout of the bound buffer
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, nullptr);
	glEnableVertexAttribArray(0);

	const std::string vertexShaderSrc = R"(
	#version 430 core
	layout(location = 0) in vec2 position;

	void main(){
		gl_Position = vec4(position, 0.0, 1.0);
	}
	)";

	const std::string fragmentShaderSrc = R"(
	#version 430 core
	
	out vec4 color;
		
	uniform vec4 u_Color;

	void main(){
		color = u_Color;
	}
	)";
	
	// Compile the vertex shader
	auto vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLchar* vss = vertexShaderSrc.c_str();
	glShaderSource(vertexShader, 1, &vss, nullptr);
	glCompileShader(vertexShader);
 
	// Compile the fragment shader
	auto fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	const GLchar* fss = fragmentShaderSrc.c_str();
	glShaderSource(fragmentShader, 1, &fss, nullptr);
	glCompileShader(fragmentShader);

	// Create a shader program
	auto shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);

	glLinkProgram(shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	glUseProgram(shaderProgram);

	// Application loop code SECTION 5
	
	glClearColor(0.5f, 0.0f, 0.0f, 1.0f);
	float t = 0.0f;
	while(!glfwWindowShouldClose(window)){
		glClear(GL_COLOR_BUFFER_BIT);
		auto colorLocation = glGetUniformLocation(shaderProgram, "u_Color");
		
		float color[4] = {1.0f, t, 1.0f, 1.0f};
		glUniform4fv(colorLocation, 1, color);
		
		t += 0.01;
		if(t>=1) t=0;
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glfwSwapBuffers(window);
	}

	// Termination code SECTION 6
	
	glfwTerminate();



	
}
void error_callback(int error, const char* description) {
	fprintf(stderr, "Error: %s\n", description);
}
