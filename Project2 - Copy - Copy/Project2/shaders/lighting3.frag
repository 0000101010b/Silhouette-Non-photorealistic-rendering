#version 330 core
//Blinn-Phong Shader
//edited version of blinn-phong http://learnopengl.com/#!Advanced-Lighting/Advanced-Lighting 
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec3 randColor;

uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 objectColor;


uniform bool blinn;

void main()
{      
    
	// Ambient
    vec3 ambient = 0.5 * lightColor;
    
	// Diffuse
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    
	float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    
	float spec = 0.0;
	
    if(blinn)
    {
        vec3 halfwayDir = normalize(lightDir + viewDir);  
        spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    }
    else
    {
        reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0), 8.0);
    }
    
	vec3 specular = vec3(0.3) * spec *lightColor; // assuming bright white light color
    FragColor = vec4((ambient + diffuse + specular) * (objectColor+ randColor)/2, 1f);
}