/* Contributors: Shannon Liu & Kevin Bui*/

#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>

void linearInterpolation(const int varyDim, const double a[], const double b[], const double c[], const double x[2], 
    const double AInv[2][2],const double betaMinAlpha[], const double gamMinAlpha[], double chi[]){
    
    //create empty vectors and matrix to input
    double xMinusA[2], pq[2], pMult[varyDim], qMult[varyDim], pqMultAdd[varyDim];
    
    // subtract vector x from vector a
    vecSubtract(2, x, a, xMinusA);

    //multiply inverse of matrix A by x-a
    mat221Multiply(AInv, xMinusA, pq);

    //p*(beta - alpha)
    vecScale(varyDim, pq[0], betaMinAlpha, pMult);

    //q*(gamma - alpha)
    vecScale(varyDim, pq[1], gamMinAlpha, qMult);

    //p*(beta - alpha) + q*(gamma - alpha)
    vecAdd(varyDim, pMult, qMult, pqMultAdd);

    //alpha + (p*(beta - alpha) + q*(gamma - alpha))
    vecAdd(varyDim, a, pqMultAdd, chi);
}

/*
helper function for triRender
*/
void triRenderALeft(
        const shaShading *sha, const double unif[], const texTexture *tex[], 
        const double a[], const double b[], const double c[]){
    //printf("entered left triangle render\n");

    // Calculate matrix A based on vectors a, b, c
    double A[2][2] = {{b[0]-a[0], c[0]-a[0]}, {b[1]-a[1], c[1]-a[1]}};
    //invert matrix A
    double AInv[2][2];

    //det for backface culling
    double det = mat22Invert(A, AInv);

    //backface culling: do not print triangles with a negative determinant
    if (det < 0){
        return;
    }

    double betaMinAlpha[sha->varyDim], gamMinAlpha[sha->varyDim];

    //beta - alpha
    vecSubtract(sha->varyDim, b, a, betaMinAlpha);

    //gamma - alpha
    vecSubtract(sha->varyDim, c, a, gamMinAlpha);

    double x[2], chi[sha->varyDim], final[3];

    //Case if a0 = c0, prevent division by 0
    if (a[0] == c[0]){
        //printf("entered a0=c0\n");

        for(int x0 = ceil(a[0]); x0<=floor(b[0]); x0++){

        double upper = floor(c[1] + (b[1]-c[1])/(b[0]-c[0])*(x0-c[0]));
        double lower = ceil(a[1]+(b[1]-a[1])/(b[0]-a[0])*(x0-a[0]));

        for(int x1 = lower; x1 <= upper; x1++){
            // printf("entered left print loops\n");
            x[0]=x0;
            x[1]=x1;

            linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
            sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
            pixSetRGB(x0, x1, final[0], final[1], final[2]);
        }
        }
    }
    //Case if a0 = b0, prevent division by 0
    else if(a[0]==b[0]){
        //printf("entered a0=b0\n");
        for(int x0 = ceil(a[0]); x0<=floor(c[0]); x0++){

            double upper = floor(a[1]+(c[1]-a[1])/(c[0]-a[0])*(x0-a[0]));
            double lower = ceil(b[1] + (c[1]-b[1])/(c[0]-b[0])*(x0-b[0]));

            for(int x1 = lower; x1 <= upper; x1++){
                // printf("entered left print loops\n");
                x[0]=x0;
                x[1]=x1;
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                pixSetRGB(x0, x1, final[0], final[1], final[2]);
            }
        }
    }
    //Case if c0 <= b0
    else if(c[0] <= b[0]){
        //printf("points were not equal: c0 < b0\n");
        for(int x0 = ceil(a[0]); x0<=floor(c[0]); x0++){

            double upper = floor(a[1]+((c[1]-a[1])/(c[0]-a[0])*(x0-a[0])));
            double lower = ceil(a[1] + ((b[1]-a[1])/(b[0]-a[0])*(x0-a[0])));

            for(int x1 = lower; x1 <= upper; x1++){
                // printf("entered left print loops\n");
                x[0]=x0;
                x[1]=x1;

                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);

                pixSetRGB(x0, x1, final[0], final[1], final[2]);
            }
        }

        for(int x0 = floor(c[0])+1; x0<=floor(b[0]); x0++){

            double upper = floor(c[1] + (b[1]-c[1])/(b[0]-c[0])*(x0-c[0]));
            double lower = ceil(a[1]+(b[1]-a[1])/(b[0]-a[0])*(x0-a[0]));

            for(int x1 = lower; x1 <= upper; x1++){
                // printf("entered left print loops\n");
                x[0]=x0;
                x[1]=x1;
                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                pixSetRGB(x0, x1, final[0], final[1], final[2]);
            }
        }
    }
    //case if b0 < c0
    else{
        //printf("points were not equal: b0 < c0\n");
        for(int x0 = ceil(a[0]); x0<=floor(b[0]); x0++){

            double upper = floor(a[1]+((c[1]-a[1])/(c[0]-a[0])*(x0-a[0])));
            double lower = ceil(a[1] + ((b[1]-a[1])/(b[0]-a[0])*(x0-a[0])));

            for(int x1 = lower; x1 <= upper; x1++){
                // printf("entered left print loops\n");
                x[0]=x0;
                x[1]=x1;

                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                pixSetRGB(x0, x1, final[0], final[1], final[2]);
            }
        }

        for(int x0 = floor(b[0])+1; x0<=floor(c[0]); x0++){

            double upper = floor(a[1]+(c[1]-a[1])/(c[0]-a[0])*(x0-a[0]));
            double lower = ceil(b[1] + (c[1]-b[1])/(c[0]-b[0])*(x0-b[0]));

            for(int x1 = lower; x1 <= upper; x1++){
                // printf("entered left print loops\n");
                x[0]=x0;
                x[1]=x1;

                linearInterpolation(sha->varyDim, a, b, c, x, AInv, betaMinAlpha, gamMinAlpha, chi);
                sha->shadeFragment(sha->unifDim, unif, sha->texNum, tex, sha->varyDim, chi, final);
                pixSetRGB(x0, x1, final[0], final[1], final[2]);
            }
        }
    }

}
/* Assumes that the 0th and 1th elements of a, b, c are the 'x' and 'y' 
coordinates of the vertices, respectively (used in rasterization, and to 
interpolate the other elements of a, b, c). */
void triRender(
        const shaShading *sha, const double unif[], const texTexture *tex[], 
        const double a[], const double b[], const double c[]){

    //reorder points to make a0 the leftmost
    if(a[0] <= b[0] && a[0] <= c[0]){
        triRenderALeft(sha, unif, tex, a, b ,c);
    }
    else if(b[0] <= c[0] && b[0] <= a[0]){
        triRenderALeft(sha, unif, tex, b, c ,a);

    }
    else{
        triRenderALeft(sha, unif, tex, c, a ,b);
    }
	
}