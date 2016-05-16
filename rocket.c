#include "rocket.h"

vec3 dirVect;
GLfloat tiltAngle = 0.0f;

mat4 keyHandler(vec3* cam, vec3* cameraUp, Model* tm, vec3* rocketPoint,vec3* rocketVel, vec3* rocketTopPoint, float* fuel)
{
    
    dirVect = Normalize(VectorSub(*rocketPoint,*cam));
    
 //   GLfloat cameraSpeed = 0.4f;
    GLfloat tiltSpeed = 0.05f;
    
    vec3 orthRocketVect = CrossProduct(dirVect, *cameraUp);
    
    float groundHeight = heightCalc(fmod(abs(rocketPoint->x),256), fmod(abs(rocketPoint->z),256), tm);
   // printf("rocketPy %f\n",rocketPoint->y);
   // printf("rocketPx %f\n",fmod(abs(rocketPoint->x),256));
   // printf("rocketPz %f\n",fmod(abs(rocketPoint->z),256));
   // printf("GH %f\n", groundHeight);
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
    if(glutKeyIsDown('f') && *fuel > 0)
    {
        *rocketVel = VectorAdd(*rocketVel,ScalarMult(thrustDir,0.07));
        *fuel = *fuel - 1;
        printf("fuel %f\n",*fuel);
    }
    
    *rocketTopPoint = VectorAdd(*rocketPoint,ScalarMult(thrustDir,7));
    return rocketRotate;
}


