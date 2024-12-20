#include <string>

const std::string chessFragmentShader = R"(
	#version 430 core

	layout(binding=0) uniform sampler2D u_floorTextureSampler;

	in vec2 position2;
	in vec2 vs_tcoords;
  in vec4 vs_normal;
  in vec4 vs_fragPosition;
		
  out vec4 color;

	uniform ivec2 selector;	
  uniform float u_ambientStrength = 1.0;
  uniform float u_diffuseStrength;
  uniform vec3 u_lightSourcePosition;
  uniform vec3 u_diffuseColor;

  uniform vec3 u_cameraPosition; 
  uniform float u_specularStrength = 0.5;
  uniform vec3 u_specularColor;

	void main(){
		vec4 m_color;
		ivec2 tileIndex = ivec2(floor(position2 * 8));

		if((tileIndex.x + tileIndex.y) % 2 == 0){
			 m_color = vec4(0.0, 0.0, 0.0, 1.0);
		} else {
			 m_color = vec4(1.0, 1.0, 1.0, 1.0);
		}

		color = mix(m_color, texture(u_floorTextureSampler, vs_tcoords), 0.7);
    color = vec4(u_ambientStrength*color.rgb, color.w);

    vec3 lightDirection = normalize(vec3(u_lightSourcePosition - vs_fragPosition.xyz));
    float diffuseStrength = max(dot(lightDirection, vs_normal.xyz), 0.0f) * u_diffuseStrength;

    vec3 reflectedLight = normalize(reflect(-lightDirection, vs_normal.xyz));
    vec3 observerDirection = normalize(u_cameraPosition - vs_fragPosition.xyz);
    float specFactor = pow(max(dot(observerDirection, reflectedLight), 0.0), 12);
    
    vec3 diffuse = vec3(diffuseStrength)* u_diffuseColor;
    vec3 ambient = vec3(u_ambientStrength);
    vec3 specular = vec3(specFactor * u_specularStrength) * u_specularColor;


    color = vec4(color.rgb * (ambient + diffuse + specular).rgb, 1.0);
	}
)";
