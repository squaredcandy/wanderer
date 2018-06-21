#version 430 core
out vec4 FragColor;

in vec3 Pos_Tese_Out;
in vec3 Nor_Tese_Out;
in vec2 Tex_Tese_Out;

//in vec3 Pos_Geom_Out;
//in vec3 Nor_Geom_Out;
//in vec2 Tex_Geom_Out;

struct DirectionalLight
{
	// Ambient
	vec3 ambient;
	float aIntensity;

	// Diffuse
	vec3 direction;
	float dIntensity;
};

uniform bool drawGridLines;
uniform float gridLineWidth;

uniform int chunkSize;

uniform DirectionalLight dLight;
uniform sampler2D heightNoise;

vec3 water		= vec3(0.059, 0.369, 0.612);
vec3 sand		= vec3(0.914, 0.867, 0.780);
vec3 grass		= vec3(0.663, 0.8,   0.643);
vec3 mountain	= vec3(0.733, 0.733, 0.733);
vec3 snow		= vec3(0.973, 0.973, 0.973);

vec3 calcDiffuseColor()
{
	vec3 color = vec3(0,0,0);
	vec4 noise = texture(heightNoise, Tex_Tese_Out);
	if(noise.r < 0.005)	   color = water;
	else if(noise.r < 0.1) color = sand;
	else if(noise.r < 0.5) color = grass;
	else if(noise.r < 0.7) color = mountain;
	else if(noise.r < 2.0) color = snow;
	return color;
}

void main()
{
	vec2 coords = Tex_Tese_Out * chunkSize;
	if(drawGridLines)
	{
		if(fract(coords.x) > gridLineWidth ||
		fract(coords.y) > gridLineWidth)
		{
			FragColor = vec4(1,1,1,1);
			return;
		}
	}
	
	vec3 ambientColor = dLight.ambient * dLight.aIntensity;
	
	float diffuseFactor = dot(normalize(Nor_Tese_Out), -dLight.direction);
	vec3 diffuseColor;
	if(diffuseFactor > 0)
	{
		diffuseColor = dLight.ambient * dLight.dIntensity * diffuseFactor;
	}
	else
	{
		diffuseColor = vec3(0,0,0);
	}
	
	vec3 totalColor = calcDiffuseColor() * (ambientColor + diffuseColor);
	FragColor = vec4(totalColor, 1.0);
}