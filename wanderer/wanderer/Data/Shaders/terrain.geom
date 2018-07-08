#version 430 core
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 Pos_Tese_Out[3];
in vec3 Nor_Tese_Out[3];
in vec2 Tex_Tese_Out[3];

out vec3 Pos_Geom_Out;
out vec3 Nor_Geom_Out;
out vec2 Tex_Geom_Out;


void main()
{
	for(int i = 0; i < 3; i++)
	{
		gl_Position = gl_in[i].gl_Position;
		Pos_Geom_Out = Pos_Tese_Out[i];
		Nor_Geom_Out = Nor_Tese_Out[i];
		Tex_Geom_Out = Tex_Tese_Out[i];
		EmitVertex;
	}
	EndPrimitive;
}