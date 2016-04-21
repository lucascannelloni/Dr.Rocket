#version 150

out vec4 outColor;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;
uniform float time;


in vec2 texCoord;

//TEST NEDAN

in vec3 reflectedView;

void main(void)
{
    vec3 bumpNorm = vec3(sin(0.001*time),0,cos(0.001*time));
    
    float fx = texCoord.s*2.0 - 1.0;
    float fy = texCoord.t*2.0 - 1.0;
    float shade = sqrt(fx*fx + fy * fy);
    
    
   // vec4 outColor1 = mix(texture(cubeMap, normalize(reflectedView)), vec4(cos(0.001*time)/ 2.0 + 0.5, 1, sin(0.001*time)/ 2.0 + 0.5, 0.4),0.3);
    
    
    vec4 outColor1 = texture(cubeMap, normalize(reflectedView));
    vec4 outColor2 = vec4(0.3,0.5,0.7,1);
    outColor = outColor1*outColor2;
}
