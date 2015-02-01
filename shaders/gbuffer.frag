#version 130

uniform sampler2D colorTex;
uniform float objectID;

void main(void)
{
	vec4 texColor = vec4(0.0);

	const bool hasTexture = false;
	if (hasTexture)
	{
		texColor = texture2D(colorTex, gl_TexCoord[0].st);
	}
	gl_FragColor = texColor; // sceneColor

	return;
}