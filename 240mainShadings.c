
/* Contributors: Shannon Liu & Kevin Bui*/

/* On macOS, compile with...c
    clang 230mainHomogeneous.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>

#include "040pixel.h"

#include "080vector.c"
#include "230matrix.c"
#include "150texture.c"
#include "220shading.c"
/* New! We no longer need to include these files after shadeFragment and 
shadeVertex. So instead we include them up here. It's good C style to have all 
#includes in one section near the top of the file. */
#include "220triangle.c"
#include "220mesh.c"
#include "190mesh2D.c"

#define ATTRX 0
#define ATTRY 1
#define ATTRS 2
#define ATTRT 3
#define VARYX 0
#define VARYY 1
#define VARYS 2
#define VARYT 3

#define UNIFMODELING 0
#define UNIFR 9
#define UNIFG 10
#define UNIFB 11

#define TEXR 0
#define TEXG 1
#define TEXB 2



double rotationAngle = 0.0;
double deltaX = 0.0;
double deltaY = 0.0;
int holdingUp = 0;
int holdingDown = 0;


double circTranslateX = 0;

void shadeVertex(
        int unifDim, const double unif[], int attrDim, const double attr[], 
        int varyDim, double vary[]) {

        double attrHomog[3] = {attr[ATTRX], attr[ATTRY], 1};
        double varyHomog[3]; // declare size 3 array for homogeonated 

        mat331Multiply((double(*)[3])(&unif[UNIFMODELING]), attrHomog, varyHomog);

        //input varyHomog into vary arrays for mesh rendering
        vary[VARYX] = varyHomog[VARYX];
        vary[VARYY] = varyHomog[VARYY];
        vary[VARYS] = attr[ATTRS];
        vary[VARYT] = attr[ATTRT];
}

void shadeFragment(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        int varyDim, const double vary[], double rgb[3]) {

    texSample(tex[0], vary[VARYS], vary[VARYT], rgb);

    // vecModulate(3, sample, &unif[UNIFR], rgb);
}


/**
 * shadeVertexCirc
 * vertex shader for circle, used in NyanCat game
 * */
void shadeVertexCirc(
        int unifDim, const double unif[], int attrDim, const double attr[], 
        int varyDim, double vary[]) {

        // double attrHomog[3] = {attr[ATTRX], attr[ATTRY], 1};
        // double varyHomog[3]; // declare size 3 array for homogeonated 

        // mat331Multiply((double(*)[3])(&unif[UNIFMODELING]), attrHomog, varyHomog);

        // input varyHomog into vary arrays for mesh rendering
        vary[VARYX] = attr[ATTRX] + unif[3];
        vary[VARYY] = attr[ATTRY];
        vary[VARYS] = attr[ATTRS];
        vary[VARYT] = attr[ATTRT];
}

/**
 * shadeFragmentCirc
 * 
 * */
void shadeFragmentCirc(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        int varyDim, const double vary[], double rgb[3]) {

    vecCopy(3, unif, rgb);

    // vecModulate(3, sample, &unif[UNIFR], rgb);
}

//shader for nyan cat
shaShading shaNyan;
texTexture texture;
const texTexture *textures[1] = {&texture};
const texTexture **tex = textures;
meshMesh meshNyan;
double unif[9];
double translationVector[2];


//shader for circle (obstacle)
meshMesh meshCirc;
shaShading shaCirc;
texTexture textureCir;
const texTexture *texturesCirc[1] = {&texture};
const texTexture **texCirc = texturesCirc;
double unifCir[4] = {1.0,1.0,0.0, 0};
double translationVectorCir[2];



void render(void) {
    pixClearRGB(0.0, 0.0, 0.0);
    meshRender(&meshNyan, &shaNyan, unif, tex);
    meshRender(&meshCirc, &shaCirc, unifCir, texCirc);
}

void handleKeyUp(
        int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, 
        int superCommandIsDown) {

    switch(key){
        case GLFW_KEY_ENTER:
            if (texture.filtering == texLINEAR)
                texSetFiltering(&texture, texNEAREST);
            else
                texSetFiltering(&texture, texLINEAR);
            render();
            break;
        case GLFW_KEY_UP:
            holdingUp = 0;
            break;
        case GLFW_KEY_DOWN:
            holdingDown = 0;
            break;

        default:
            printf("not yet handled\n");
    }

}

void handleKeyDown(
        int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, 
        int superCommandIsDown) {

    switch(key){
        case GLFW_KEY_ENTER:
            if (texture.filtering == texLINEAR)
                texSetFiltering(&texture, texNEAREST);
            else
                texSetFiltering(&texture, texLINEAR);
            render();
            break;
        case GLFW_KEY_UP:
            holdingUp = 1;
            break;
        case GLFW_KEY_DOWN:
            holdingDown = 1;
            break;

        default:
            printf("not yet handled\n");
    }

}
void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

    if(holdingUp == 1){
        deltaY = deltaY + 2;
    }

    if(holdingDown == 1){
        deltaY = deltaY - 2;
    }

    translationVector[0] = deltaX;
    translationVector[1] = deltaY;
    
    unifCir[3] -= cos(newTime) * 10.0;

    double isom[3][3];
    mat33Isometry(rotationAngle, translationVector, isom);
    vecCopy(9, (double *)isom, &unif[UNIFMODELING]);

    render();
}

int main(void) {
    if (pixInitialize(512, 512, "Shader Program") != 0)
        return 1;
    if (texInitializeFile(&texture, "./nyan.jpg") != 0) {
        pixFinalize();
        return 2;
    }
// meshMesh *mesh, double left, double right, double bottom, double top

    if (mesh2DInitializeRectangle(&meshNyan, 10.0, 100.0, 10.0, 100.0) != 0) {
        texFinalize(&texture);
        pixFinalize();
        return 3;
    }
    texSetFiltering(&texture, texNEAREST);
    texSetLeftRight(&texture, texCLIP);
    texSetTopBottom(&texture, texCLIP);
    /* New! The shader program now records which shader functions to use. */


    //make mesh for circle
    if (mesh2DInitializeEllipse(&meshCirc, 400.0, 200.0, 20.0, 20.0, 15) != 0) {
        printf("unable to load mesh for circle, exiting...\n");
        exit(1);
        return 3;
    }

    shaNyan.unifDim = 9;
    shaNyan.attrDim = 2 + 2;
    shaNyan.varyDim = 2 + 2;
    shaNyan.texNum = 1;
    shaNyan.shadeVertex = shadeVertex;
    shaNyan.shadeFragment = shadeFragment;

    shaCirc.unifDim = 9;
    shaCirc.attrDim = 2 + 2;
    shaCirc.varyDim = 2 + 2;
    shaCirc.texNum = 0;
    shaCirc.shadeVertex = shadeVertexCirc;
    shaCirc.shadeFragment = shadeFragmentCirc;

    // render();
    pixSetKeyUpHandler(handleKeyUp);
    pixSetKeyDownHandler(handleKeyDown);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
    meshFinalize(&meshNyan);
    meshFinalize(&meshCirc);
    texFinalize(&texture);
    texFinalize(&textureCir);
    pixFinalize();
    return 0;
}

