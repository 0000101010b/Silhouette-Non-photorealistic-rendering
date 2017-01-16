#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 Normal;
out vec3 FragPos;
out vec3 randColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// Random Generation by JeGX april 27 2011
// http://www.ozone3d.net/blogs/lab/20110427/glsl-random-generator/
float rand(vec2 n)
{
  return 0.5 + 0.5 * 
     fract(sin(dot(n.xy, vec2(12.9898, 78.233)))* 43758.5453);
}

void main()
{
	vec2 seed=vec3(model * vec4(position, 1.0f)).xy;
	vec2 seed2=vec3(model * vec4(position, 1.0f)).zy;
	vec2 seed3=vec3(model * vec4(position, 1.0f)).xz;
	float r=rand(seed);
	float r2=rand(seed2);
	float r3=rand(seed3);
	
	vec3 pos=vec3(r,r2,r3) * 5 + position - vec3(5,5,5);
    gl_Position = (projection * view *  model * vec4(pos, 1.0f));
    FragPos = vec3(model * vec4(position, 1.0f));
    Normal = mat3(transpose(inverse(model))) * normal ;
	
	seed=position.xy;
	seed2=position.zy;
	seed3=position.xz;
	r=rand(seed);
	r2=rand(seed2);
	r3=rand(seed3);
	
	randColor=vec3(r,r2,r3);

	
} 