#version 430 core
layout(triangles, equal_spacing, ccw) in;

in vec3 Pos_Tesc_Out[];
in vec3 Nor_Tesc_Out[];
in vec2 Tex_Tesc_Out[];

out vec3 Pos_Tese_Out;
out vec3 Nor_Tese_Out;
out vec2 Tex_Tese_Out;

uniform mat4 Projection;
uniform mat4 View;

uniform sampler2D heightNoise;
uniform float heightFactor;

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
   	return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
   	return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	Pos_Tese_Out = interpolate3D(Pos_Tesc_Out[0], Pos_Tesc_Out[1], Pos_Tesc_Out[2]);
	Nor_Tese_Out = interpolate3D(Nor_Tesc_Out[0], Nor_Tesc_Out[1], Nor_Tesc_Out[2]);
	Nor_Tese_Out = normalize(Nor_Tese_Out);
	Tex_Tese_Out = interpolate2D(Tex_Tesc_Out[0], Tex_Tesc_Out[1], Tex_Tesc_Out[2]);

	const vec2 size = vec2(0, heightFactor);
	const ivec3 off = ivec3(-1, 0, 1);

	float d11 = texture(heightNoise, Tex_Tese_Out).x;

	float d01 = textureOffset(heightNoise, Tex_Tese_Out, off.xy).x;
	float d21 = textureOffset(heightNoise, Tex_Tese_Out, off.zy).x;
	float d10 = textureOffset(heightNoise, Tex_Tese_Out, off.yx).x;
	float d12 = textureOffset(heightNoise, Tex_Tese_Out, off.yz).x;

	vec3 a = Nor_Tese_Out * heightFactor;

	vec3 b = Pos_Tese_Out + (a * d11);
	vec3 c = Pos_Tese_Out + (a * d01);
	vec3 d = Pos_Tese_Out + (a * d21);
	vec3 e = Pos_Tese_Out + (a * d10);
	vec3 f = Pos_Tese_Out + (a * d12);

	Pos_Tese_Out = b;
	Nor_Tese_Out = normalize(cross(d - c, f - e));

	//Pos_Tese_Out += Nor_Tese_Out * d11 * heightFactor;


	gl_Position = Projection * View * vec4(Pos_Tese_Out, 1.0);
}