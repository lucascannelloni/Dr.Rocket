// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif

#include "givenFiles/MicroGlut.h"
#include "givenFiles/GL_utilities.h"
#include "givenFiles/VectorUtils3.h"
#include "givenFiles/loadobj.h"
#include "givenFiles/LoadTGA.h"
#include "world.h"
#include "rocket.h"

mat4 projectionMatrix;
vec3 cam = {50, 20, 40};
vec3 rocketPoint = {70, 50, 50};
vec3 rocketVel = {0,0,0};
vec3 cameraUp = {0,1,0};
vec3 cameraFront;
bool firstMouse = true;
int lastX,lastY;
float pi = 3.1415;
float yaw, pitch;
float waveSpeed = 0.01f;
float moveFactor = 0;
int texwidth;
int texheight;
GLfloat tiltAngle;

// vertex array object
Model *m, *m2, *tm, *skybox,*box[6],*rocketObject,*waterModel;

// Reference to shader program
GLuint program,programSky,programWater, programRocket;
GLuint tex1, tex2, cubeMap,texRocket,dudvTex,NormalTex,refractTex;
TextureData ttex,water;
TextureData skyTex[6];

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 1000.0);

	// Load and compile shader
	program = loadShaders("Shaders/terrain.vert", "Shaders/terrain.frag");
    programRocket = loadShaders("Shaders/rocket.vert", "Shaders/rocket.frag");
	programSky = loadShaders("Shaders/skybox.vert","Shaders/skybox.frag");
	programWater= loadShaders("Shaders/water.vert","Shaders/water.frag");

	//TERRAIN
	glUseProgram(program);
	printError("init shader");
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex1"), 0); // Texture unit 0

	LoadTGATextureSimple("dirt.tga", &tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// Load terrain data
	LoadTGATextureData("newTerrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");

	//WATER
	LoadTGATextureData("waterlevel.tga", &water);
	waterModel = GenerateTerrain(&water);
	glUseProgram(programWater);
    
	glUniformMatrix4fv(glGetUniformLocation(programWater, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	glUniform1i(glGetUniformLocation(programWater, "cubeMap"), 1); // Texture unit 1
	
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, refractTex);
	glUniform1i(glGetUniformLocation(programWater, "refractTex"), 0);

	LoadTGATextureSimple("waterDUDV.tga", &dudvTex);
	glActiveTexture(GL_TEXTURE2);
	glUniform1i(glGetUniformLocation(programWater, "dudvMap"), 2);

	LoadTGATextureSimple("normalMap.tga", &NormalTex);
	glActiveTexture(GL_TEXTURE3);
	glUniform1i(glGetUniformLocation(programWater, "normalMap"), 3);

	//SKYBOX
	glUseProgram(programSky);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(programSky, "skyTex"), 0); // Texture unit 0
	
	// load skybox texture
    glActiveTexture(GL_TEXTURE0);
	loadTextures(&cubeMap, skyTex, box);

	//ROCKET
    //Load Rocket Model
    glUseProgram(programRocket);
    glActiveTexture(GL_TEXTURE0);
    rocketObject = LoadModelPlus("Objects/redstoneRocket.obj");
    glUniform1i(glGetUniformLocation(programRocket, "texRocket"), 0); // Texture unit 0
    glUniformMatrix4fv(glGetUniformLocation(programRocket, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
    LoadTGATextureSimple("grass.tga", &texRocket);

}


mat4 placeModelOnGround(float x, float z)
{
 	float y = heightCalc(x,z,tm);
 	printf("x %f\n", x);
 	printf("y %f\n", y);
 	printf("z %f\n", z);
 	mat4 trans = T(x,y,z);
 	return trans;
}

void display(void)
{
	//Update Cam to follow Rocket
	cam = VectorSub(rocketPoint,cameraFront);
    
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
    
    GLfloat gravVel = 0.05f;
    float rocketOffset = 10;
    if (rocketPoint.y > rocketOffset)
    {
    	rocketVel.y = rocketVel.y-gravVel;
    }

    rocketPoint = VectorAdd(rocketPoint,rocketVel);
	
	mat4 total, modelView, camMatrix;
	printError("pre display");

	// Key handler for update of Rocket position and orientation, camera pos etc
	mat4 rocketRotate = keyHandler(&cam,&cameraUp,tm, &rocketPoint, &rocketVel);
	camMatrix = lookAt(cam.x, cam.y, cam.z,
				rocketPoint.x, rocketPoint.y, rocketPoint.z,
				cameraUp.x,cameraUp.y,cameraUp.z);

	// -------
	// SKYBOX
	// -------
	modelView = IdentityMatrix();
	mat4 skyMatrix = camMatrix;
	skyMatrix.m[3] = 0;
	skyMatrix.m[7] = 0;
	skyMatrix.m[11] = 0;
	mat4 totalSky = Mult(skyMatrix,modelView);

	glUseProgram(programSky);
	glDisable(GL_DEPTH_TEST);
	glUniformMatrix4fv(glGetUniformLocation(programSky, "mdlMatrix"), 1, GL_TRUE, totalSky.m);
	printError("skybox display");
   
	for (int i = 0; i < 6; i++)
	{
		glBindTexture(GL_TEXTURE_2D, skyTex[i].texID);
		DrawModel(box[i], programSky, "inPosition", NULL, "inTexCoord");
	}
	glEnable(GL_DEPTH_TEST);
	
	//----------
	// TERRAIN
	//----------
	// send cubemap to terrain shaders as well (i think)
    glUseProgram(program);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
	
	//Transformation matrix for terrain
	mat4 terrainTrans = T(0,0,0);
	total = Mult(camMatrix, terrainTrans);


	//send to shaders & draw
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, modelView.m);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");


	//----------
	// WATER
	//----------

    //glEnable (GL_BLEND);
    //glBlendFunc (GL_ONE, GL_ONE);
    
    glUseProgram(programWater);
	glActiveTexture(GL_TEXTURE1); //IS THIS NESSESARY?
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	//Transformation matrix for water

    mat3 inverseCam = InvertMat3(mat4tomat3(camMatrix));
	mat4 transCam = T(cam.x,cam.y,cam.z);
	mat4 waterModelView = T(0,-10,0);

	moveFactor = moveFactor + waveSpeed*t;
	moveFactor = fmod(moveFactor,1);

	//send to shaders & draw
	//glEnable (GL_BLEND);
	//glBlendFunc (GL_ONE, GL_ONE);

	glUniformMatrix4fv(glGetUniformLocation(programWater, "mdlMatrix"), 1, GL_TRUE, waterModelView.m);
	glUniformMatrix4fv(glGetUniformLocation(programWater, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	glUniformMatrix4fv(glGetUniformLocation(programWater, "transCam"), 1, GL_TRUE, transCam.m);
    glUniformMatrix3fv(glGetUniformLocation(programWater, "InvCamMatrix"), 1, GL_TRUE, inverseCam.m);
    glUniform1f(glGetUniformLocation(programWater, "moveFactor"), moveFactor);
	glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dudvTex);
	glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, NormalTex);
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1

	DrawModel(waterModel, programWater, "inPosition", "inNormal", NULL);

	//glDisable(GL_BLEND);


	// --------
	// ROCKET
	//---------
	// translate and rotate Rocket
    mat4 scaleRocket = S(0.5,0.25,0.5);
	glUseProgram(programRocket);
    mat4 rocketTrans = Mult(T(rocketPoint.x,rocketPoint.y,rocketPoint.z),rocketRotate);
	mat4 rocketTotal = Mult(camMatrix, rocketTrans);
    rocketTotal = Mult(rocketTotal,scaleRocket);
	
	// bind texture, send to shader & draw model
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, texRocket );

	glUniform1i(glGetUniformLocation(programRocket, "texRocket"), 0);
	glUniformMatrix4fv(glGetUniformLocation(programRocket, "mdlMatrix"), 1, GL_TRUE, rocketTotal.m);
	glUniformMatrix4fv(glGetUniformLocation(programRocket, "camMatrix"), 1, GL_TRUE, camMatrix.m);
	DrawModel(rocketObject, programRocket, "inPosition", "inNormal", "inTexCoord");
	
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
    cameraFront = ScalarMult(cameraFront,50);
    
    cam = VectorSub(rocketPoint,cameraFront);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("Dr. Rocket");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
