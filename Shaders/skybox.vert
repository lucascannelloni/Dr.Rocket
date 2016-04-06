#version 150

in vec3 in_Position;
in vec2 inTexCoord;

uniform mat4 projectionMatrix;
uniform mat4 mdlMatrix;
uniform mat4 lookMatSky;

out vec2 texCoord;

void main(void)
{
    	//mat3 normalMatrix1 = mat3(mdlMatrix);
    	//vec3 transformedNormal = normalMatrix1 * in_Normal;

	//const vec3 light = vec3(0.58, 0.58, 0.58);
	//float shade;
	//shade = dot(normalize(transformedNormal), light);
	//exColor = (shade);

	texCoord = inTexCoord;
	gl_Position = projectionMatrix*lookMatSky*mdlMatrix*vec4(in_Position, 1.0);
}