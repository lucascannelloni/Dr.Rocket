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
uniform vec3 camPos;


out vec3 reflectedView;
out vec2 texCoord;
out vec3 refractView;
out vec3 fromLightVect;
out vec3 toCameraVect;

const float tiling = 6.0;
const float pi = 3.1415;
const vec3 lightPos = vec3(-0.5,1,-0.5);


void main(void)
{
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
    
    //Reflect skybox in water
    vec3 posInViewCoord = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0));
    
    texCoord = vec2(inPosition.x/800, inPosition.z/800)* tiling;
    
    vec3 viewDirectionInViewCoord = normalize(posInViewCoord);
    vec3 viewDirectionInWorldCoord = InvCamMatrix * viewDirectionInViewCoord;
    vec3 wcNormal = mat3(mdlMatrix) * inNormal;
  
    reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
  //  reflectedView.x = clamp(reflectedView.z,0.001,0.999);
   // reflectedView.y = clamp(reflectedView.x,-0.999,-0.001);

    
    //refract
    float ratio = 1/1.33;
    refractView = refract(viewDirectionInWorldCoord,normalize(wcNormal),ratio);
    
    //lightdirection
    vec4 wordlPos = mdlMatrix * vec4(inPosition, 1.0);
    toCameraVect = camPos - wordlPos.xyz;
    fromLightVect = wordlPos.xyz - lightPos;
}
