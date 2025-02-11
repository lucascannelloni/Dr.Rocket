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
vec3 startPoint = {103, 76, 46};
vec3 rocketPoint;
vec3 rocketTopPoint = {103, 80, 46};
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
int waterLevel = 67;
float fuel;
float fullTank = 80;
GLfloat tiltAngle;
bool isGameOver = false;

// vertex array object
Model *m, *m2, *tm,*tm2,*rocketFire, *skybox,*box[6],*rocketObject,*waterModel;

// Reference to shader program
GLuint program,programSky,programWater, programRocket;
GLuint tex1, tex2, cubeMap,texRocket,dudvTex,NormalTex,refractTex;
TextureData ttex,water,fire,ttex2;
TextureData skyTex[6];

void init(void)
{
    rocketPoint = startPoint;
    fuel = fullTank;
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
    LoadTGATextureSimple("Objects/redstoneRocket3.tga", &texRocket);
    
    //rocket fire
    LoadTGATextureData("grass.tga", &fire);
    rocketFire = GenerateTerrain(&fire);

}

void drawTerrain(vec3 cam,vec3 rocketPoint, mat4 camMatrix, mat3 inverseCam, float moveFactor)
{
    glUseProgram(program);
    
	int terrainOffset = 255;
    float diagTerrain = terrainOffset/sqrt(2);
	vec3 dirVect = VectorSub(rocketPoint,cam);

	float xPosPatch = floor(cam.x/255);
	float zPosPatch = floor(cam.z/255);

	mat4 terrainTrans = T(xPosPatch*terrainOffset,0,zPosPatch*terrainOffset);
	mat4 total = Mult(camMatrix, terrainTrans);

    float frustumAngle = pi/4;

    mat4 planeRotPos = Ry(frustumAngle);
    mat4 planeRotNeg = Ry(-frustumAngle);
    
    
    vec3 rightVect = MultVec3(planeRotNeg,dirVect);
    vec3 leftVect = MultVec3(planeRotPos,dirVect);
    
    vec3 rightPlane = Normalize(CrossProduct(SetVector(0,1,0),rightVect));
    vec3 leftPlane = Normalize(CrossProduct(SetVector(0,-1,0),leftVect));
    
    float rightD = DotProduct(rightPlane,cam);
    float leftD = DotProduct(leftPlane,cam);
    
    float scalarGridRight;
    float scalarGridLeft;


    glUseProgram(programWater);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(dudvTex,2);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(NormalTex,3);
    glUniform1f(glGetUniformLocation(programWater,"moveFactor"),moveFactor);
    glUniformMatrix4fv(glGetUniformLocation(programWater, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniformMatrix3fv(glGetUniformLocation(programWater, "InvCamMatrix"), 1, GL_TRUE, inverseCam.m);
    glUniform3f(glGetUniformLocation(programWater, "camPos"), cam.x, cam.y,cam.z);
    glUniformMatrix4fv(glGetUniformLocation(programWater, "mdlMatrix"), 1, GL_TRUE, terrainTrans.m);
   // DrawModel(waterModel, programWater, "inPosition", "inNormal", NULL);
    //printf("Enter forloop\n");

    int numberOfPatches = 3;
    
    for (int i = -numberOfPatches; i<numberOfPatches+1; i++) {
        for (int j = -numberOfPatches; j<numberOfPatches+1; j++) {

            vec3 gridPoint1;
            gridPoint1.x = (i+ xPosPatch)*terrainOffset + terrainOffset/2;
            gridPoint1.y = 0;
            gridPoint1.z = (j+ zPosPatch)*terrainOffset + terrainOffset/2;
            
            scalarGridRight = DotProduct(gridPoint1,rightPlane);
            scalarGridLeft = DotProduct(gridPoint1,leftPlane);

            
            if (scalarGridRight<(rightD + diagTerrain) && scalarGridLeft<(leftD + diagTerrain))
            {
              //  printf("drawn patch\n");
                
                // TERRAIN
                glEnable(GL_POLYGON_SMOOTH);
                glUseProgram(program);
                mat4 terrainTrans2 = T(terrainOffset*i,0,terrainOffset*j);
                mat4 total2 = Mult(total, terrainTrans2);
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex1);

                glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total2.m);
                DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
                
                // WATER
                glUseProgram(programWater);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap);
                
                mat4 waterTot = Mult(terrainTrans,terrainTrans2);
                glUniformMatrix4fv(glGetUniformLocation(programWater, "mdlMatrix"), 1, GL_TRUE, waterTot.m);
                glUniformMatrix4fv(glGetUniformLocation(programWater, "mdlMatrixTerrain"), 1, GL_TRUE, total2.m);
                DrawModel(waterModel, programWater, "inPosition", "inNormal", NULL);
            }
    }
    }
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
    if (rocketPoint.y > rocketOffset && (rocketVel.x != 0 || rocketVel.y != 0 || rocketVel.z != 0))
    {
    	rocketVel.y = rocketVel.y-gravVel;
        rocketVel.x = 0.99*rocketVel.x;
        rocketVel.z = 0.99*rocketVel.z;
    }
    // Load tank if still
    if (rocketVel.x == 0 && rocketVel.y == 0 && rocketVel.z == 0 && fuel < fullTank)
    {
        fuel = fuel + 0.5;
    }
    
    rocketPoint = VectorAdd(rocketPoint,rocketVel);
	
	mat4 total, modelView, camMatrix;

	// Key handler for update of Rocket position and orientation, camera pos etc
	mat4 rocketRotate = keyHandler(&cam,&cameraUp,tm, &rocketPoint, &rocketVel, &rocketTopPoint, &fuel);
	camMatrix = lookAt(cam.x, cam.y, cam.z,
				rocketPoint.x, rocketPoint.y, rocketPoint.z,
				cameraUp.x,cameraUp.y,cameraUp.z);
    mat3 inverseCam = InvertMat3(mat4tomat3(camMatrix));

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
	// TERRAIN & WATER
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
	glBindTexture(GL_TEXTURE_2D, tex1);
    
    moveFactor = moveFactor + waveSpeed*t;
    moveFactor = fmod(moveFactor,0.9) + 0.1;
	drawTerrain(cam,rocketPoint, camMatrix, inverseCam, moveFactor);
    
	glDisable(GL_BLEND);


	// --------
	// ROCKET
	//---------
	// translate and rotate Rocket
    mat4 scaleRocket = S(0.25,0.125,0.25);
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

	glBindTexture(GL_TEXTURE_2D, texRocket);
	glUniform1i(glGetUniformLocation(programRocket, "objectFlag"), 1);
	glUniform1i(glGetUniformLocation(programRocket, "texRocket"), 0);
	glUniformMatrix4fv(glGetUniformLocation(programRocket, "mdlMatrix"), 1, GL_TRUE, rocketTotal.m);
	glUniformMatrix4fv(glGetUniformLocation(programRocket, "camMatrix"), 1, GL_TRUE, camMatrix.m);
    glUniform3f(glGetUniformLocation(programRocket, "camPos"), cam.x, cam.y,cam.z);
    
	DrawModel(rocketObject, programRocket, "inPosition", "inNormal", "inTexCoord");
    
    glUniform1i(glGetUniformLocation(programRocket, "objectFlag"), 0);
    glUniformMatrix4fv(glGetUniformLocation(programRocket, "mdlMatrix"), 1, GL_TRUE, fireTotal.m);
    
    if(glutKeyIsDown('f') && fuel > 0)
    {
        DrawModel(rocketFire, programRocket, "inPosition", "inNormal", "inTexCoord");
    }
    
    double num = fuel;
    char output[5];
    
    snprintf(output, 5, "%f", num);
    
    sfDrawString(400, 170, output);
    sfDrawString(400, 155, "Fuel:");
    
    if(isGameOver)
    {
       sfDrawString(270, 100, "game over");
        sfDrawString(220, 150, "press g to restart");
        if(glutKeyIsDown('g'))
        {
            isGameOver = false;
            rocketPoint = startPoint;
            tiltAngle = 0;
            fuel = fullTank;
        }
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

    if(pitch > 60.0f)
        pitch = 60.0f;
    if(pitch < -60.0f)
        pitch = -60.0f;

    cameraFront.x = cos(pi*yaw/180) * cos(pi*pitch/180);
    cameraFront.y = sin(pi*pitch/180);
    cameraFront.z = sin(pi*yaw/180) * cos(pi*pitch/180);
    
    cameraFront = Normalize(cameraFront);
    cameraFront = ScalarMult(cameraFront,10);
    
    cam = VectorSub(rocketPoint,cameraFront);
}

bool gameOver(bool gameOver)
{
    isGameOver = true;
    rocketVel = SetVector(0,0,0);
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
