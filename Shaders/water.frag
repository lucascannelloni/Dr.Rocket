#version 150

out vec4 outColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;

void main(void)
{
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    outColor = texture(cubeMap, R);
}
