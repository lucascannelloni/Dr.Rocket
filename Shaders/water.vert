#version 150

in  vec3 inPosition;
in  vec3 inNormal;

out vec3 Normal;
out vec3 Position;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;
uniform mat4 InvCamMatrix;

void main(void)
{
	mat3 normalMatrix1 = mat3(camMatrix*mdlMatrix);
   
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
    
    Normal = mat3(transpose(InvCamMatrix)) * inNormal;
    Position = vec3(mdlMatrix * vec4(inPosition, 1.0f));
    
}
