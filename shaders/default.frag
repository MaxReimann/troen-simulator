#version 130
uniform sampler2D diffuseTexture;
uniform int modelID;
uniform float trueColor;
uniform float alpha;
uniform bool bendingActivated;

in vec2 uv;
in float scaled_height;

void main() {


	vec4 diffuseColor =  vec4(texture(diffuseTexture, uv).xyz, alpha);
	vec4 adjustedColor =  vec4(diffuseColor.x, diffuseColor.x / 2.0, diffuseColor.x, 1.0);

	// decide whether to use the original or adjusted color, based on the trueColor uniform
	gl_FragColor = mix(adjustedColor, diffuseColor, 1.0);
}