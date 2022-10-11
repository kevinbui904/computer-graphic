// Written by Shannon Liu and Thien K. M. Bui

/* On macOS, compile with...
	clang 310mainCamera.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
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
#include "300isometry.c"
#include "300camera.c"

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
	rgbd[0] = unif[UNIFR];
	rgbd[1] = unif[UNIFG];
	rgbd[2] = unif[UNIFB];
	rgbd[3] = vary[VARYZ];
}


shaShading sha;
texTexture texture;
const texTexture *textures[1] = {&texture};
const texTexture **tex = textures;
meshMesh meshRedBox;
meshMesh meshGreenBox;
meshMesh meshSphere;
double unifRedBox[3 + 16 + 16] = {
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

double unifGreenBox[3 + 16 + 16] = {
	0.0, 1.0, 0.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

double unifSphere[3 + 16 + 16] = {
	1.0, 1.0, 1.0,
	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0,

	1.0, 0.0, 0.0, 0.0,
	0.0, 1.0, 0.0, 0.0,
	0.0, 0.0, 1.0, 0.0,
	0.0, 0.0, 0.0, 1.0};

double rotationAngle = 0.0;
double rotationAngleRedBox = M_PI/4;

double translationVector[3] = {0.0, 0.0, -10.0};
double translationVectorSphere[3] = {1.0, 1.0, -20.0};

double viewport[4][4], proj[4][4], camInverseIsometry[4][4], finalProj[4][4];

depthBuffer buf;

//camera setup
camCamera camera;
double target[3] = {0.0, 0.0, -10.0};

//camera control
double theta = -(M_PI/2);
double phi = 0;
double rho = 10;
double fovy = M_PI/6;

void render(void)
{
    camLookAt(&camera, target, rho, phi, theta);
    camSetFrustum(&camera, fovy, 10, 10, 512,512);
    camGetProjectionInverseIsometry(&camera, proj);

    vecCopy(16, (double *)proj, &unifRedBox[UNIFPROJ]);
    vecCopy(16, (double *)proj, &unifGreenBox[UNIFPROJ]);
    vecCopy(16, (double *)proj, &unifSphere[UNIFPROJ]);

	pixClearRGB(0.0, 0.0, 0.0);
	depthClearDepths(&buf, INT_MAX);
	meshRender(&meshRedBox, &buf, viewport, &sha, unifRedBox, tex);
	meshRender(&meshGreenBox, &buf, viewport, &sha, unifGreenBox, tex);
	meshRender(&meshSphere, &buf, viewport, &sha, unifSphere, tex);
}

void handleKeyUp(
	int key, int shiftIsDown, int controlIsDown, int altOptionIsDown,
	int superCommandIsDown)
{
	switch(key){
		case GLFW_KEY_ENTER:
			if (texture.filtering == texLINEAR)
				texSetFiltering(&texture, texNEAREST);
			else
				texSetFiltering(&texture, texLINEAR);
			render();
			break;
		case GLFW_KEY_P:
			if (camera.projectionType == camORTHOGRAPHIC){
				camSetProjectionType(&camera, camPERSPECTIVE);
				render();
			}
			else{
				camSetProjectionType(&camera, camORTHOGRAPHIC);
				render();
			}
			printf("ProjType current: %d\n", camera.projectionType);
			break;
        
        //make theta smaller, which rotate camera to the left of XY plane
        case GLFW_KEY_LEFT:
            theta -= M_PI/180;
            render();
            break;
        //the other direction
        case GLFW_KEY_RIGHT:
            theta += M_PI/180;
            render();
            break;
        
        //make phi smaller, which rotate camera down along the zy plane
        case GLFW_KEY_DOWN:
            phi -= M_PI/180;
            render();
            break;
        case GLFW_KEY_UP:
            phi += M_PI/180;
            render();
            break;

        //decreasing fovy and increasing distance
        //simulate a dolly-zooming in effect
        case GLFW_KEY_O:
            fovy -= M_PI/180;
            rho -= 0.2;
            render();
            break; 
        
        case GLFW_KEY_L:
            fovy += M_PI/180;
            rho += 0.2;
            render();
            break;
		default:
			printf("undefined behavior, we'll implement this later\n");
	}
}

void handleTimeStep(double oldTime, double newTime)
{
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
	// render();
}

int main(void)
{
	if (pixInitialize(512, 512, "Main World") != 0)
		return 1;

	if (texInitializeFile(&texture, "./nyan.jpg") != 0)
	{
		pixFinalize();
		return 2;
	}

	if (mesh3DInitializeBox(&meshRedBox, -0.5, 0.5, -1.5, -0.5, -1.5, -1.0) != 0)
    {
     texFinalize(&texture);
     pixFinalize();
     return 3;
    }

	if (mesh3DInitializeBox(&meshGreenBox, -2.0, 2.0, -1.5, -0.5, -5.0, 0.0) != 0)
    {
     texFinalize(&texture);
	 meshFinalize(&meshRedBox);
     pixFinalize();
     return 4;
    }

	if (mesh3DInitializeSphere(&meshSphere, 0.5, 30, 30) != 0)
	{
		texFinalize(&texture);
		meshFinalize(&meshRedBox);
		meshFinalize(&meshGreenBox);
		pixFinalize();
		return 5;
	}

	if(depthInitialize(&buf, 512,512) != 0){
		texFinalize(&texture);
		pixFinalize();
		meshFinalize(&meshRedBox);
		meshFinalize(&meshGreenBox);
		meshFinalize(&meshSphere);
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

	//configure the camera
	camSetProjectionType(&camera, camORTHOGRAPHIC);

	//setting up viewport
	mat44Viewport(512, 512, viewport);
	
	//setting up the rotation angles so that it is centered
	double isomGreenBox[4][4], isomRedBox[4][4], isomSphere[4][4];
	double rotation[3][3];
	double axis[3] = {1.0 / sqrt(3.0), 1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
	mat33AngleAxisRotation(rotationAngle, axis, rotation);
	mat44Isometry(rotation, translationVector, isomGreenBox);

	mat33AngleAxisRotation(rotationAngleRedBox, axis, rotation);
	mat44Isometry(rotation, translationVector, isomRedBox);

	mat33AngleAxisRotation(rotationAngle, axis, rotation);
	mat44Isometry(rotation, translationVectorSphere, isomSphere);

    //setting UNIF for first render
	vecCopy(16, (double *)isomRedBox, &unifRedBox[UNIFMODELING]);
	vecCopy(16, (double *)isomGreenBox, &unifGreenBox[UNIFMODELING]);
	vecCopy(16, (double *)isomSphere, &unifSphere[UNIFMODELING]);

	render();
	pixSetKeyUpHandler(handleKeyUp);
	pixSetTimeStepHandler(handleTimeStep);
	pixRun();
	meshFinalize(&meshRedBox);
	meshFinalize(&meshGreenBox);
	meshFinalize(&meshSphere);
	texFinalize(&texture);
	depthFinalize(&buf);
	pixFinalize();
	return 0;
}
