#ifndef ROCKET_H_
#define ROCKET_H_
#include "givenFiles/VectorUtils3.h"
#include "givenFiles/loadobj.h"
#include "givenFiles/LoadTGA.h"
#include "givenFiles/MicroGlut.h"
#include "world.h"
#include <math.h>

mat4 keyHandler(vec3 *cam, vec3 *cameraUp, Model* tm, vec3 *rocketPoint);
//float heightCalc(float x, float z);
void mouse(int x, int y);

#endif