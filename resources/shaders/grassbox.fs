#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube grassbox;

void main()
{    
    FragColor = texture(grassbox, TexCoords) * 0.4f;
}
