#version 430 core
layout (location = 0) in vec3 Msh_Pos;
layout (location = 1) in vec3 Msh_Normal;
layout (location = 2) in vec2 Msh_TexCoord;
layout (location = 3) in vec3 Msh_Tangent;
layout (location = 4) in vec3 Msh_Bitangent;
layout (location = 5) in mat4 Model;

out vec2 texCoord;

uniform mat4 Projection;
uniform mat4 View;


void main()
{
	texCoord = Msh_TexCoord;
	gl_Position = Projection * View * Model * vec4(Msh_Pos, 1.0);
}