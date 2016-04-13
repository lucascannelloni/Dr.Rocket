#include "rocket.h"

vec3 dirVect;
GLfloat tiltAngle = 0.0f;

mat4 keyHandler(vec3* cam, vec3* cameraUp, Model* tm, vec3* rocketPoint)
{
    
    dirVect = Normalize(VectorSub(*rocketPoint,*cam));
    
    GLfloat cameraSpeed = 0.4f;
    GLfloat tiltSpeed = 0.05f;
    
    vec3 orthRocketVect = CrossProduct(dirVect, *cameraUp);

    if(glutKeyIsDown('g'))
    {
        rocketPoint->y = rocketPoint->y+1;
    }
    
    if(glutKeyIsDown('e'))
    {
        tiltAngle = tiltAngle+tiltSpeed;
    }
    if(glutKeyIsDown('d'))
    {
        tiltAngle = tiltAngle-tiltSpeed;
    }
    
    mat4 rocketRotate = ArbRotate(orthRocketVect,tiltAngle);
    
    if(glutKeyIsDown('SPACEBAR'))
    {
        vec3 thrustDir = MultVec3(rocketRotate,*cameraUp);
        *rocketPoint = VectorAdd(*rocketPoint,thrustDir);
    }
    
    *cam = VectorSub(*rocketPoint,ScalarMult(dirVect,50));
    return rocketRotate;
}


