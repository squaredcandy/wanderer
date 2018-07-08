#version 430 core
layout(triangles, equal_spacing, ccw) in;

in vec3 Pos_Tesc_Out[];
in vec3 Nor_Tesc_Out[];
in vec2 Tex_Tesc_Out[];

out VS_OUT {
	mat4 MVP;
	vec3 Pos;
	vec3 Nor;
	vec3 Tan;
	vec3 BiT;
} vs_out;

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

float height(vec2 coord, ivec2 offset)
{
	float displacment = textureOffset(heightNoise, coord, offset).x;
	return displacment;
}

void main()
{
	vs_out.MVP = Projection * View;
	vs_out.Pos = interpolate3D(Pos_Tesc_Out[0], Pos_Tesc_Out[1], Pos_Tesc_Out[2]);
	vec2 Tex_Tese_Out = interpolate2D(Tex_Tesc_Out[0], Tex_Tesc_Out[1], Tex_Tesc_Out[2]);

	const int offset = 1;
	const ivec3 off = ivec3(offset, 0, -offset);
	
	float h0 = height(Tex_Tese_Out, off.zz);
	float h1 = height(Tex_Tese_Out, off.yz);
	float h2 = height(Tex_Tese_Out, off.xz);
	float h3 = height(Tex_Tese_Out, off.zy);
	float h4 = height(Tex_Tese_Out, off.yy);
	float h5 = height(Tex_Tese_Out, off.xy);
	float h6 = height(Tex_Tese_Out, off.zx);
	float h7 = height(Tex_Tese_Out, off.yx);
	float h8 = height(Tex_Tese_Out, off.xx);
	
	vec3 normal;
	float c = 2 * (h5 - h3);
	float d = 2 * (h7 - h1);
	normal.x = heightFactor * -(h2 - h0 + c + h8 - h6);
	normal.y = heightFactor * -(h6 - h0 + d + h8 - h2);
	normal.z = 1.0;
	vs_out.Nor = normalize(normal * 2 - 1);

	vec3 a = cross(vs_out.Nor, vec3(0,0,1));
	vec3 b = cross(vs_out.Nor, vec3(0,1,0));
	vs_out.Tan = normalize(a + b);
	vs_out.BiT = normalize(cross(vs_out.Tan, vs_out.Nor));

	vs_out.Pos += vec3(0,1,0) * h4 * heightFactor;

	gl_Position = Projection * View * vec4(vs_out.Pos, 1.0);
}