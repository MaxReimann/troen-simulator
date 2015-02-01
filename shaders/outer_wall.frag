#version 130

uniform sampler2D colorTex;
in vec2 uv;
uniform int modelID;
uniform float glowIntensity;
uniform int levelSize;

in vec3 theNormal;
in float linearDepth;

void main()
{
	int inverseFrequency = 100;

	float modifier = 2;
	float smoothingFactorX = abs(mod((5000 / modifier * uv.x), inverseFrequency / modifier) - 4.8);
	//some magic numbers, adjusted to the wall height
	float smoothingFactorY = abs(mod((4300 / modifier * uv.y), inverseFrequency / 16.0) -0.7);
	gl_FragColor = mix(
		vec4(22, 115, 131, 100) / 255.f,
		vec4(1, 1, 1, 0) / 255.f,
		pow(min(smoothingFactorX, smoothingFactorY), 0.5));

	return;
}