#include "rocket.h"

vec3 dirVect;
GLfloat tiltAngle = 0.0f;

mat4 keyHandler(vec3* cam, vec3* cameraUp, Model* tm, vec3* rocketPoint,vec3* rocketVel)
{
    
    dirVect = Normalize(VectorSub(*rocketPoint,*cam));
    
    GLfloat cameraSpeed = 0.4f;
    GLfloat tiltSpeed = 0.05f;
    
    vec3 orthRocketVect = CrossProduct(dirVect, *cameraUp);
    
    float groundHeight = heightCalc(rocketPoint->x, rocketPoint->z, tm);
    float rocketOffset = 10;
    if (rocketPoint->y<groundHeight+rocketOffset)
    {
        rocketPoint->y =groundHeight+rocketOffset;
        rocketVel->y = -rocketVel->y;
    }
    
    if(glutKeyIsDown('d'))
    {
        tiltAngle = tiltAngle+tiltSpeed;
    }
    if(glutKeyIsDown('e'))
    {
        tiltAngle = tiltAngle-tiltSpeed;
    }
    
    mat4 rocketRotate = ArbRotate(orthRocketVect,tiltAngle);
    
   
    vec3 thrustDir = MultVec3(rocketRotate,*cameraUp);
    
    if(glutKeyIsDown('y'))
    {
        *rocketPoint = VectorAdd(*rocketPoint,thrustDir);
    }
    if(glutKeyIsDown('f'))
    {
        *rocketVel = VectorAdd(*rocketVel,ScalarMult(thrustDir,0.07));
    }
    
    //*cam = VectorSub(*rocketPoint,ScalarMult(dirVect,50));
    return rocketRotate;
}


