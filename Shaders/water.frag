#version 150

out vec4 outColor;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;
uniform float time;

//TEST NEDAN

in vec3 reflectedView;

void main(void)
{
    outColor = texture(cubeMap, normalize(reflectedView));
   //outColor = vec4(reflectedView,1);
}
