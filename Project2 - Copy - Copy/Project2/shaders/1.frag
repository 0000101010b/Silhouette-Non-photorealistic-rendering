#version 330 core
in vec3 ourColor;
in vec2 TexCoord;

out vec4 color;

uniform sampler2D ourTexture1;
//uniform sampler2D ourTexture2;

void main()
{
	// color = vec4(.2,.2,.2,.1);
	color=texture(ourTexture1, TexCoord);
	//color= vec4(1,1,1,.1);
}