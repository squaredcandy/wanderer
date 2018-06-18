#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 tePosition[3];

void main()
{
	gl_Position = gl_in[0].gl_Position;
	EmitVertex;

	gl_Position = gl_in[1].gl_Position;
	EmitVertex;

	gl_Position = gl_in[2].gl_Position;
	EmitVertex;

	EndPrimitive;
}