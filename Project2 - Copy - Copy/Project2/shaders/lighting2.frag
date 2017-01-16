#version 330 core
out vec4 color;
//toon shader

in vec3 Normal;
in vec3 randColor;
  
uniform vec3 lightPos; 
uniform vec3 objectColor;
uniform vec3 lightColor;

//eddit version of http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/toon-shader-version-ii/ 

void main()
{
	float intensity;
	vec4 temp;
	vec3 n = Normal;
	intensity = dot(lightPos,normalize(n));
	
	vec3 tempColor=(objectColor+lightColor+randColor)/3;
	
	if (intensity > 0.95)
		temp = vec4(tempColor,1.0);
	else if (intensity > 0.75)
		temp = vec4(tempColor/1.2,1.0);
	else if (intensity > 0.5)
		temp = vec4(tempColor/1.5,1.0);
	else
		temp = vec4(tempColor/2,1.0);
	
	color=temp;
	
}
