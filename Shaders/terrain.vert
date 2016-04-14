#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;
out vec3 transformedNormal;
out vec4 vertexPoint;
out vec3 exNormal;
out vec3 pos;
out vec3 reflectedView;


uniform mat4 projMatrix;
uniform mat4 mdlMatrix;
uniform mat4 camMatrix;

void main(void)
{
	mat3 normalMatrix1 = mat3(mdlMatrix);
    transformedNormal = normalMatrix1*inNormal;
    
	texCoord = inTexCoord;
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);
    pos = inPosition;
    
    vertexPoint = mdlMatrix*vec4(inPosition,1.0);
    exNormal = inNormal;
    
    
    vec3 posInViewCoord = vec3(camMatrix * mdlMatrix * vec4(inPosition, 1.0));
    vec3 viewDirectionInViewCoord = normalize(posInViewCoord);
    vec3 viewDirectionInWorldCoord = inverse(mat3(camMatrix)) * viewDirectionInViewCoord;
    
    vec3 wcNormal = mat3(mdlMatrix) * inNormal;
    reflectedView = reflect(viewDirectionInWorldCoord, normalize(wcNormal));
     
    
}
