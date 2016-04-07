#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

out vec2 texCoord;
out vec3 transformedNormal;
out vec4 reflectPoint;

void main(void)
{
    
    mat4 cameraMatrix = camMatrix*mdlMatrix;
    mat3 normalMatrix1 = mat3(cameraMatrix);
    transformedNormal = normalMatrix1*inNormal;

    reflectPoint = camMatrix*mdlMatrix*vec4(inPosition, 1.0);

	texCoord = inTexCoord;
	gl_Position = projMatrix*camMatrix*mdlMatrix*vec4(inPosition, 1.0);
}