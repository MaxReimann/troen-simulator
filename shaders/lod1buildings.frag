#version 130
uniform sampler2D diffuseTexture;
uniform int modelID;
uniform float trueColor;
uniform float alpha;
uniform bool bendingActivated;
uniform sampler2D routeMask;

in vec2 uv;
in float scaled_height;

void main() {


	gl_FragColor =  vec4(texture(routeMask, uv).xyz,1.0) ;// vec4(texture(diffuseTexture, uv).xyz, alpha);

}