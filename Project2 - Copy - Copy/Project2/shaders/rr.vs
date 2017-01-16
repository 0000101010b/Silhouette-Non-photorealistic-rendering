#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 Position;

out vec3 Reflect;
out vec3 Refract;
out vec3 RefractR,RefractG,RefractB;

out float Ratio;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



const float Eta = 0.66; // Ratio of indices of refraction


const float FresnelPower = 5.0;

const float EtaR=0.2;
const float EtaG=0.4;//Ratio of indices of refraction
const float ETaB=0.8;

const float F=((1.0- EtaG)*(1.0- EtaG))/((1.0+EtaG)*(1.0+EtaG));


void main()
{

	vec4 ecPosition= view*vec4(position, 1.0f);
	vec3 ecPosition3=ecPosition.xyz/ecPosition.w;

	vec3 i=normalize(ecPosition3);
	vec3 n=normalize(model * view * vec4(normal,1.0f)).xyz;

	Ratio = F + (1.0 - F) * pow((1.0 - dot(-i, n)), FresnelPower);
	
	RefractG = refract(i, n, EtaG);
	RefractB = refract(i, n,EtaG);
	RefractR = refract(i, n,EtaR);
	
	Reflect = reflect(i, n);
	
    gl_Position = projection * view * model * vec4(position, 1.0f);
    

	//Normal = mat3(transpose(inverse(model))) * normal;
    Position = vec3(model * vec4(position, 1.0f));
}  