#version 130
uniform sampler2D diffuseTexture;
uniform int modelID;
uniform float trueColor;
uniform float alpha;
uniform bool bendingActivated;
uniform sampler2D routeMask;
uniform vec2 screenSize;
uniform vec2 viewOrigin;

in vec2 uv;
in float scaled_height;

in float alphaModifier;

void main() {

	// fragment location in navigation Window normalized to 0-1
	vec2 normalizedWindowPosition = (gl_FragCoord.xy - viewOrigin) / screenSize;
	//set alpha depending on the presence of the route in this fragment
	//routeMask is rendered fbo of same shot, containing only route+naviIcon
	float masked_alpha = (1 - texture(routeMask, normalizedWindowPosition).a);
	masked_alpha *= alphaModifier;
	
	gl_FragColor = vec4(texture(diffuseTexture, uv).xyz, masked_alpha);
}
