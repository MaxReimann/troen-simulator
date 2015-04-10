#version 130

uniform vec3 fenceColor;
uniform int modelID;
uniform float glowIntensity;
uniform float fadeOutFactor;
uniform bool distanceDependent;
in float distToCamera;
in float v_relWidth;

void main()
{
	if (distanceDependent)
	{
		gl_FragColor = vec4(clamp(1000.0/(distToCamera * distToCamera),0.0,0.9));// color * 1/distToCamera;
		return;
	}

	float mixFactor = (0.5 - abs(v_relWidth - 0.5)) / 0.5;	
	vec4 color = vec4(fenceColor, (1.0-(v_relWidth / 0.5 )) * 0.7);
	color.w *= fadeOutFactor;

	gl_FragColor = color;



	return;
}