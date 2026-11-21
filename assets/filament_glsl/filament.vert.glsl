#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTangent;
layout (location = 2) in float aT;

out float T;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform float width;

void main()
{
	T = aT;

	vec3 viewPoint = vec3(view * model * vec4(aPos, 1.0)); // get the position in view space
	vec3 viewDir = normalize(-viewPoint); // use that position to obtain the view direction by taking its vector from origin
	vec3 viewTangent = normalize(mat3(view * model) * aTangent); // get the tangent in view space
	vec3 sideways = normalize(cross(viewTangent, viewDir)); // get the upwards position of the point
	float side = (gl_VertexID % 2 == 0) ? -1.0 : 1.0; // we can figure out whether this is an upper point or lower via parity
	vec3 ribbonPosition = viewPoint + side * sideways * width * 0.5; // R(t) = P(t) +/- w/2 * S(t)

	gl_Position = projection * vec4(ribbonPosition, 1.0);
}
