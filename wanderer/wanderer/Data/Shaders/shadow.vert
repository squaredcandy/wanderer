#version 430 core
layout (location = 0) in vec3 Msh_Pos;
layout (location = 1) in vec3 Msh_Normal;
layout (location = 2) in vec2 Msh_TexCoord;
layout (location = 3) in vec3 Msh_Tangent;
layout (location = 4) in vec3 Msh_Bitangent;

//uniform mat4 ProjectionView;

out vec3 Pos_Vert_Out;
//out vec3 Nor_Vert_Out;
out vec2 Tex_Vert_Out;

uniform mat4 Model;

void main()
{
	//gl_Position = ProjectionView * Model * vec4(Msh_Pos, 1.0);
	Pos_Vert_Out = (Model * vec4(Msh_Pos, 1.0)).xyz;
	//Nor_Vert_Out = (Model * vec4(Msh_Normal, 1.0)).xyz;
	Tex_Vert_Out = Msh_TexCoord;
}