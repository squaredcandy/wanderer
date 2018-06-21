#version 430 core
layout(triangles, equal_spacing, ccw) in;

in vec3 Pos_Tesc_Out[];
in vec3 Nor_Tesc_Out[];
in vec2 Tex_Tesc_Out[];

//out vec3 Pos_Tese_Out;
//out vec3 Nor_Tese_Out;
//out vec2 Tex_Tese_Out;

out VS_OUT {
	mat4 MVP;
	vec3 Pos;
	vec3 Nor;
} vs_out;

uniform mat4 Projection;
uniform mat4 View;
//uniform mat4 Model;

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
	vs_out.MVP = Projection * View;
	vs_out.Pos = interpolate3D(Pos_Tesc_Out[0], Pos_Tesc_Out[1], Pos_Tesc_Out[2]);
	vs_out.Nor = interpolate3D(Nor_Tesc_Out[0], Nor_Tesc_Out[1], Nor_Tesc_Out[2]);
	vs_out.Nor = normalize(vs_out.Nor);
	vec2 Tex_Tese_Out = interpolate2D(Tex_Tesc_Out[0], Tex_Tesc_Out[1], Tex_Tesc_Out[2]);

	float displacement = texture(heightNoise, Tex_Tese_Out).x;
	
	vs_out.Pos += vs_out.Nor * displacement * heightFactor;
	
	gl_Position = Projection * View * vec4(vs_out.Pos, 1.0);
}