#version 130

uniform sampler2D colorTex;
uniform float objectID;
in float distToCamera;
in vec2 uv;



void main(void)
{

	gl_FragColor = texture(colorTex, uv);

	return;
}