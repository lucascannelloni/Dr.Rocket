#include "world.h"

int texwidth;
int texheight;

//TextureData t[6];
#define TEXTURE_OFFSET 0

char *textureFileName[6] =
{
// 0-5: petomavar: Pretty realistic skybox.
  "Skyboxes/mystic_rt.tga",
  "Skyboxes/mystic_lf.tga",
  "Skyboxes/mystic_up.tga",
  "Skyboxes/mystic_dn.tga",
  "Skyboxes/mystic_bk.tga",
  "Skyboxes/mystic_ft.tga",
};

void *loadTextures(GLuint *cubemap, TextureData *t)
{
	int i;
	
	glGenTextures(1, cubemap);			// Generate OpenGL texture IDs
	glActiveTexture(GL_TEXTURE0); // Just make sure the texture unit match
	
	// Note all operations on GL_TEXTURE_CUBE_MAP, not GL_TEXTURE_2D
	
	// Load texture data and create ordinary texture objects (for skybox)
	for (i = 0; i < 6; i++)
	{
		printf("Loading texture %s\n", textureFileName[i+TEXTURE_OFFSET]);
		LoadTGATexture(textureFileName[i+TEXTURE_OFFSET], &t[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	// Load to cube map
	glBindTexture(GL_TEXTURE_CUBE_MAP, *cubemap);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, t[0].w, t[0].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[0].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, t[1].w, t[1].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[1].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, t[2].w, t[2].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[2].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, t[3].w, t[3].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[3].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, t[4].w, t[4].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[4].imageData);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, t[5].w, t[5].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, t[5].imageData);
  
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

// MIPMAPPING
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}


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

float heightCalc(float x, float z, Model* tm)
{
    int xCeil = ceil(x);
    int zCeil = ceil(z);

    float y1 = tm->vertexArray[(xCeil + zCeil*texwidth)*3 + 1];
    float y2 = tm->vertexArray[(xCeil - 1 + zCeil*texwidth)*3 + 1];
    float y3 = tm->vertexArray[(xCeil + (zCeil - 1)*texwidth)*3 + 1];
    float y4 = tm->vertexArray[(xCeil - 1 + (zCeil - 1)*texwidth)*3 + 1];

    float xe = xCeil - x;
    float ze = zCeil - z;
    float A = y4*xe + y3*(1-xe);
    float B = y2*xe + y1*(1-xe);
    float y = A*ze + B*(1-ze);
    return y;
}
