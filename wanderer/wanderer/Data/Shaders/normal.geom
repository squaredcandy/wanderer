#version 430 core
layout(triangles) in;
layout(line_strip, max_vertices = 192) out; // Max Vert is 256

in VS_OUT {
	mat4 MVP;
	vec3 Pos;
	vec3 Nor;
	vec3 Tan;
	vec3 BiT;
} gs_in[];

out vec3 color;

void main()
{
	mat4 mvp = gs_in[0].MVP;
	
	for(int i = 0; i < 3; i++)
	{
		// Normal Red
		color = vec3(1,0,0);
		gl_Position = mvp * vec4(gs_in[i].Pos, 1.0f);
		EmitVertex();
		gl_Position = mvp * vec4(gs_in[i].Pos + (normalize(gs_in[i].Nor) * 0.3), 1.0f);
		EmitVertex();
		EndPrimitive();

		// Tangent Blue
		color = vec3(0,0,1);
		gl_Position = mvp * vec4(gs_in[i].Pos, 1.0f);
		EmitVertex();
		gl_Position = mvp * vec4(gs_in[i].Pos + (normalize(gs_in[i].Tan) * 0.3), 1.0f);
		EmitVertex();
		EndPrimitive();
		
		// Bitangent Green
		color = vec3(0,1,0);
		gl_Position = mvp * vec4(gs_in[i].Pos, 1.0f);
		EmitVertex();
		gl_Position = mvp * vec4(gs_in[i].Pos + (normalize(gs_in[i].BiT) * 0.3), 1.0f);
		EmitVertex();
		EndPrimitive();
	}
}