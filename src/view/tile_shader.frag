#version 400 core
out vec4 FragColor;

in vec2 TexCoord;

// texture samplers
uniform sampler2D tileTexture;
uniform vec4 backgroundColor;

void main()
{

	vec4 texColor = texture(tileTexture, TexCoord);
    if(texColor.a < 0.1){
        FragColor = vec4(backgroundColor);
    } else {
        FragColor = texColor;
    }
}

