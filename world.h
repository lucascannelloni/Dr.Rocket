#ifndef WORLD_H_
#define WORLD_H_
#include "givenFiles/VectorUtils3.h"
#include "givenFiles/loadobj.h"
#include "givenFiles/LoadTGA.h"

Model* GenerateTerrain(TextureData *tex);
float heightCalc(float x, float z, Model* tm);
void loadTextures(GLuint *cubemap, TextureData *t, Model *box[6]);
void waterCheck(vec3 *rocketPoint, int* waterLevel);


//GLuint loadCubemap();

#endif