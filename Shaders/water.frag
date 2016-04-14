#version 150

out vec4 outColor;

in vec3 exNormal;

in vec3 reflectedView;

uniform samplerCube cubeMap;

void main(void)
{
    outColor = texture(cubeMap,normalize(reflectedView));
}
