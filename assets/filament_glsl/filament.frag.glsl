#version 330 core

in float T;

out vec4 FragColor;

uniform float PI;

void main()
{
	vec3 pink = vec3(1.0, 0.41, 0.71);
	vec3 blue = vec3(0.42, 0.35, 0.80);

	float factor = pow(sin(PI * T), 0.45);
	vec3 color = mix(pink, blue, factor);
	FragColor = vec4(color, 1.0);
}
