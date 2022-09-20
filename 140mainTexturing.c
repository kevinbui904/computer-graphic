/**
 * @file 140mainTexturing.c
 * @author Thien K. M. Bui <buik@carleton.edu> and Shannon Liu
 * @brief texture mapping for computer graphic
 * @version 0.1
 * @date 2022-09-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "040pixel.h"

#include "080vector.c"
#include "100matrix.c"
#include "140texture.c"
#include "140triangle.c"


double a[2] = {144.0, -156.0};
double b[2] = {244.0, 244.0};
double c[2] = {-226.0, -226.0};
double rgb[3] = {1.0, 1.0, 1.0};
double angle = 0.0;

double alpha[2] = {1.0, 0.0};
double beta[2] = {0.0, 1.0};
double gam[2] = {1.0, 1.0};

double A[2] = {0, 0};
double B[2] = {512, 0};
double C[2] = {0, 512};

void handleTimeStep(double oldTime, double newTime)
{
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/sec\n", 1.0 / (newTime - oldTime));
}

int main(void)
{

    if (pixInitialize(512, 512, "Testing") != 0)
        return 1;

    texTexture newTexture;
    texInitializeFile(&newTexture, "./140imageCat.jpg");
    triRender(A, B, C, rgb, &newTexture, alpha, beta, gam);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
    texFinalize(&newTexture);
    pixFinalize();
    return 0;
}
