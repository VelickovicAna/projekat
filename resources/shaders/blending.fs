#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture1;

void main()
{
    vec4 texColor = texture(texture1, TexCoords);
    if(texColor.a < 0.4)
        discard;
    FragColor = texColor;
    //FragColor=vec4(1.0, 0.0, 0.0, 1.0);
}