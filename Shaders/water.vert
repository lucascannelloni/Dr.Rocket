#version 150

in  vec3 inPosition;
in  vec3 inNormal;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;
uniform mat4 transCam;
uniform mat3 InvCamMatrix;
uniform float time;
uniform mat4 rotMatrix;

out vec3 reflectedView;

void main(void)
{
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
    
    
    //Reflect skybox in water
    vec3 posInViewCoord = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0));
    vec3 viewDirectionInViewCoord = normalize(posInViewCoord);
 
    vec3 viewDirectionInWorldCoord = InvCamMatrix * viewDirectionInViewCoord;
    vec3 waveNormal = inNormal * mat3(rotMatrix);
    vec3 wcNormal = mat3(mdlMatrix) * waveNormal;
  
    reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
}
