#version 130

uniform sampler2D colorTex;
uniform float objectID;
in vec2 uv;

void main(void)
{

	gl_FragColor = texture(colorTex, uv);

	return;
}