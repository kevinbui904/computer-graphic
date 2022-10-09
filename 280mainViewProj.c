// Written by Shannon Liu and Thien K. M. Bui

/* On macOS, compile with...
	clang 280mainViewProj.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
On Ubuntu, compile with...
	cc 270main3D.c 040pixel.o -lglfw -lGL -lm -ldl
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "040pixel.h"

#include "250vector.c"
#include "280matrix.c"
#include "150texture.c"
#include "220shading.c"

#include "260depth.c"
#include "270triangle.c"
#include "280mesh.c"
#include "190mesh2D.c"
#include "250mesh3D.c"
#include "280camera.c"

#define ATTRX 0
#define ATTRY 1
#define ATTRZ 2
#define ATTRS 3
#define ATTRT 4
#define ATTRN 5
#define ATTRO 6
#define ATTRP 7
#define VARYX 0
#define VARYY 1
#define VARYZ 2
#define VARYW 3
#define VARYS 4
#define VARYT 5
#define UNIFR 0
#define UNIFG 1
#define UNIFB 2
#define UNIFMODELING 3
#define UNIFPROJ 19
#define TEXR 0
#define TEXG 1
#define TEXB 2

void shadeVertex(
	int unifDim, const double unif[], int attrDim, const double attr[],
	int varyDim, double vary[])
{

	double attrHomog[4] = {attr[0], attr[1], attr[2], 1.0};
	double world[4];
	//world coordinate
	mat441Multiply((double(*)[4])(&unif[UNIFMODELING]), attrHomog, world);
	//project our world coordinate into proj coordinate (NDC)

	//NOTE: our vary is the final coordinates, so we will use vary interchangeably
	// 		here to represent ndc coordinate
	mat441Multiply((double(*)[4])(&unif[UNIFPROJ]), world, vary);

	vary[VARYS] = attr[ATTRS];
	vary[VARYT] = attr[ATTRT];
}

void shadeFragment(
	int unifDim, const double unif[], int texNum, const texTexture *tex[],
	int varyDim, const double vary[], double rgbd[4])
{
	double sample[tex[0]->texelDim];
	texSample(tex[0], vary[VARYS], vary[VARYT], sample);
	// vecModulate(3, sample, &unif[UNIFR], rgb);
	rgbd[0] = sample[TEXR] * unif[UNIFR];
	rgbd[1] = sample[TEXG] * unif[UNIFG];
	rgbd[2] = sample[TEXB] * unif[UNIFB];
	rgbd[3] = vary[VARYZ];
}


shaShading sha;
texTexture texture;
const texTexture *textures[1] = {&texture};
const texTexture **tex = textures;
meshMesh mesh;
meshMesh mesh2;
double unif[3 + 16 + 16] = {
	1.0, 1.0, 1.0,

	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,

	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

double unif2[3 + 16 + 16] = {
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,


	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

double rotationAngle = 0.0;
double translationVector[2] = {256.0, 256.0};

double viewport[4][4], proj[4][4];

depthBuffer buf;

camCamera camera;

void render(void)
{
	pixClearRGB(0.0, 0.0, 0.0);
	depthClearDepths(&buf, INT_MAX);
	meshRender(&mesh, &buf, viewport, &sha, unif, tex);
	meshRender(&mesh2, &buf, viewport, &sha, unif2, tex);
}

void handleKeyUp(
	int key, int shiftIsDown, int controlIsDown, int altOptionIsDown,
	int superCommandIsDown)
{
	if (key == GLFW_KEY_ENTER)
	{
		if (texture.filtering == texLINEAR)
			texSetFiltering(&texture, texNEAREST);
		else
			texSetFiltering(&texture, texLINEAR);
		render();
	}
}

void handleTimeStep(double oldTime, double newTime)
{
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

	//this make the ball glow
	unif2[UNIFB] = sin(newTime);
	unif2[UNIFG] = cos(oldTime);
	rotationAngle += (newTime - oldTime);
	double isom[4][4];
	double rotation[3][3];
	double axis[3] = {1.0 / sqrt(3.0), 1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
	mat33AngleAxisRotation(rotationAngle, axis, rotation);
	mat44Isometry(rotation, translationVector, isom);
	vecCopy(16, (double *)isom, &unif[UNIFMODELING]);
	vecCopy(16, (double *)isom, &unif2[UNIFMODELING]);

	//vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	vecCopy(16, (double *)proj, &unif2[UNIFPROJ]);
	
	render();
}

int main(void)
{
	if (pixInitialize(512, 512, "Three Dimensions") != 0)
		return 1;

	if (texInitializeFile(&texture, "./nyan.jpg") != 0)
	{
		pixFinalize();
		return 2;
	}

	if (mesh3DInitializeBox(&mesh, -128.0, 128.0, -64.0, 64.0, -80.0, -34.0) != 0)
	{
		texFinalize(&texture);
		pixFinalize();
		return 3;
	}

	if (mesh3DInitializeSphere(&mesh2, 40, 30, 30) != 0)
	{
		texFinalize(&texture);
		meshFinalize(&mesh);
		pixFinalize();
		return 4;
	}

	if(depthInitialize(&buf, 512,512) != 0){
		texFinalize(&texture);
		pixFinalize();
		meshFinalize(&mesh);
		meshFinalize(&mesh2);
		return 5;
	}
	texSetFiltering(&texture, texNEAREST);
	texSetLeftRight(&texture, texREPEAT);
	texSetTopBottom(&texture, texREPEAT);
	sha.unifDim = 3 + 16 + 16;
	sha.attrDim = 3 + 2 + 3;
	sha.varyDim = 4 + 2;
	sha.shadeVertex = shadeVertex;
	sha.shadeFragment = shadeFragment;
	sha.texNum = 1;

	//set up camera
	double camSetup[6] = {0.0, 512.0, 0.0, 512.0, -200.0, -100.0};
	camSetProjection(&camera, camSetup);
	camSetProjectionType(&camera, camORTHOGRAPHIC);
	camGetOrthographic(&camera, proj);

	//setting up viewport
	mat44Viewport(512, 512, viewport);

	//shoving our projection matrix into our
	vecCopy(16, (double *)proj, &unif[UNIFPROJ]);
	vecCopy(16, (double *)proj, &unif2[UNIFPROJ]);

	render();
	pixSetKeyUpHandler(handleKeyUp);
	pixSetTimeStepHandler(handleTimeStep);
	pixRun();
	meshFinalize(&mesh);
	meshFinalize(&mesh2);
	texFinalize(&texture);
	depthFinalize(&buf);
	pixFinalize();
	return 0;
}
