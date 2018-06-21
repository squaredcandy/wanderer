#version 430 core
layout(vertices = 3) out;

in vec3 Pos_Vert_Out[];
in vec3 Nor_Vert_Out[];
in vec2 Tex_Vert_Out[];

out vec3 Pos_Tesc_Out[];
out vec3 Nor_Tesc_Out[];
out vec2 Tex_Tesc_Out[];

uniform vec3 Wld_Eye_Pos;

uniform float lodDistance[5];
uniform float tesLevels[5];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

	for(int i = 0; i < 5; ++i)
	{
		if(AvgDistance <= lodDistance[i])
		{
			return tesLevels[i] <= 0 ? 0 : tesLevels[i];
		}
	}
	return 1.0;
}

void main()
{
	Pos_Tesc_Out[gl_InvocationID] = Pos_Vert_Out[gl_InvocationID];
	Nor_Tesc_Out[gl_InvocationID] = Nor_Vert_Out[gl_InvocationID];
	Tex_Tesc_Out[gl_InvocationID] = Tex_Vert_Out[gl_InvocationID];

	float EyeToVertexDist0 = distance(Wld_Eye_Pos, Pos_Tesc_Out[0]);
	float EyeToVertexDist1 = distance(Wld_Eye_Pos, Pos_Tesc_Out[1]);
	float EyeToVertexDist2 = distance(Wld_Eye_Pos, Pos_Tesc_Out[2]);

	gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDist1, EyeToVertexDist2);
	gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDist2, EyeToVertexDist0);
	gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDist0, EyeToVertexDist1);
	gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}