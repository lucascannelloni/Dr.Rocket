#version 150

in  vec3 inPosition;
in  vec3 inNormal;

out vec3 Normal;
out vec3 Position;
out vec3 exNormal;

uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;
uniform mat4 transCam;
uniform mat3 InvCamMatrix;

// TEST UNDER

out vec3 reflectedView;

void main(void)
{
	mat3 normalMatrix1 = mat3(camMatrix*mdlMatrix);
   
	gl_Position = projMatrix * camMatrix * mdlMatrix * vec4(inPosition, 1.0);
    
   Normal = mat3(transCam)* inNormal;
   Position = vec3(mdlMatrix * vec4(inPosition, 1.0f));
    
    exNormal = inNormal;
    
    // TEST UNDER DETTA
    
   // vec3 viewDirectionInWorldCoord = vec3(transCam * mdlMatrix * vec4(inPosition, 1.0));
    
   // vec3 wcNormal = mat3(mdlMatrix) * inNormal;
    //reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
    
    //
    
    vec3 posInViewCoord = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0));
    vec3 viewDirectionInViewCoord = normalize(posInViewCoord);
 
    vec3 viewDirectionInWorldCoord = inverse(mat3(camMatrix)) * viewDirectionInViewCoord;
    
 
    vec3 wcNormal = mat3(mdlMatrix) * inNormal;
  
    reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
}
