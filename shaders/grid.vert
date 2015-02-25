#version 130

uniform vec2 levelSize;
uniform float objectID;
uniform vec2 nearFar;
uniform bool bendingActivated;

void mainDeform(vec4);

out vec2 uv;
out vec3 vertex_objCoords;
out vec4 bendedVertex;

void main()
{

	uv = gl_Vertex.xy / levelSize+ vec2(0.5, 0.5);
	//citymodel specific tranlation..no idea why, in blender it looks right
	//uv.x -= 4.0 / levelSize.x;
	//uv.y += 4.39 / levelSize.y;

	vertex_objCoords = gl_Vertex.xyz;

	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex ;
	
	if (bendingActivated)
		mainDeform(gl_Vertex);

	bendedVertex = gl_Position;

	return;
}