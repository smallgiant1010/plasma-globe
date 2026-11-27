#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform float emissionIntensity;
uniform float bloomStrength;
uniform vec3 pink;

void main()
{
	vec3 result = pink * emissionIntensity;

	FragColor = vec4(result, 1.0);
	BrightColor = vec4(bloomStrength * result, 1.0);
}
