#version 150

out vec4 outColor;

uniform vec3 cameraPos;
uniform samplerCube cubeMap;
uniform float time;
uniform sampler2D dudvMap;
uniform sampler2D refractTex;
uniform sampler2D normalMap;
uniform float moveFactor;

in vec2 texCoord;
in vec3 reflectedView;
in vec3 refractView;
in vec3 fromLightVect;
in vec3 toCameraVect;
in vec4 pos;

const float waveStrength = 0.06;
const vec3 lightColor = vec3(0.6,0.4,0.3);
const float shineDamper = 20.0;
const float reflectivity = 0.6;
float fogFactor;

void main(void)
{
    /*
    vec2 distortion1 = (texture(dudvMap,vec2(texCoord.x + moveFactor/5,texCoord.y)).rg*2 - 1)*waveStrength;
    vec2 distortion2 = (texture(dudvMap,vec2(-texCoord.x + moveFactor/5,texCoord.y + moveFactor/5)).rg*2 - 1)*waveStrength;
    vec2 totalDistortion = distortion1 + distortion2;
    */
    
    vec2 distortedTexCoords = texture(dudvMap, vec2(texCoord.x + moveFactor*200, texCoord.y)).rg*0.1;
    distortedTexCoords = texCoord + vec2(distortedTexCoords.x, distortedTexCoords.y+moveFactor*200);
    vec2 totalDistortion = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;
    
    vec3 reflectDistortion = vec3(totalDistortion.x,totalDistortion.y,0) + normalize(reflectedView);
 //   reflectDistortion.y = clamp(reflectDistortion.y,0.001,0.999);
  //  reflectDistortion.x = clamp(reflectDistortion.x,-0.999,-0.001);
    
    vec3 refractDistortion = vec3(totalDistortion.x,totalDistortion.y,0) + normalize(refractView);
    
    vec4 normalMapColor = texture(normalMap,distortedTexCoords);
    vec3 normal = vec3(normalMapColor.r*2.0 - 1.0, normalMapColor.b, normalMapColor.g*2.0 -1.0);
    normal = normalize(normal);
    
    vec3 reflectedLight = reflect(normalize(fromLightVect), normal);
    float specular = max(dot(reflectedLight, normalize(toCameraVect)), 0.0);
    specular = pow(specular, shineDamper);
    vec3 specularHighlights = lightColor * specular * reflectivity;
    
    //vec4 outColor1 = texture(cubeMap, reflectDistortion);
    float dist = abs(length(vec3(pos)));
    float horizon = 1200;
    float world = 900;
    
    if (dist < world)
    {
        fogFactor = 0.92;
    }
    else if (dist > horizon)
    {
        fogFactor = 0.02;
    }
    else{
        fogFactor = world/dist;
    }

   // outColor = mix(outColor1,texture(refractTex, vec2(refractDistortion.x,reflectDistortion.y)), 0.2);
    
    vec4 outColor1 = vec4(texture(cubeMap,reflectDistortion).rgb,0.92);
    outColor = outColor1 + vec4(specularHighlights,0.0);
    outColor = vec4(outColor.xyz, fogFactor);
}
