#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform float width = 1, height = 1;
uniform int x = 1, y = 0;

uniform sampler2D sprite;

void main()
{
	vec2 scaledTex = vec2((texCoord.x + float(x)) / width, 
						  (texCoord.y + float(y)) / height);
	FragColor = texture(sprite, scaledTex);
}