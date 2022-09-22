/* Contributors: Shannon Liu & Kevin Bui*/

/* On macOS, compile with...
    clang 150mainTexturing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
On Ubuntu, compile with...
    cc 100mainTesting.c 040pixel.o -lglfw -lGL -lm -ldl
*/

#include <stdio.h>
#include <math.h>
#include "040pixel.h"

#include "080vector.c"
#include "100matrix.c"
#include "150texture.c"
#include "140triangle.c"

double a[2] = {60.0, 100.0};
double b[2] = {244.0, 50.0};
double c[2] = {380.0, 480.0};
double rgb[3] = {1.0, 1.0, 1.0};
double alpha[2] = {0.0, 1.0};
double beta[2] = {1.0, 0.0};
double gam[2] = {1.0, 1.0};
double angle = 0.0;
texTexture img;

int filtering = texLINEAR;
int topBottom = texREPEAT;

void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
        int altOptionIsDown, int superCommandIsDown) {
    //Changes filtering type when enter is pressed
    if (key == GLFW_KEY_ENTER){
        if(filtering == texLINEAR){
            filtering = texNEAREST;
        }
        else{
            filtering = texLINEAR;
        }
    }
}

void handleTimeStep(double oldTime, double newTime) {

    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));

        //updates rendering when every time update
        texSetFiltering(&img, filtering);
        triRender(a, b, c, rgb, &img, alpha, beta, gam);
}

int main(void) {
    if (pixInitialize(512, 512, "Testing") != 0)
        return 1;

    texInitializeFile(&img, "./140imageCat.jpg");
    //texInitializeFile(&img, "./facesmall.jpg");
    pixSetKeyUpHandler(handleKeyUp);
    pixSetTimeStepHandler(handleTimeStep);

    pixClearRGB(0.0, 0.0, 0.0);
    pixRun();
    
    texFinalize(&img);
    pixFinalize();
    return 0;
}