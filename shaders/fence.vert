#version 130

in float a_relWidth;
out float v_relWidth;
uniform bool bendingActivated;
uniform bool isReflecting;
uniform vec3 playerPosition;
uniform mat4 osg_ViewMatrixInverse;

void mainDeform();

void main()
{
	// this is the relative height of the fence between 0 and 1
	v_relWidth = a_relWidth;
	vec4 worldVert = osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex; 
	vec3 line = vec3(playerPosition - worldVert.xyz);
	float dist = sqrt(dot(line, line));
	
	float height = (dist * dist);
	vec4 adj_vert = gl_Vertex + vec4(0.0,0.0,height,0.0);
	
	gl_Position = gl_ModelViewProjectionMatrix * adj_vert;
	

	if (bendingActivated && !isReflecting)
		mainDeform();
	return;
}