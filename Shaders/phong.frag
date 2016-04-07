#version 150

out vec4 out_Colors;
in vec2 texCoord;
in vec3 transformedNormal;
in vec4 reflectPoint;

uniform sampler2D tex;
uniform sampler2D texUnit2;

uniform vec3 lightSourcesDirPosArr[4];
uniform vec3 lightSourcesColorArr[4];

uniform float specularExponent[4];
uniform bool isDirectional[4];
uniform mat4 camMatrix;


void main(void)
{
    float shade, cosAng;
    vec3 s,n, color, reflectVect, viewDir,shading,diffuse,diffuse_temp,specular_temp,specular;
    vec4 tmp_Colors,tmp_Colors2;
    diffuse = vec3(0.0);
    specular = vec3(0.0);
    shading = vec3(0.0);
    
    vec3 lightSourcesDir[4];
    
    viewDir = normalize(-vec3(reflectPoint));
    
    tmp_Colors = texture(tex, texCoord);
    tmp_Colors2 = texture(texUnit2, texCoord);
    
    for(int i=0;i<4;i++)
    {
        lightSourcesDir[i]= mat3(camMatrix)*lightSourcesDirPosArr[i]; //view cooord
        
        if(!isDirectional[i])
        {
            vec4 temp = camMatrix*vec4(lightSourcesDirPosArr[i],1.0);
            lightSourcesDir[i] =  vec3(temp)-vec3(reflectPoint);
        }
        
        n = normalize(transformedNormal);

        // DIFFUSE
        color = lightSourcesColorArr[i];
        s = normalize(lightSourcesDir[i]);
        
        float alpha = max(0.0,dot(s,n));
        
        diffuse_temp = color*alpha;
        diffuse += diffuse_temp;
            
        // THE REFLECTION VECTOR
        reflectVect = reflect(s,n); //2*transformedNormal*dot(normalize(transformedNormal),s) - s;
            
        //SPECULAR
        cosAng = dot(reflectVect,viewDir);
        cosAng = max(0.0,cosAng);
            
        specular_temp = color*pow(cosAng,specularExponent[i]);
            
        specular += specular_temp;
    
    }

    
    shading = diffuse + specular;
    
   // shading = shading+tmp_Colors+tmp_Colors2;
    out_Colors = vec4(shading,1)*tmp_Colors;//mix(tmp_Colors,tmp_Colors2,0.5);

    
//    out_Colors = vec4(diffuseRed,diffuseGreen,diffuseBlue,diffuseWhite);
    //out_Colors =texture(texUnit, texCoord)*shadered;
}

