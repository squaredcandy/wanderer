#version 430 core
out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D textureSample;

void main()
{
	vec3 color = texture(textureSample, texCoord).rgb;
	FragColor = vec4(color, 1.0);
}