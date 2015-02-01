#version 130

uniform vec3 fenceColor;
uniform int modelID;
uniform float glowIntensity;
uniform float fadeOutFactor;

in float v_relWidth;

void main()
{
	float mixFactor = (0.5 - abs(v_relWidth - 0.5)) / 0.5;

	// vec4 color = mix(
		// vec4(fenceColor, 1.0),
		// vec4(fenceColor, 0.10),
		// mixFactor
	// );
	
	vec4 color = vec4(fenceColor, (1.0-(v_relWidth / 0.5 )) * 0.7);

	color.w *= fadeOutFactor;

	gl_FragColor = color;

	return;
}