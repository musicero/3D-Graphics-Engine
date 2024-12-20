#include <string>

const std::string cubeFragmentShader = R"(
	#version 430 core

	layout(binding = 1) uniform samplerCube uTexture;

	in vec3 vs_position;
  in vec4 vs_normal;
  in vec4 vs_normal_model;
  in vec4 vs_fragPosition;
  
	out vec4 color;
		
	uniform int color_choice;

  uniform float u_ambientStrength = 1.0;
  uniform vec3 u_ambientColor;

  uniform vec3 u_lightSourcePosition;
  uniform float u_diffuseStrength;
  uniform vec3 u_diffuseColor;

  uniform vec3 u_cameraPosition; 
  uniform float u_specularStrength = 0.5;
  uniform vec3 u_specularColor;

	void main(){
		vec4 m_color;
		if(color_choice == 1){
			m_color = vec4(1.0, 0.0, 0.0, 1.0);
		} else if(color_choice==2){
			m_color = vec4(0.0, 1.0, 0.0, 1.0);
		} else if(color_choice==3){
			m_color = vec4(0.0, 0.0, 1.0, 1.0);
		}
		color = mix(m_color, texture(uTexture, vs_position), 0.7);

    vec3 lightDirection = normalize(vec3(u_lightSourcePosition - vs_fragPosition.xyz));
    float diffuseStrength = max(dot(lightDirection, vs_normal.xyz), 0.0f) * u_diffuseStrength;

    vec3 reflectedLight = normalize(reflect(-lightDirection, vs_normal.xyz));
    vec3 observerDirection = normalize(u_cameraPosition - vs_fragPosition.xyz);
    float specFactor = pow(max(dot(observerDirection, reflectedLight), 0.0), 12);
    
    vec3 diffuse = color.rgb * vec3(diffuseStrength)*u_diffuseColor;
    vec3 ambient = color.rgb * vec3(u_ambientStrength)*u_ambientColor;
    vec3 specular = vec3(specFactor * u_specularStrength)* u_specularColor;
    

    //color = vec4(diffuse + ambient + specular, 1.0);
    color = vec4(diffuse, 1.0);
    //color = vec4(specular, 1.0); //only render with specular lighting

	}
)";
