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
#include "simplefont.h"
#include <stdlib.h>
#include <string.h>

mat4 projectionMatrix;
vec3 cam = {50, 20, 40};
vec3 rocketPoint = {50, 50, 50};
vec3 rocketTopPoint = {50, 60, 50};
vec3 rocketVel = {0,0,0};
vec3 cameraUp = {0,1,0};
vec3 cameraFront;
bool firstMouse = true;
int lastX,lastY;
float pi = 3.1415;
float yaw, pitch;
float waveSpeed = 0.01f;
float moveFactor = 0.1;
int texwidth;
int texheight;
int waterLevel = 3;
GLfloat tiltAngle;
bool isGameOver = false;

// vertex array object
Model *m, *m2, *tm,*tm2,*rocketFire, *skybox,*box[6],*rocketObject,*waterModel,*terrainArray[4];

// Reference to shader program
GLuint program,programSky,programWater, programRocket;
GLuint tex1, tex2, cubeMap,texRocket,dudvTex,NormalTex,refractTex;
TextureData ttex,water,fire,ttex2;
TextureData skyTex[6];

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 2000.0);

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

	LoadTGATextureSimple("grass.tga", &tex1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	// Load terrain data
	LoadTGATextureData("maskHeight.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	LoadTGATextureData("maskHeight2.tga", &ttex2);
	tm2 = GenerateTerrain(&ttex);
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
    LoadTGATextureSimple("Objects/redstoneRocket2.tga", &texRocket);
    
    //rocket fire
    LoadTGATextureData("grass.tga", &fire);
    rocketFire = GenerateTerrain(&fire);

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

void drawTerrain(vec3 cam,vec3 rocketPoint, mat4 camMatrix)
{
	int terrainOffset = 510;
	vec3 dirVect = VectorSub(Normalize(rocketPoint),Normalize(cam));
	float xPosPatch = floor(cam.x/512);
	float zPosPatch = floor(cam.z/512);

	mat4 terrainTrans = T(xPosPatch*terrainOffset,0,zPosPatch*terrainOffset);
	mat4 total = Mult(camMatrix, terrainTrans);
 
    float frustumAngle = 1; //radians
    mat4 planeRotPos = Ry(frustumAngle);
    mat4 planeRotNeg = Ry(-frustumAngle);
    
    vec3 rightVect = MultVec3(planeRotPos,dirVect);
    vec3 leftVect = MultVec3(planeRotNeg,dirVect);
    
    vec3 rightPlane = CrossProduct(rightVect,SetVector(0,1,0));
    vec3 leftPlane = CrossProduct(leftVect,SetVector(0,-1,0));
    
    vec3 refPoint = VectorSub(cam,ScalarMult(Normalize(dirVect),15));
    
    float rightD = -DotProduct(rightPlane,refPoint);
    float leftD = -DotProduct(leftPlane,refPoint);
    
    vec4 rightHomPlane;
    rightHomPlane.x = rightPlane.x;
    rightHomPlane.y = rightPlane.y;
    rightHomPlane.z = rightPlane.z;
    rightHomPlane.w = rightD;
    
    vec4 leftHomPlane;
    leftHomPlane.x = leftPlane.x;
    leftHomPlane.y = leftPlane.y;
    leftHomPlane.z = leftPlane.z;
    leftHomPlane.w = leftD;
    
    
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
    
    for (int i = -3; i<3; i++) {
        for (int j = -3; j<3; j++) {
            vec4 gridPoint;
            gridPoint.x = (i + xPosPatch)*terrainOffset;
            gridPoint.y = 0;
            gridPoint.z = (j + zPosPatch)*terrainOffset;
            gridPoint.w = 1;
            
            int scalarGrid1 = rightHomPlane.x*gridPoint.x + rightHomPlane.y*gridPoint.y + rightHomPlane.z*gridPoint.z + rightHomPlane.w*gridPoint.w;
            int scalarGrid2 = leftHomPlane.x*gridPoint.x + leftHomPlane.y*gridPoint.y + leftHomPlane.z*gridPoint.z + leftHomPlane.w*gridPoint.w;
            if (scalarGrid1<0 && scalarGrid2<0) {
                
                mat4 terrainTrans2 = T(terrainOffset*i,0,terrainOffset*j);
                terrainTrans2 = T(terrainOffset*i,0,terrainOffset*j);
                mat4 total2 = Mult(total, terrainTrans2);
                
                glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total2.m);
                DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
            }
    }
    }
}

void drawWater(vec3 cam, mat4 camMatrix)
{
	float waterOffset = 510;
	float xPosPatch = floor(cam.x/waterOffset);
	float zPosPatch = floor(cam.z/waterOffset);

	mat4 scaleWater = S(3,1,3);
	mat4 waterModelView = T(-440 + xPosPatch*waterOffset,waterLevel,-440+ zPosPatch*waterOffset);
    mat4 waterModel = Mult(waterModelView,scaleWater);

	glUniformMatrix4fv(glGetUniformLocation(programWater, "mdlMatrix"), 1, GL_TRUE, waterModel.m);
}

void display(void)
{
	//Update Cam to follow Rocket
	cam = VectorSub(rocketPoint,cameraFront);
    waterCheck(&rocketPoint, &waterLevel);
    
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
    
    GLfloat gravVel = 0.02f;
    float rocketOffset = 10;
    if (rocketPoint.y > rocketOffset)
    {
    	rocketVel.y = rocketVel.y-gravVel;
    }
    
    rocketPoint = VectorAdd(rocketPoint,rocketVel);
	
	mat4 total, modelView, camMatrix;
	printError("pre display");

	// Key handler for update of Rocket position and orientation, camera pos etc
	mat4 rocketRotate = keyHandler(&cam,&cameraUp,tm, &rocketPoint, &rocketVel, &rocketTopPoint);
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
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//send to shaders & draw
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "camMatrix"), 1, GL_TRUE, modelView.m);
    glUniform3f(glGetUniformLocation(program, "camPos"), cam.x, cam.y,cam.z);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1); // Bind Our Texture tex1
	drawTerrain(cam,rocketPoint, camMatrix);
    glDisable(GL_BLEND);
   
	//----------
	// WATER
	//----------

   // glEnable (GL_BLEND);
    //glBlendFunc (GL_ONE, GL_ONE);
   // glBlendFunc(GL_ONE_MINUS_DST_ALPHA,GL_DST_ALPHA);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    
    glUseProgram(programWater);
	glActiveTexture(GL_TEXTURE1); //IS THIS NESSESARY?
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);

	//Transformation matrix for water

    mat3 inverseCam = InvertMat3(mat4tomat3(camMatrix));

	moveFactor = moveFactor + waveSpeed*t;
	moveFactor = fmod(moveFactor,1) + 0.1;

	drawWater(cam,camMatrix);
	glUniformMatrix4fv(glGetUniformLocation(programWater, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix3fv(glGetUniformLocation(programWater, "InvCamMatrix"), 1, GL_TRUE, inverseCam.m);
    glUniform3f(glGetUniformLocation(programWater, "camPos"), cam.x, cam.y,cam.z);

    glUniform1f(glGetUniformLocation(programWater, "moveFactor"), moveFactor);
	glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, dudvTex);
	glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, NormalTex);

	DrawModel(waterModel, programWater, "inPosition", "inNormal", NULL);

	glDisable(GL_BLEND);


	// --------
	// ROCKET
	//---------
	// translate and rotate Rocket
    mat4 scaleRocket = S(0.5,0.25,0.5);
    mat4 scaleFire = S(0.005,-0.4*fabsf(sin(10*t)),0.005);
    mat4 fireTrans = T(-1.4,0,-1.4);
	glUseProgram(programRocket);
    mat4 rocketTrans = Mult(T(rocketPoint.x,rocketPoint.y,rocketPoint.z),rocketRotate);
	mat4 rocketTotal = Mult(camMatrix, rocketTrans);
    rocketTotal = Mult(rocketTotal,scaleRocket);
    mat4 fireTotal = Mult(rocketTotal,fireTrans);
    fireTotal = Mult(fireTotal,scaleFire);
	
	// bind texture, send to shader & draw model
	glActiveTexture(GL_TEXTURE0);

	glBindTexture(GL_TEXTURE_2D, texRocket );
	glUniform1i(glGetUniformLocation(programRocket, "objectFlag"), 1);
	glUniform1i(glGetUniformLocation(programRocket, "texRocket"), 0);
	glUniformMatrix4fv(glGetUniformLocation(programRocket, "mdlMatrix"), 1, GL_TRUE, rocketTotal.m);
	glUniformMatrix4fv(glGetUniformLocation(programRocket, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    
	DrawModel(rocketObject, programRocket, "inPosition", "inNormal", "inTexCoord");
    
    glUniform1i(glGetUniformLocation(programRocket, "objectFlag"), 0);
    glUniformMatrix4fv(glGetUniformLocation(programRocket, "mdlMatrix"), 1, GL_TRUE, fireTotal.m);
    
    if(glutKeyIsDown('f'))
    {
        DrawModel(rocketFire, programRocket, "inPosition", "inNormal", "inTexCoord");
    }
	
	printError("display 2");
    if(isGameOver)
    {
       sfDrawString(270, 100, "game over");
    }
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

bool gameOver(bool gameOver)
{
    isGameOver = true;
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("Dr. Rocket");
    sfMakeRasterFont(); // init font
    sfSetRasterSize(600, 200);
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
