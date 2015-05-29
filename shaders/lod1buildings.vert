#version 130

out vec2 uv;
uniform bool bendingActivated;
uniform bool distanceBasedAlpha;
uniform mat4 osg_ViewMatrixInverse;

void mainDeform(vec4);
void mainDeformReflected();


out float scaled_height;
out float alphaModifier;

void main(void)
{
	uv =  gl_MultiTexCoord0.xy;
	scaled_height = gl_Vertex.z;
	bool k = bendingActivated;

	gl_Position = gl_ModelViewProjectionMatrix  * gl_Vertex;

	if ( distanceBasedAlpha)
	{
		alphaModifier = clamp(sqrt(gl_Position.z) / 15.0,0,1);
	}
	else
	{
		alphaModifier = 1.0;
	}

	//osg::Vec3 eye, lookAt, up;
            //m_player->cameras()->at(NAVIGATION_WINDOW)->getViewMatrixAsLookAt(eye, lookAt, up);
            //auto nodePos = node->asTransform()->asMatrixTransform()->getMatrix().getTrans();
            //float distance = (eye - nodePos).length();
            //float alpha = min(1, sqrt(distance / 10.0));)

	if (bendingActivated) {
		mainDeform(gl_Vertex);}
	
	return;

}