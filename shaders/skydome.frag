#version 130

uniform samplerCube cubemap;
uniform int modelID;
const float blackenFactor = 1.0;

void main() {
	vec4 color = texture(cubemap, gl_TexCoord[0].xyz);
	color = vec4(color.rgb * blackenFactor, color.a);
	gl_FragColor = color;
	return;
}