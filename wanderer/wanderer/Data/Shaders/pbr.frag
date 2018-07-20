#version 430 core
out vec4 FragColor;
in vec2 oldTexCoords;
in vec3 WorldPos;
in vec3 Normal;

in vec3 TangentViewPos;
in vec3 TangentFragPos;

// material parameters
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D heightMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// lights
#define LIGHTS_LEN 32
uniform samplerCube shadowMap[LIGHTS_LEN];
uniform vec3 lightPositions[LIGHTS_LEN];
uniform vec3 lightColors[LIGHTS_LEN];
uniform float far_plane[LIGHTS_LEN];
uniform int lightLength;

uniform float heightScale;
uniform vec3 camPos;
uniform float bias;
uniform int samples;

const float PI = 3.14159265359;

// array of offset direction for sampling
const vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCalculation(vec3 fragPos, int idx)
{
	// We should be using the first light position
	vec3 fragToLight = fragPos - lightPositions[idx];
	float currentDepth = length(fragToLight);

	float shadow = 0;
	float viewDistance = length(camPos - fragPos);
	float diskRadius = (1.0 + (viewDistance / far_plane[idx])) / 25.0;
	for(int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(shadowMap[idx], fragToLight + gridSamplingDisk[i] * diskRadius).r;
		closestDepth *= far_plane[idx];
		if(currentDepth - bias > closestDepth)
		{
			shadow += 1.0;
		}
	}
	shadow /= float(samples);
	return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
	// number of depth layers
	const float minLayers = 4;
	const float maxLayers = 16;
	float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
	// calculate the size of each layer
	float layerDepth = 1.0 / numLayers;
	// depth of current layer
	float currentLayerDepth = 0.0;
	// the amount to shift the texture coordinates per layer (from vector P)
	vec2 P = viewDir.xy / viewDir.z * heightScale; 
	vec2 deltaTexCoords = P / numLayers;
  
	// get initial values
	vec2  currentTexCoords     = texCoords;
	float currentDepthMapValue = texture(heightMap, currentTexCoords).r;
	  
	while(currentLayerDepth < currentDepthMapValue)
	{
		// shift texture coordinates along direction of P
		currentTexCoords -= deltaTexCoords;
		// get depthmap value at current texture coordinates
		currentDepthMapValue = texture(heightMap, currentTexCoords).r;  
		// get depth of next layer
		currentLayerDepth += layerDepth;  
	}
	
	// get texture coordinates before collision (reverse operations)
	vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

	// get depth after and before collision for linear interpolation
	float afterDepth  = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;
 
	// interpolation of texture coordinates
	float weight = afterDepth / (afterDepth - beforeDepth);
	vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

	return finalTexCoords;
}

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal 
// mapping the usual way for performance anways; I do plan make a note of this 
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap(vec2 TexCoords)
{
	vec3 tangentNormal = texture(normalMap, TexCoords).xyz * 2.0 - 1.0;

	vec3 Q1  = dFdx(WorldPos);
	vec3 Q2  = dFdy(WorldPos);
	vec2 st1 = dFdx(TexCoords);
	vec2 st2 = dFdy(TexCoords);

	vec3 N   = normalize(Normal);
	vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B  = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
// ----------------------------------------------------------------------------
void main()
{
	//vec3 viewDir = normalize(TangentViewPos - TangentFragPos);
	//vec2 TexCoords = ParallaxMapping(oldTexCoords, viewDir);
	vec2 TexCoords = oldTexCoords;

//	if(TexCoords.x > 1.0 || TexCoords.y > 1.0 || TexCoords.x < 0.0 || TexCoords.y < 0.0)
//		discard;

	vec3 albedo     = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
	float metallic  = texture(metallicMap, TexCoords).r;
	float roughness = texture(roughnessMap, TexCoords).r;
	float ao        = texture(aoMap, TexCoords).r;

	vec3 N = getNormalFromMap(TexCoords);
	//vec3 N = texture(normalMap, TexCoords).rgb;
	//N = normalize(N * 2 - 1);
	vec3 V = normalize(camPos - WorldPos);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	for(int i = 0; i < lightLength; ++i) 
	{
		// calculate per-light radiance
		vec3 L = normalize(lightPositions[i] - WorldPos);
		vec3 H = normalize(V + L);
		float distance = length(lightPositions[i] - WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = lightColors[i] * attenuation;

		// Cook-Torrance BRDF
		float NDF = DistributionGGX(N, H, roughness);   
		float G   = GeometrySmith(N, V, L, roughness);      
		vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
		   
		vec3 nominator    = NDF * G * F; 
		float denominator = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
		vec3 specular = nominator / denominator;
		
		// kS is equal to Fresnel
		vec3 kS = F;
		// for energy conservation, the diffuse and specular light can't
		// be above 1.0 (unless the surface emits light); to preserve this
		// relationship the diffuse component (kD) should equal 1.0 - kS.
		vec3 kD = vec3(1.0) - kS;
		// multiply kD by the inverse metalness such that only non-metals 
		// have diffuse lighting, or a linear blend if partly metal (pure metals
		// have no diffuse light).
		kD *= 1.0 - metallic;	  

		// scale light by NdotL
		float NdotL = max(dot(N, L), 0.0);        

		// add to outgoing radiance Lo
		Lo += (kD * albedo / PI + specular) * radiance * NdotL;
		// note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
	}   
	
	float totalShadow = 0;
	for(int i = 0; i < lightLength; ++i) 
	{
		totalShadow += ShadowCalculation(WorldPos, i);
	}

	// ambient lighting (note that the next IBL tutorial will replace 
	// this ambient lighting with environment lighting).
	vec3 ambient = vec3(0.03) * albedo * ao;

	vec3 color = ambient + (Lo * (1 - totalShadow / lightLength));
	//vec3 color = ambient + Lo;
	color -= totalShadow * (0.01 / lightLength);

	// Other one we can use
	//vec3 color = ambient + Lo;
	//if(totalShadow / lightLength >= 0.9)
	//{
	//	color = ambient + (Lo * 0.1);
	//}
	//color -= totalShadow * (0.01 / lightLength);

	// HDR tonemapping
	color = color / (color + vec3(1.0));
	// gamma correct
	color = pow(color, vec3(1.0/2.2)); 

	FragColor = vec4(color, 1.0);
}