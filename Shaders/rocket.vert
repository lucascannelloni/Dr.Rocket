#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;

out vec2 texCoord;
out vec3 transformedNormal;
out vec4 vertexPoint;
out vec3 exNormal;
out vec3 pos;
out vec3 fromLightVect;
out vec3 toCameraVect;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform vec3 camPos;

const vec3 lightPos = vec3(-0.5,1,-0.5);

void main(void)
{
	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
    pos = inPosition;
    
    vertexPoint = mdlMatrix*vec4(inPosition,1.0);
    exNormal = inNormal;
    
    //lightdirection
    toCameraVect = camPos - vertexPoint.xyz;
    fromLightVect = vertexPoint.xyz - lightPos;
}
