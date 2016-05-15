#include "rocket.h"

vec3 dirVect;
GLfloat tiltAngle = 0.0f;

mat4 keyHandler(vec3* cam, vec3* cameraUp, Model* tm, vec3* rocketPoint,vec3* rocketVel, vec3* rocketTopPoint)
{
    
    dirVect = Normalize(VectorSub(*rocketPoint,*cam));
    
 //   GLfloat cameraSpeed = 0.4f;
    GLfloat tiltSpeed = 0.05f;
    
    vec3 orthRocketVect = CrossProduct(dirVect, *cameraUp);
    
    float groundHeight = heightCalc(fmod(abs(rocketPoint->x),255), fmod(abs(rocketPoint->z),255), tm);
   // printf("rocketPy %f\n", fmod(rocketPoint->y,255));
    printf("GH %f\n", groundHeight);
    float rocketOffset = 0;
    if (rocketPoint->y<groundHeight+rocketOffset && fabsf(rocketVel->y) < 2  && fabsf(tiltAngle) < 0.5f)
    {
        rocketPoint->y = groundHeight + rocketOffset;
        *rocketVel = SetVector(0,0,0);
    }
    else if(rocketPoint->y<groundHeight+rocketOffset || rocketTopPoint->y<groundHeight+rocketOffset)
    {

        *rocketVel = SetVector(rocketVel->x/1.2,-rocketVel->y/1.2,rocketVel->z/1.2);
        gameOver();
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
    
   
    vec3 thrustDir = Normalize(MultVec3(rocketRotate,*cameraUp));
    
    if(glutKeyIsDown('y'))
    {
        *rocketPoint = VectorAdd(*rocketPoint,thrustDir);
    }
    if(glutKeyIsDown('f'))
    {
        *rocketVel = VectorAdd(*rocketVel,ScalarMult(thrustDir,0.07));
    }
    
    *rocketTopPoint = VectorAdd(*rocketPoint,ScalarMult(thrustDir,7));
    return rocketRotate;
}


