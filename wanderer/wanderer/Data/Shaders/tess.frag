#version 430 core
out vec4 FragColor;

in vec3 Pos_Tese_Out;
in vec3 Nor_Tese_Out;
in vec2 Tex_Tese_Out;

uniform sampler2D heightNoise;

void main()
{
	vec4 noise = texture(heightNoise, Tex_Tese_Out);
	if(noise.r < 0.005) FragColor =    vec4(0.059, 0.369, 0.612, 1.0);
	else if(noise.r < 0.1) FragColor = vec4(0.914, 0.867, 0.780, 1.0);
	else if(noise.r < 0.5) FragColor = vec4(0.663, 0.8,   0.643, 1.0);
	else if(noise.r < 0.7) FragColor = vec4(0.733, 0.733, 0.733, 1.0);
	else if(noise.r < 2.0) FragColor = vec4(0.973, 0.973, 0.973, 1.0);
	
	
	//FragColor = vec4(noise.r, noise.r, noise.r, 1.f);
}