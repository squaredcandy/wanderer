#version 430 core
layout(triangles) in;
layout(line_strip, max_vertices = 192) out; // Max Vert is 256

in VS_OUT {
	mat4 MVP;
	vec3 Pos;
	vec3 Nor;
} gs_in[];

void main()
{
	mat4 mvp = gs_in[0].MVP;
	
	for(int i = 0; i < 3; ++i)
	{
		gl_Position = mvp * vec4(gs_in[i].Pos, 1.0f);
		EmitVertex();
		gl_Position = mvp * vec4(gs_in[i].Pos + (normalize(gs_in[i].Nor) * 0.3), 1.0f);
		EmitVertex();
		EndPrimitive();
	}
}