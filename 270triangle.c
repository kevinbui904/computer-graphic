/* Contributors: Shannon Liu & Kevin Bui*/
/**
 * 270 triangle.c
 * 
 */
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>

void linearInterpolation(const int varyDim, const double a[], const double b[], const double c[], const double x[2],
                         const double AInv[2][2], const double betaMinAlpha[], const double gamMinAlpha[], double chi[])
{

    // create empty vectors and matrix to input
    double xMinusA[2], pq[2], pMult[varyDim], qMult[varyDim], pqMultAdd[varyDim];

    // subtract vector x from vector a
    vecSubtract(2, x, a, xMinusA);

    // multiply inverse of matrix A by x-a
    mat221Multiply(AInv, xMinusA, pq);

    // p*(beta - alpha)
    vecScale(varyDim, pq[0], betaMinAlpha, pMult);

    // q*(gamma - alpha)
    vecScale(varyDim, pq[1], gamMinAlpha, qMult);

    // p*(beta - alpha) + q*(gamma - alpha)
    vecAdd(varyDim, pMult, qMult, pqMultAdd);

    // alpha + (p*(beta - alpha) + q*(gamma - alpha))
    vecAdd(varyDim, a, pqMultAdd, chi);
}

/*
helper function for triRender
*/
void triRenderALeft(
    const shaShading *sha, depthBuffer *buf, const double unif[], const texTexture *tex[],
    const double a[], const double b[], const double c[])
{
    // printf("entered left triangle render\n");

    // Calculate matrix A based on vectors a, b, c
    double A[2][2] = {{b[0] - a[0], c[0] - a[0]}, {b[1] - a[1], c[1] - a[1]}};
    // invert matrix A
    double AInv[2][2];

    // det for backface culling
    double det = mat22Invert(A, AInv);

    // backface culling: do not print triangles with a negative determinant
    if (det < 0)
    {
        return;
    }

    double betaMinAlpha[sha->varyDim], gamMinAlpha[sha->varyDim];

    // beta - alpha
    vecSubtract(sha->varyDim, b, a, betaMinAlpha);

    // gamma - alpha
    vecSubtract(sha->varyDim, c, a, gamMinAlpha);

    double x[2], chi[sha->varyDim], final[4];
    int x0, x1;
    // Case if a0 = c0, prevent division by 0
    if (a[0] == c[0])
    {
        //viewport clipping from the left
        if(ceil(a[0]) < 0){
            x0 = 0;
        }
        else{
            x0 = ceil(a[0]);
        }

        //viewport clipping from the right (implemented straight into the for loop)
        //first section of for loop not included because C expects initialization code.
        //Since x0 is initialized outside, so the initialization is not include 
        //(just treat this like a while loop)
        for (; (x0 <= floor(b[0]) && x0 <= buf->width); x0++)
        {

            double upper = floor(c[1] + (b[1] - c[1]) / (b[0] - c[0]) * (x0 - c[0]));
            double lower = ceil(a[1] + (b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0]));
            
            //viewport clipping from the bottom
            if (lower < 0){
                x1 = 0;
            }
            else{
                x1 = lower;
            }
            
            //viewport clipping from the top
            for (; (x1 <= upper && x1 <= buf->height); x1++)
            {
                // printf("entered left print loops\n");
                x[0] = x0;
                x[1] = x1;

                //only renders if the pixel is within viewport
              
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                
                if (final[3] < depthGetDepth(buf,x0,x1)){
                    pixSetRGB(x0, x1, final[0], final[1], final[2]);
                    depthSetDepth(buf, x0, x1, final[3]);
                }
                
            }
        }
    }
    // Case if a0 = b0, prevent division by 0
    else if (a[0] == b[0])
    {
        if(ceil(a[0]) < 0){
            x0 = 0;
        }
        else{
            x0 = ceil(a[0]);
        }
        for (; (x0 <= floor(c[0]) && x0 <= buf->width); x0++)
        {

            double upper = floor(a[1] + (c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0]));
            double lower = ceil(b[1] + (c[1] - b[1]) / (c[0] - b[0]) * (x0 - b[0]));

            if(lower < 0){
                x1 = 0;
            }
            else{
                x1 = lower;
            }
            for (; (x1 <= upper && x1 <= buf->height); x1++)
            {
                // printf("entered left print loops\n");
                x[0] = x0;
                x[1] = x1;

                //only renders if the pixel is within viewport
               
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                
                if (final[3] < depthGetDepth(buf,x0,x1)){
                    pixSetRGB(x0, x1, final[0], final[1], final[2]);
                    depthSetDepth(buf, x0, x1, final[3]);
                }
                 
            }
        }
    }
    // Case if c0 <= b0
    else if (c[0] <= b[0])
    {
        if (ceil(a[0]) < 0){
            x0 = 0;
        }
        else{
            x0 = ceil(a[0]);
        }
        for (; (x0 <= floor(c[0]) && x0 <= buf->width); x0++)
        {

            double upper = floor(a[1] + ((c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0])));
            double lower = ceil(a[1] + ((b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0])));

            if(lower < 0){
                x1 = 0;
            }
            else{
                x1 = lower;
            }
            for (; (x1 <= upper && x1 <= buf->height); x1++)
            {
                // printf("entered left print loops\n");
                x[0] = x0;
                x[1] = x1;

                //only renders if the pixel is within viewport
                
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                
                if (final[3] < depthGetDepth(buf,x0,x1)){
                    pixSetRGB(x0, x1, final[0], final[1], final[2]);
                    depthSetDepth(buf, x0, x1, final[3]);
                }
                 
            }
        }
        if(floor(c[0]) + 1 < 0){
            x0 = 0;
        }
        else{
            x0 = floor(c[0]) + 1;
        }
        for (; (x0 <= floor(b[0]) && x0 <= buf->width); x0++)
        {

            double upper = floor(c[1] + (b[1] - c[1]) / (b[0] - c[0]) * (x0 - c[0]));
            double lower = ceil(a[1] + (b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0]));
            
            if(lower < 0){
                x1 = 0;
            }
            else{
                x1 = lower;
            }
            for (; (x1 <= upper && x1 <= buf->height); x1++)
            {
                // printf("entered left print loops\n");
                x[0] = x0;
                x[1] = x1;

                //only renders if the pixel is within viewport
                
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                
                if (final[3] < depthGetDepth(buf,x0,x1)){
                    pixSetRGB(x0, x1, final[0], final[1], final[2]);
                    depthSetDepth(buf, x0, x1, final[3]);
                }
                
            }
        }
    }
    // case if b0 < c0
    else
    {
        if(ceil(a[0]) < 0){
            x0 = 0;
        }
        else{
            x0 = ceil(a[0]);
        }
        for (; (x0 <= floor(b[0]) && x0 <= buf->width); x0++)
        {

            double upper = floor(a[1] + ((c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0])));
            double lower = ceil(a[1] + ((b[1] - a[1]) / (b[0] - a[0]) * (x0 - a[0])));

            if(lower < 0){
                x1 = 0;
            }
            else{
                x1 = lower;
            }
            for (; (x1 <= upper && x1 <= buf->height); x1++)
            {
                // printf("entered left print loops\n");
                x[0] = x0;
                x[1] = x1;
              
                //only renders if the pixel is within viewport
                
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                
                if (final[3] < depthGetDepth(buf,x0,x1)){
                    pixSetRGB(x0, x1, final[0], final[1], final[2]);
                    depthSetDepth(buf, x0, x1, final[3]);
                }
                
                
            }
        }

        if(floor(b[0]) + 1 < 0){
            x0 = 0;
        }
        else{
            x0 = floor(b[0]) + 1;
        }
        for (; (x0 <= floor(c[0]) && x0 <= buf->width); x0++)
        {

            double upper = floor(a[1] + (c[1] - a[1]) / (c[0] - a[0]) * (x0 - a[0]));
            double lower = ceil(b[1] + (c[1] - b[1]) / (c[0] - b[0]) * (x0 - b[0]));

            if(lower < 0){
                x1 = 0;
            }
            else{
                x1 = lower;
            }
            for (; (x1 <= upper && x1 <= buf->height); x1++)
            {
                // printf("entered left print loops\n");
                x[0] = x0;
                x[1] = x1;

                //only renders if the pixel is within viewport
              
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                
                if (final[3] < depthGetDepth(buf,x0,x1)){
                    pixSetRGB(x0, x1, final[0], final[1], final[2]);
                    depthSetDepth(buf, x0, x1, final[3]);
                }
                
                 
            }
        }
    }
}
/* Assumes that the 0th and 1th elements of a, b, c are the 'x' and 'y'
coordinates of the vertices, respectively (used in rasterization, and to
interpolate the other elements of a, b, c). */
void triRender(
    const shaShading *sha, depthBuffer *buf, const double unif[], const texTexture *tex[],
    const double a[], const double b[], const double c[])
{

    // reorder points to make a0 the leftmost
    if (a[0] <= b[0] && a[0] <= c[0])
    {
        triRenderALeft(sha, buf, unif, tex, a, b, c);
    }
    else if (b[0] <= c[0] && b[0] <= a[0])
    {
        triRenderALeft(sha, buf, unif, tex, b, c, a);
    }
    else
    {
        triRenderALeft(sha, buf, unif, tex, c, a, b);
    }
}