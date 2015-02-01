#version 130

uniform int modelID;
uniform float glowIntensity;
uniform vec4 materialColor;
uniform vec3 ambientColor;

in vec2 uv;

void main() {
	
	vec4 ambient_color = gl_FrontMaterial.ambient * gl_LightSource[0].ambient + gl_LightModel.ambient * gl_FrontMaterial.ambient;
	
	gl_FragColor = gl_FrontMaterial.diffuse * gl_LightSource[0].diffuse;
}