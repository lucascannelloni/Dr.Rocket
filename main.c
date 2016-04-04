// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"


mat4 projectionMatrix;
vec3 cam = {10, 10, 10};
vec3 dirVect,cameraFront;
vec3 lookAtPoint = {0, 0, 0};
vec3 cameraUp = {0,1,0};
bool firstMouse = true;
int lastX,lastY;
float pi = 3.1415;
float yaw, pitch;
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
			/*
			index = x + z * (tex->width)*6;
			int tmp1 = indexArray[index + 0];
			int tmp2 = indexArray[index + 1];
			vec3 v1 = VectorSub(tmp2,tmp1);

			tmp2 = indexArray[index + 2];
			vec3 v2 = VectorSub(tmp2,tmp1);
*/
			//cross =  CrossProduct(dx,dz);

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


// vertex array object
Model *m, *m2, *tm, *sphere;
// Reference to shader program
GLuint program;
GLuint tex1, tex2, skyTex;
TextureData ttex; // terrain

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

	// Load and compile shader
	program = loadShaders("terrain.vert", "terrain.frag");
	glUseProgram(program);
	printError("init shader");

	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0
	LoadTGATextureSimple("grass.tga", &tex1);

	glUniform1i(glGetUniformLocation(program, "tex2"), 1); // Texture unit 0
	LoadTGATextureSimple("maskros512.tga",&tex2);

	glUniform1i(glGetUniformLocation(program, "skyTex"), 2); // Texture unit 0
	LoadTGATextureSimple("SkyBox512.tga",&tex2);
// Load terrain data
	
	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

	//Load Sphere Model
	sphere = LoadModelPlus("groundsphere.obj");
	skybox = LoadModelPlus("skybox.obj");

}

float heightCalc(float x, float z)
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

void keyHandler()
{
	dirVect = VectorSub(lookAtPoint,cam);
	GLfloat cameraSpeed = 0.4f;

	if(glutKeyIsDown('s')) { cam = VectorSub(cam,ScalarMult(dirVect,cameraSpeed));
		float yCam = heightCalc(cam.x,cam.z);
		if (cam.y < yCam + 0.5)
		{
			cam.y = yCam + 0.5;
		}
		lookAtPoint = VectorAdd(dirVect,cam);}
    if(glutKeyIsDown('w')) { cam = VectorAdd(cam,ScalarMult(dirVect,cameraSpeed));
    	float yCam = heightCalc(cam.x,cam.z);
		if (cam.y < yCam + 0.5)
		{
			cam.y = yCam + 0.5;
		}
    lookAtPoint = VectorAdd(dirVect,cam);}
    if(glutKeyIsDown('a')) { cam = VectorSub(cam,ScalarMult(Normalize(CrossProduct(dirVect,cameraUp)),cameraSpeed));
    	float yCam = heightCalc(cam.x,cam.z);
		if (cam.y < yCam + 0.5)
		{
			cam.y = yCam + 0.5;
		}
    	lookAtPoint = VectorAdd(dirVect,cam);}
    if(glutKeyIsDown('d')) { cam = VectorAdd(cam,ScalarMult(Normalize(CrossProduct(dirVect,cameraUp)),cameraSpeed));
    	float yCam = heightCalc(cam.x,cam.z);
		if (cam.y < yCam + 0.5)
		{
			cam.y = yCam + 0.5;
		}
    lookAtPoint = VectorAdd(dirVect,cam);}
}



mat4 placeModelOnGround(float x, float z)
{
 	float y = heightCalc(x,z);
 	printf("x %f\n", x);
 	printf("y %f\n", y);
 	printf("z %f\n", z);
 	mat4 trans = T(x,y,z);
 	return trans;
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
	
	mat4 total, modelView, camMatrix;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	keyHandler();
	camMatrix = lookAt(cam.x, cam.y, cam.z,
				lookAtPoint.x, lookAtPoint.y, lookAtPoint.z,
				cameraUp.x,cameraUp.y,cameraUp.z);
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

	glDisable(GL_TEXTURE_2D);
	// SPHERE
	float x = 10 + 5*sin(0.0005*t);
	float z = 10 + 7*cos(0.0005*t);
	mat4 sphereTrans = placeModelOnGround(x,z);
	sphereTrans = Mult(camMatrix, sphereTrans);
	
	//glEnable(GL_TEXTURE_2D);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, tex2);		// Bind Our Texture tex2
	//glUniform1i(glGetUniformLocation(program, "tex2"), 1); // Texture unit 1
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, sphereTrans.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	DrawModel(sphere, program, "inPosition", "inNormal", "inTexCoord");

	glDisable(GL_TEXTURE_2D);
	printError("display 2");
	
	glutSwapBuffers();
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	x = x-300;
	y = y-300;

	if(firstMouse)
    {
         lastX = x;
         lastY = y;
        firstMouse = false;
    }
	GLfloat xoffset = x - lastX;
	GLfloat yoffset = y - lastY;
	lastX = x;
	lastY = y;

	GLfloat sensitivity = 0.4;
    xoffset = sensitivity*xoffset;
    yoffset = sensitivity*yoffset;

   	yaw  = yaw + xoffset;
    pitch = pitch + yoffset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    cameraFront.x = cos(pi*yaw/180) * cos(pi*pitch/180);
    cameraFront.y = sin(pi*pitch/180);
    cameraFront.z = sin(pi*yaw/180) * cos(pi*pitch/180);
    cameraFront = Normalize(cameraFront);
    
    lookAtPoint = VectorAdd(cameraFront,cam);
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
