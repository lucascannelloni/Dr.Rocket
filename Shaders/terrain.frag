#version 150

out vec4 outColor;

in vec2 texCoord;
in vec3 transformedNormal;
in vec4 vertexPoint;
in vec3 exNormal;
in vec3 pos;

uniform sampler2D tex1;
uniform sampler2D tex2;

uniform mat4 mdlMatrix;

float shade,diffuse,specular,cosAng;
int specularExponent = 5;

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
   // vec3 halfway = normalize(eyeDir + normLight);
    
    cosAng = max(0.0,dot(reflectDir,eyeDir));
    specular = pow(cosAng,specularExponent);
    //specular = 0;
    
    if(pos.y > 5){
    
    // Out Color
    outColor2 = mix(texture(tex1,texCoord),texture(tex2,texCoord),0.3);
    outColor1 = vec4(0.5*(diffuse + specular), 0.5*(diffuse + specular), 0.5*(diffuse + specular),1);
    outColor = outColor1*outColor2;
    }
    else
    {
        
        outColor2 = mix(texture(tex1,vec2(reflectDir)),texture(tex2,vec2(reflectDir)),0.3);
        outColor1 = vec4(0.3,0.2,0.5,0.7);
        outColor = outColor1*outColor2;
        
    }

    //outColor2 = mix(texture(tex1,texCoord),texture(tex2,texCoord),0.3);
   // outColor = outColor1*outColor2;
}
