#version 130

in float a_relWidth;
out float v_relWidth;
uniform bool bendingActivated;
uniform bool isReflecting;

void mainDeform();

void main()
{
	// this is the relative height of the fence between 0 and 1
	v_relWidth = a_relWidth;
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	if (bendingActivated && !isReflecting)
		mainDeform();
	return;
}