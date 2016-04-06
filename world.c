#include "world.h"

int texwidth;
int texheight;

Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;

	texwidth = tex->width;
	texheight = tex->height;
	
	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 10.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.
			normalArray[(x + z * tex->width)*3 + 0] = 0.0;
			normalArray[(x + z * tex->width)*3 + 1] = 1.0;
			normalArray[(x + z * tex->width)*3 + 2] = 0.0;
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
		}
	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
	
	Point3D cross;
	Point3D tmp1;
	Point3D tmp2;
	vec3 dx;
	vec3 dz;
	int index;
	for (x = 0; x < tex->width -1; x++)
		for (z = 0; z < tex->height - 1; z++)
		{
			
			index = (x + 1 + z * (tex->width))*3;
			tmp1 = SetVector(vertexArray[index + 0],vertexArray[index + 1],vertexArray[index + 2]);
			index = (x + z * (tex->width))*3;
			tmp2 = SetVector(vertexArray[index + 0],vertexArray[index + 1],vertexArray[index + 2]);
			dx = VectorSub(tmp1,tmp2);


			index = (x + (z + 1) * (tex->width))*3;
			tmp1 = SetVector(vertexArray[index + 0],vertexArray[index + 1],vertexArray[index + 2]);
			index = (x + z  * (tex->width))*3;
			tmp2 = SetVector(vertexArray[index + 0],vertexArray[index + 1],vertexArray[index + 2]);
			dz = VectorSub(tmp1,tmp2);

			cross = CrossProduct(dz,dx);
			cross = Normalize(cross);

			normalArray[(x + z * tex->width)*3 + 0] = cross.x;
			normalArray[(x + z * tex->width)*3 + 1] = cross.y;
			normalArray[(x + z * tex->width)*3 + 2] = cross.z;

	}
	

	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}


