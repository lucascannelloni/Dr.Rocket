#version 150

out vec4 outColor;

in vec3 exNormal;

in vec3 reflectedView;

uniform samplerCube cubeMap;

void main(void)
{
    outColor = vec4(reflectedView,0.5);//texture(cubeMap,normalize(reflectedView));
}
