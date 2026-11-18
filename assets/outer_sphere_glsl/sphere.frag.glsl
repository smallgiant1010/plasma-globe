#version 330 core
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	vec3 direction;
};

uniform vec3 F0;
uniform vec3 viewPos;
uniform Light light;
uniform Material material;

vec3 fresnelSchlick(float cosTheta, vec3 f0);

void main()
{
	// ambient
	vec3 ambient = light.ambient * material.ambient;

	// diffuse
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(-light.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// specular
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

	//alpha
	float cosTheta = clamp(abs(dot(norm, viewDir)), 0.0, 1.0);
	float fresnel = fresnelSchlick(cosTheta, F0).r;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, fresnel);
}

vec3 fresnelSchlick(float cosTheta, vec3 f0)
{
	return f0 + (1.0 - f0) * pow(1 - cosTheta, 5.0);
}

