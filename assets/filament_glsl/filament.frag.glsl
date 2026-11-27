#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in float T;

uniform float PI;
uniform float emissionIntensity;
uniform float bloomStrength;
uniform vec3 pink;

void main()
{
	vec3 blue = vec3(0.03, 0.10, 0.90);

	float factor = pow(sin(PI * T), 0.35);
	vec3 color = mix(pink, blue, factor);

	vec3 result = color * emissionIntensity;

	FragColor = vec4(result, 1.0);
	BrightColor = vec4(bloomStrength * result, 1.0);
}
