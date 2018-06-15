#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 192) out; // Max Vert is 256

in VS_OUT {
	vec2 TexCoords;
	mat4 MVP;
	vec3 Pos;
} gs_in[];

out vec2 TexCoords;

uniform sampler2D heightNoise;
uniform float tVal;

const float heightScale = 1.f;

vec4 GetHeight(int index)
{
	vec4 height = texture(heightNoise, gs_in[index].TexCoords);
	height.g = height.r;
	height.r = 0;
	return height;
}

float Lerp(float a, float b, float t)
{
	return (1 - t) * a + t * b;
}

vec4 Lerp4(vec4 a, vec4 b, float t)
{
	return vec4(
		Lerp(a.r, b.r, t),
		Lerp(a.g, b.g, t),
		Lerp(a.b, b.b, t),
		Lerp(a.a, b.a, t)
	);
}

void main()
{
	//if(gl_Back

	mat4 mvp = gs_in[0].MVP;
	
	vec3 pos1 = gs_in[0].Pos;
	vec3 pos2 = gs_in[1].Pos;
	vec3 pos3 = gs_in[2].Pos;
	
	//for(int i = 0; i < 3; ++i)
	//{
	//	gl_Position = mvp * vec4(gs_in[i].Pos, 1.0f);
	//	TexCoords = gs_in[i].TexCoords;
	//	EmitVertex();
	//}

	EndPrimitive();
	float size = 0.625f;
	vec3 triangle[] = {vec3(0,0,0), vec3(size,0,0), vec3(size,size,0)}; 

	for(int k = 0; k < 1; ++k)
	{
		for(int i = 0; i < 16; ++i)
		{
			vec3 offset = vec3(i * size, k * size, 0);
			for(int j = 0; j < 3; ++j)
			{
				gl_Position = mvp * vec4(pos1 - triangle[j] - offset, 1.0f);
				TexCoords = gs_in[0].TexCoords;
				EmitVertex();
			}

			EndPrimitive();
		}
	}
}