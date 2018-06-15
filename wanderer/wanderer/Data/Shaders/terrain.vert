#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

out VS_OUT {
	vec2 TexCoords;
	mat4 MVP;
	vec3 Pos;
} vs_out;

void main()
{	
	vs_out.TexCoords = aTexCoord;
	vs_out.MVP = Projection * View * Model;
	vs_out.Pos = aPos;
	gl_Position = vs_out.MVP * vec4(aPos, 1.0f);
}