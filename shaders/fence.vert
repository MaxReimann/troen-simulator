#version 130

in float a_relWidth;
out float v_relWidth;
uniform bool bendingActivated;
uniform bool isReflecting;
uniform vec3 playerPosition;
uniform mat4 osg_ViewMatrixInverse;


void mainDeform(vec4);

vec4 adjustHeight(void)
{
	vec4 worldVert = osg_ViewMatrixInverse * gl_ModelViewMatrix * gl_Vertex; 
	vec3 line = vec3(playerPosition - worldVert.xyz);
	float dist = sqrt(dot(line, line));
	
	float height = pow(dist/300.0, 2);
	height = clamp(height,0.3,80.0);
	//height = (a_relWidth) * height + 1.0 - a_relWidth;
	return gl_Vertex * vec4(1.0,1.0,height,1.0);
}

void main()
{
	// this is the relative height of the fence between 0 and 1
	v_relWidth = a_relWidth;

	
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	

	if (bendingActivated && !isReflecting)
		mainDeform(adjustHeight());
	return;
}