#include <string>

const std::string cubeVertexShader = R"(
	#version 430 core

	layout(location = 0) in vec3 i_position;
	layout(location = 1) in vec2 i_tcoords;
  layout(location = 2) in vec3 i_normal;

	out vec2 vs_tcoords;
	out vec3 vs_position;
  out vec4 vs_normal;
  out vec4 vs_normal_model;
  out vec4 vs_fragPosition;

	uniform mat4 u_Model;
	uniform mat4 u_ViewProjection;
  uniform mat4 u_Rotation;

	void main(){
		gl_Position = u_ViewProjection * u_Model * vec4(i_position, 1.0);

		vs_tcoords = i_tcoords;
		vs_position = i_position;
    vs_normal_model = normalize(u_Model    * vec4(i_normal, 1.0));
    vs_normal = normalize(u_Rotation * vec4(i_normal, 1.0));
    vs_fragPosition = u_Model * vec4(i_position, 1.0);
	}
)";
