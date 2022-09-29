/* Contributors: Shannon Liu & Kevin Bui*/

/* On macOS, compile with...
    clang 200mainAbstracted.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
On Ubuntu, compile with...
    cc 160mainAbstracted.c 040pixel.o -lglfw -lGL -lm -ldl
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "040pixel.h"

#include "080vector.c"
#include "100matrix.c"
#include "150texture.c"
#include "200shading.c"

/* It is important that shadeFragment correctly parse the data that we give it. 
To help avoid errors in communication, we set up some constants. Notice that the 
documentation for the new triRender requires ATTRX to be 0 and ATTRY to be 1. */
#define ATTRX 0
#define ATTRY 1
#define ATTRS 2
#define ATTRT 3


#define UNIFTRANSLX 0
#define UNIFTRANSLY 1
#define UNIFTHETA 2

#define TEXR 0
#define TEXG 1
#define TEXB 2

#define VARYX 0
#define VARYY 1
#define VARYS 2
#define VARYT 3

/* attr has already been interpolated from the vertex attributes. tex is an 
array of texNum elements, each of which is of type texTexture *. rgb is the 
output parameter. The other parameters should be self-explanatory. This function 
is not allowed to read from any variables other than its arguments and local 
variables. It is not allowed to write to any variables except for its rgb 
parameter and local variables. In particular, it cannot use any global 
variables. */
void shadeFragment(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        int attrDim, const double vary[], double rgb[3]) {
		
		texSample(*tex,vary[VARYS],vary[VARYT],rgb);
		//set rgb to uniform colors for modulating later
}


/*
vertexShader, adapted from Professor Josh Davis' code
*/
void shadeVertex(
int unifDim, const double unif[], int attrDim, const double attr[],
int varyDim, double vary[]) {
		vary[VARYX] = attr[ATTRX];
        vary[VARYY] = attr[ATTRY];

        double rot[2][2];
        mat22Rotation(unif[UNIFTHETA], rot);
        mat221Multiply(rot, attr, vary);

        vary[VARYX] += unif[UNIFTRANSLX];
        vary[VARYY] += unif[UNIFTRANSLY];
        vary[VARYS] = attr[ATTRS];
        vary[VARYT] = attr[ATTRT];
}

/* We have to include triangle.c after defining shadeFragment, because triRender 
refers to shadeFragment. (Later in the course we handle this issue better.) */
#include "170triangle.c"
#include "200mesh.c"
#include "190mesh2D.c"

/* This struct is initialized in main() below. */
shaShading sha;
meshMesh mesh;
/* Here we make an array of one texTexture pointer, in such a way that the 
const qualifier can be enforced throughout the surrounding code. C is confusing 
for stuff like this. Don't worry about mastering C at this level. It doesn't 
come up much in our course. */
texTexture texture1;
const texTexture *textures[1] = {&texture1};
const texTexture **tex = textures;

double unif[3] = {0,0,0};
void handleTimeStep(double oldTime, double newTime) {
	if (floor(newTime) - floor(oldTime) >= 1.0)
		printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

	//unif denote how much to shift x,y as time changes
	unif[UNIFTRANSLX] += (newTime - oldTime);
	unif[UNIFTRANSLY] += (newTime - oldTime);
	unif[UNIFTHETA] += (newTime - oldTime) / 10;
	pixClearRGB(0.0, 0.0, 0.0);
    meshRender(&mesh, &sha, unif, tex);

}

int main(void) {
	if (pixInitialize(512, 512, "Abstracted") != 0)
		return 1;

	if (texInitializeFile(&texture1, "./nyan.jpg") != 0) {
		pixFinalize();
		return 2;
	}

    texSetFiltering(&texture1, texNEAREST);
    texSetLeftRight(&texture1, texREPEAT);
    texSetTopBottom(&texture1, texREPEAT);

    if (mesh2DInitializeEllipse(&mesh, 300.0, 300.0, 100.0, 50.0, 100)!=0){
        pixFinalize();
        return 1;
    }

    sha.unifDim = 3;
    sha.attrDim = 2 + 2;
    sha.texNum = 1;
	sha.varyDim = 2 + 2;

    // pixSetKeyUpHandler(handleKeyUp);
    pixSetTimeStepHandler(handleTimeStep);


    pixRun();
    
    texFinalize(&texture1);
    meshFinalize(&mesh);
    pixFinalize();
    return 0;
}