#version 330 core
out vec4 color;


uniform vec3 lightPos; 
uniform vec3 lightColor;
uniform vec3 objectColor;
void main()
{
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * lightColor;

    vec3 result = ambient * objectColor;
    color = vec4(result, 1.0f);
} 