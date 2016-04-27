#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec3 inTexCoord;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;
uniform mat4 transCam;
uniform mat3 InvCamMatrix;
uniform float time;
uniform vec3 lightPos;

out vec3 reflectedView;
out vec2 texCoord;
out vec3 refractView;

const float tiling = 6.0;
const float pi = 3.1415;

void main(void)
{
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
    
    //Reflect skybox in water
    vec3 posInViewCoord = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0));
    
    texCoord = vec2(posInViewCoord.x/800, posInViewCoord.z/800)* tiling;
    
    vec3 viewDirectionInViewCoord = normalize(posInViewCoord);
    vec3 viewDirectionInWorldCoord = InvCamMatrix * viewDirectionInViewCoord;
    vec3 wcNormal = mat3(mdlMatrix) * inNormal;
  
    reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
    
    //refract
    float ratio = 1/1.33;
    refractView = refract(viewDirectionInWorldCoord,normalize(wcNormal),ratio);
    
}
