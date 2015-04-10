#version 130

out vec2 uv;
uniform bool bendingActivated;

void mainDeform(vec4);
void mainDeformReflected();


out float scaled_height;

void main(void)
{
	uv =  gl_MultiTexCoord0.xy;
	scaled_height = gl_Vertex.z;
	bool k = bendingActivated;

	gl_Position = gl_ModelViewProjectionMatrix  * gl_Vertex;

	if (bendingActivated) {
		mainDeform(gl_Vertex);}
	
	return;

}