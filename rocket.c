#include "rocket.h"

vec3 dirVect;

void keyHandler(vec3* cam, vec3* lookAtPoint, vec3* cameraUp, Model* tm)
{
    dirVect = VectorSub(*lookAtPoint,*cam);
    GLfloat cameraSpeed = 0.4f;

    if(glutKeyIsDown('s')) { *cam = VectorSub(*cam,ScalarMult(dirVect,cameraSpeed));
        float yCam = heightCalc(cam->x,cam->z,tm);
        if (cam->y < yCam + 0.5)
        {
            cam->y = yCam + 0.5;
        }
        *lookAtPoint = VectorAdd(dirVect,*cam);}
    if(glutKeyIsDown('w')) { *cam = VectorAdd(*cam,ScalarMult(dirVect,cameraSpeed));
        float yCam = heightCalc(cam->x,cam->z,tm);
        if (cam->y < yCam + 0.5)
        {
            cam->y = yCam + 0.5;
        }
    *lookAtPoint = VectorAdd(dirVect,*cam);}
    if(glutKeyIsDown('a')) { *cam = VectorSub(*cam,ScalarMult(Normalize(CrossProduct(dirVect,*cameraUp)),cameraSpeed));
        float yCam = heightCalc(cam->x,cam->z,tm);
        if (cam->y < yCam + 0.5)
        {
            cam->y = yCam + 0.5;
        }
        *lookAtPoint = VectorAdd(dirVect,*cam);}
    if(glutKeyIsDown('d')) { *cam = VectorAdd(*cam,ScalarMult(Normalize(CrossProduct(dirVect,*cameraUp)),cameraSpeed));
        float yCam = heightCalc(cam->x,cam->z,tm);
        if (cam->y < yCam + 0.5)
        {
            cam->y = yCam + 0.5;
        }
    *lookAtPoint = VectorAdd(dirVect,*cam);}
}


