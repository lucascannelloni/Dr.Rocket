#version 150

in vec3 inPosition;
in vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 transformedNormal;
out vec4 vertexPoint;
out vec3 exNormal;
out vec4 pos;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
    transformedNormal = normalMatrix1*inNormal;
    
	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
    pos = mdlMatrix*vec4(inPosition,1.0);
    
    vertexPoint = mdlMatrix*vec4(inPosition,1.0);
    exNormal = inNormal;
}
