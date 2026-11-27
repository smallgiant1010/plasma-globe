#version 330 core
in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D hdrBuffer;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
	vec3 hdrColor = texture(hdrBuffer, TexCoords).rgb;
	vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
	hdrColor += bloomColor;
	// Exposure HDR Algorithm
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	// gamma correction
	result = pow(result, vec3(1.0 / 2.2));
	FragColor = vec4(result, 1.0);
}
