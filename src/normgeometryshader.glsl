#version 330
layout (triangles) in;
layout (line_strip, max_vertices = 2) out;

in VS_OUT
{
	vec3 normal;
} gs_in[];

const float MAGNITUDE = 1;

uniform mat4 objModel;
uniform mat4 view;
// uniform mat4 lightModel;
uniform mat4 projection;
uniform vec3 lightPos;

void GenerateLine(int index)
{
	vec3 lightPosView = vec3(view*vec4(lightPos,1.0));
	// gl_Position=projection*view*vec4(0,0,0,1);
	gl_Position=gl_in[index].gl_Position;
	EmitVertex();
	// gl_Position=projection*view*vec4(0,1,0,1);
	// gl_Position=gl_in[index].gl_Position;
	gl_Position=projection*view*vec4(lightPos,1);
	// gl_Position=projection*view*normalize(vec4(lightPos,1) - gl_in[index].gl_Position);
	EmitVertex();
	EndPrimitive();
}

void main()
{
	// GenerateLine(0); // first vertex normal
	// GenerateLine(1); // second vertex normal
	// GenerateLine(2); // third vertex normal
}  