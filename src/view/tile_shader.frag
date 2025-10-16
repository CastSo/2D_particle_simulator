#version 400 core
out vec4 FragColor;
in vec2 TexCoord;
uniform vec4 backgroundColor;

uniform bool isSelected;
uniform sampler2D mainTexture;
uniform sampler2D selectTexture;

void main()
{


	vec4 texColor = texture(mainTexture, TexCoord);
    vec4 mixTexColor = mix(texture(selectTexture, TexCoord), texture(mainTexture, TexCoord),  0.5);
    
    if(texColor.a < 0.1){
        if(backgroundColor.a < 0.1)
        {
            discard;
        }else {

            FragColor = vec4(backgroundColor);
        }
    } else {
        if (!isSelected)
        {
            FragColor = texColor; 
        } else {
            FragColor = mixTexColor;
        }
        
    }
}

