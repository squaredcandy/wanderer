#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D heightNoise;

void main()
{
	vec4 noise = texture(heightNoise, TexCoords);
	FragColor = vec4(noise.r, noise.r, noise.r, 1.0);
}