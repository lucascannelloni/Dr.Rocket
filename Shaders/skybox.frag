#version 150

out vec4 out_Colors;
in vec2 texCoord;
uniform sampler2D skyTex;

void main(void)
{
	out_Colors = texture(skyTex, texCoord);
}