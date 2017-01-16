#version 330 core
in vec3 Normal;
in vec3 Position;

in vec3 Reflect;

in vec3 RefractR;
in vec3 RefractB;
in vec3 RefractG;

in float Ratio; 

out vec4 FragColor;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             

    //float ratio = 1.00 / 1.52;
  //  vec3 I = normalize(Position - cameraPos);
//	vec3 R = refract(I, normalize(Normal), ratio);
	
	vec3 refractColor;
	refractColor.r = vec3(texture(skybox, RefractR)).r;
	refractColor.g =vec3(texture(skybox, RefractG)).g;
	refractColor.b =vec3(texture(skybox, RefractB)).b;
	
	vec3 reflectColor = vec3(texture(skybox, Reflect));
	vec3 color = mix(refractColor, reflectColor, Ratio);
	FragColor = vec4(color, 1.0);
	
	
	
	
  //  vec4 temp = texture(skybox, R);
   // I = normalize(Position - cameraPos);
    //R = reflect(I, normalize(Normal));
    //FragColorcolor = texture(skybox, R)+temp;
}