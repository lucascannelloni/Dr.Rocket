#version 150

in  vec3 inPosition;
in  vec3 inNormal;

out vec3 exNormal;
out vec3 reflectedView;


uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
   
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
    
    exNormal = inNormal;
    
    vec3 viewDirectionInViewCoord  = normalize(vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0)));
    vec3 viewDirectionInWorldCoord = mat3(camMatrix) * viewDirectionInViewCoord;
    
    vec3 wcNormal = mat3(mdlMatrix) * inNormal;
    reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
     
}
