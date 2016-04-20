#version 150

out vec4 outColor;

in vec3 Normal;
in vec3 Position;
in vec3 exNormal;
in vec3 wcNormal;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;

//TEST NEDAN

in vec3 reflectedView;

void main(void)
{
   vec3 I = normalize(Position - cameraPos);
   vec3 R = reflect(I, normalize(Normal));
    outColor = texture(cubeMap, R);
    
    // test
    
    outColor = texture(cubeMap, normalize(reflectedView));
   //outColor = vec4(reflectedView,1);
}
