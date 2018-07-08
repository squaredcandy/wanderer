#version 430 core
out vec4 FragColor;

in vec3 Pos_Tese_Out;
in vec3 Nor_Tese_Out;
in vec2 Tex_Tese_Out;

in vec4 FragPosLightSpace;

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

	// Specular
	float specular;
	float specularPower;
};

uniform vec3 Wld_Eye_Pos;

uniform bool drawGridLines;
uniform float gridLineWidth;

uniform int chunkSize;

uniform DirectionalLight dLight;

uniform sampler2D heightNoise;
uniform sampler2D shadowMap;


vec3 water		= vec3(0.059, 0.369, 0.612);
vec3 sand		= vec3(0.914, 0.867, 0.780);
vec3 grass		= vec3(0.663, 0.8,   0.643);
vec3 mountain	= vec3(0.733, 0.733, 0.733);
vec3 snow		= vec3(0.973, 0.973, 0.973);

vec3 calcDiffuseColor()
{
	vec3 color = vec3(0,0,0);
	vec4 noise = texture(heightNoise, Tex_Tese_Out);
	if(noise.r < 0.005)	  discard;  //color = water;
	else if(noise.r < 0.1) color = sand;
	else if(noise.r < 0.5) color = grass;
	else if(noise.r < 0.7) color = mountain;
	else if(noise.r < 2.0) color = snow;
	return color;
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's 
	// perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}

void main()
{
	// Grid Lines
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
	
	// Ambient Color
	vec3 ambientColor = dLight.ambient * dLight.aIntensity;
	
	// Diffuse Color
	float diffuseFactor = dot(normalize(Nor_Tese_Out), dLight.direction);
	vec3 diffuseColor;
	if(diffuseFactor > 0)
	{
		diffuseColor = dLight.ambient * dLight.dIntensity * diffuseFactor;
	}
	else
	{
		diffuseColor = vec3(0,0,0);
	}

	// Specular color
	vec3 viewDir = normalize(Wld_Eye_Pos - Pos_Tese_Out);
	vec3 halfwayDir = normalize(dLight.direction + viewDir);
	float spec = pow(max(dot(Nor_Tese_Out, halfwayDir), 0.0), 64.0);
	vec3 specular = spec * vec3(1.0);

	// shadowMap
	float shadow = ShadowCalculation(FragPosLightSpace);
	vec3 totalColor = (ambientColor + (1.0 - shadow) * (specular + diffuseColor)) * calcDiffuseColor();

	FragColor = vec4(totalColor, 1.0);
}