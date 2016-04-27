#version 150

out vec4 outColor;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;
uniform float time;
uniform sampler2D dudvMap;
uniform sampler2D refractTex;
uniform sampler2D normalMap;
uniform vec3 lightColor;

in vec2 texCoord;
in vec3 reflectedView;
in vec3 refractView;

const float waveStrength = 0.05;
uniform float moveFactor;

void main(void)
{
    /*
    vec2 distortion1 = (texture(dudvMap,vec2(texCoord.x + moveFactor/5,texCoord.y)).rg*2 - 1)*waveStrength;
    vec2 distortion2 = (texture(dudvMap,vec2(-texCoord.x + moveFactor/5,texCoord.y + moveFactor/5)).rg*2 - 1)*waveStrength;
    vec2 totalDistortion = distortion1 + distortion2;
    */
    
    vec2 distortedTexCoords = texture(dudvMap, vec2(texCoord.x + moveFactor*100, texCoord.y)).rg*0.1;
    distortedTexCoords = texCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor*100);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    
    vec3 reflectDistortion = vec3(totalDistortion.x,totalDistortion.y,0) + normalize(reflectedView);
    vec3 refractDistortion = vec3(totalDistortion.x,totalDistortion.y,0) + normalize(refractView);
    
    vec4 normalMapColor = texture(normalMap,distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r*2.0 - 1.0, normalMapColor.b, normalMapColor.g*2.0 -1.0);
    normal = normalize(normal);
    
    //vec4 outColor1 = texture(cubeMap, reflectDistortion);

   // outColor = mix(outColor1,texture(refractTex, vec2(refractDistortion.x,reflectDistortion.y)), 0.2);
    
    vec4 outColor1 = vec4(texture(cubeMap,reflectDistortion).rgb,0.3);
    outColor = mix(outColor1,normalMapColor,0.2);
    
}
