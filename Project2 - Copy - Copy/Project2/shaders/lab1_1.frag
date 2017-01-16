#version 330 core
out vec4 color;

in vec3 FragPos;  
in vec3 Normal;  
in vec3 randColor;

uniform vec3 lightPos; 
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform int SpecularValue;
uniform float ambientStrength;
uniform float specularStrength;

void main()
{
    // Ambient
    vec3 ambient = ambientStrength * lightColor;
  	
    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
	
    float diff = max(dot(lightDir,norm), 0.0);
    vec3 diffuse = diff * lightColor;
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(viewPos - FragPos);
	
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), SpecularValue);

    vec3 specular = specularStrength * spec * lightColor;  
        
    vec3 result = (ambient + diffuse + specular);
	
	//color=vec4(result,1.0f);
	//float r = rand(FragPos.xz);
	
	color=vec4((randColor+result)/2,1.0f);
	//color = vec4( r,r,r, 1.0 );


}

