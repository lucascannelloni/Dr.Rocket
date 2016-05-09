#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 transformedNormal;
in vec4 vertexPoint;
in vec3 exNormal;
in vec3 pos;

uniform sampler2D tex1;
uniform sampler2D tex2;
uniform samplerCube cubeMap;
uniform mat4 mdlMatrix;
uniform vec3 camPos;

float shade,diffuse,specular,cosAng,fogFactor;
int specularExponent = 3;

void main(void)
{
    vec4 outColor1,outColor2;
    
    // Light & Normal
    vec3 normLight = vec3(0,1,0); //above
    mat3 normalMatrix1 = mat3(mdlMatrix);
    normLight = normalMatrix1*normLight;
    
    vec3 normNormals = normalize(transformedNormal);
    
    //Diffuse
    shade = dot(normNormals, normLight);
    diffuse = clamp(shade,0,1);
    
    //Specular
    vec3 eyeDir = -normalize(vec3(vertexPoint));
    vec3 reflectDir = reflect(-normLight,normNormals);

    cosAng = max(0.0,dot(reflectDir,eyeDir));
    specular = pow(cosAng,specularExponent);

    float dist = length(abs(camPos - pos));
    float horizon = 500;
    float world = 300;
    
    if (dist < world) //no idea??
    {
        fogFactor = 1;
    }
    else if (dist > horizon)
    {
    fogFactor = 0;
    }
    else{
        fogFactor = world/dist;
    }
    
    // Out Color
    outColor2 = vec4(texture(tex1,texCoord).xyz,fogFactor);
    outColor1 = vec4(0.5*(diffuse + specular), 0.5*(diffuse + specular), 0.5*(diffuse + specular),fogFactor);
    outColor = outColor1*outColor2;


}
