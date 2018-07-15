#version 430 core
layout (location = 0) in vec3 Msh_Pos;
layout (location = 1) in vec3 Msh_Normal;
layout (location = 2) in vec2 Msh_TexCoord;
layout (location = 3) in vec3 Msh_Tangent;
layout (location = 4) in vec3 Msh_Bitangent;
layout (location = 5) in mat4 Model;

out vec2 oldTexCoords;
out vec3 WorldPos;
out vec3 Normal;

out vec3 TangentViewPos;
out vec3 TangentFragPos;

uniform mat4 projection;
uniform mat4 view;
uniform vec3 camPos;

void main()
{
    oldTexCoords = Msh_TexCoord;
    WorldPos = vec3(Model * vec4(Msh_Pos, 1.0));
    Normal = mat3(Model) * Msh_Normal;   

	vec3 T = normalize(mat3(Model) * Msh_Tangent);
    vec3 B = normalize(mat3(Model) * Msh_Bitangent);
    vec3 N = normalize(mat3(Model) * Msh_Normal);
    mat3 TBN = transpose(mat3(T, B, N));

	TangentViewPos = TBN * camPos;
	TangentFragPos = TBN * WorldPos;

    gl_Position =  projection * view * vec4(WorldPos, 1.0);
}