#version 150

out vec4 outColor;

in vec2 texCoord;
in vec4 vertexPoint;
in vec3 exNormal;
in vec3 pos;
in vec3 fromLightVect;
in vec3 toCameraVect;

uniform sampler2D texRocket;

uniform mat4 mdlMatrix;
uniform int objectFlag;

float shade,diffuse,specular,cosAng;
int specularExponent = 20;

const vec3 lightColor = vec3(1,0,0.3);
const float reflectivity = 0.8;

void main(void)
{
    
    vec4 outColor1,outColor2;
    
    //Specular
    
    vec3 reflectedLight = reflect(normalize(fromLightVect), exNormal);
    specular = max(dot(reflectedLight, normalize(toCameraVect)), 0.0);
    specular = pow(specular, specularExponent);
    vec3 specularHighlights = lightColor * specular * reflectivity;
    
    // Out Color
    
    outColor2 = texture(texRocket,texCoord);

    outColor = outColor2 + vec4(specularHighlights,0.0);
    if(objectFlag==0)
    {
        int r = int(pos.x) % 5;
        outColor=vec4(0.8+(1/r),0.4,0.1,0.1) + vec4(specularHighlights,0.0);
    }


}
