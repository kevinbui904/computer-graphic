
/*
    Author: Thien K. M. Bui and Shannon Liu, extended from 
        Professor Josh Davis' starter code. Computer Graphics Fall '22
        Carleton College
*/


/* On macOS, compile with...
    clang 650mainTexturing.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
On Ubuntu, compile with...
    cc 650mainTexturing.c 040pixel.o -lglfw -lGL -lm -ldl
*/
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"

#include "650vector.c"
#include "280matrix.c"
#include "300isometry.c"
#include "300camera.c"
#include "640ray.c"

/*NEW (KB+SL): used to intialize textures*/
#include "150texture.c"

#define SCREENWIDTH 512
#define SCREENHEIGHT 512

#include <stdlib.h>

/*** SPHERES ******************************************************************/

/* Given a sphere of radius r, centered at the origin in its local coordinates. 
Given a modeling isometry that places that sphere in the scene. Given a ray 
x(t) = p + t d in world coordinates. Outputs a rayIntersection, whose t member 
is the least t, in the interval [rayEPSILON, bound], when the ray intersects the 
sphere. If there is no such t, then the t member is instead rayNONE. */
void getIntersection(
        double r, const isoIsometry *isom, const double p[3], const double d[3], 
        double bound, rayIntersection* inter) {

    double c[3];
    vecCopy(3, isom->translation, c);
    double pMinusC[3], dPMinusC, dD, rSq, disc, t;
    vecSubtract(3, p, c, pMinusC);
    dPMinusC = vecDot(3, d, pMinusC);
    dD = vecDot(3, d, d);
    rSq = r * r;
    disc = dPMinusC * dPMinusC - dD * (vecDot(3, pMinusC, pMinusC) - rSq);

    if (disc <= 0) {
        inter->t = rayNONE;
        return;
    }
    disc = sqrt(disc);
    t = (-dPMinusC - disc) / dD;
    if (rayEPSILON <= t && t <= bound) {
        inter->t = t;
        return;
    }
    t = (-dPMinusC + disc) / dD;
    if (rayEPSILON <= t && t <= bound) {
        inter->t = t;
        return;
    }
    inter->t = rayNONE;

}


/* Given the sphere that just produced the given rayIntersection. Outputs the 
sphere's texture coordinates at the intersection point. Also outputs the 
sphere's unit outward-pointing normal vector there, in world coordinates. */
void getTexCoordsAndNormal(
        double r, const isoIsometry *isom, const double p[3], const double d[3], 
        const rayIntersection* inter, double texCoords[2], double normal[3]){
        
        /* NEW (KB+SL): calculate the unit normal vector at this xyzWorld point */
        double xyzWorld[3], tD[3], unNormalizedNormal[3];
        vecScale(3, inter->t, d, tD);
        vecAdd(3, p, tD, xyzWorld);
        vecSubtract(3, xyzWorld, isom->translation, unNormalizedNormal);
        vecUnit(3, unNormalizedNormal, normal);
        
        double xyzLocal[3], rho, phi, theta;
        /* NEW (KB+SL): calculate the texture coordinate*/
        isoUntransformPoint(isom, xyzWorld, xyzLocal);
        vec3Rectangular(xyzLocal, &rho, &phi, &theta);
        texCoords[0] = theta / (2*M_PI);
        texCoords[1] = 1 - (phi/M_PI);
}


/*** ARTWORK ******************************************************************/

camCamera camera;
double cameraTarget[3] = {0.0, 0.0, 0.0};
double cameraRho = 10.0, cameraPhi = M_PI / 3.0, cameraTheta = M_PI / 3.0;

/* Four spheres. */
#define BODYNUM 4
isoIsometry isoms[BODYNUM];
double radii[BODYNUM] = {1.0, 0.5, 0.5, 0.5};
double colors[BODYNUM][3] = {
    {1.0, 1.0, 1.0}, 
    {1.0, 0.0, 0.0}, 
    {0.0, 1.0, 0.0}, 
    {0.0, 0.0, 1.0}};

/*NEW (KB+SL): textures*/
texTexture texture;
int initializeArtwork(void) {

    /* NEW (KB+SL): initializing a texture*/
    if (texInitializeFile(&texture, "./nyan.jpg") != 0)
    {
        printf("unable to initialize texture\n");
        return 2;
    }
    camSetProjectionType(&camera, camPERSPECTIVE);
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);
    double rot[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};
    for (int k = 0; k < BODYNUM; k += 1)
        isoSetRotation(&(isoms[k]), rot);
    double transl[3] = {0.0, 0.0, 0.0};
    isoSetTranslation(&(isoms[0]), transl);
    vec3Set(1.0, 0.0, 0.0, transl);
    isoSetTranslation(&(isoms[1]), transl);
    vec3Set(0.0, 1.0, 0.0, transl);
    isoSetTranslation(&(isoms[2]), transl);
    vec3Set(0.0, 0.0, 1.0, transl);
    isoSetTranslation(&(isoms[3]), transl);
    return 0;
}

void finalizeArtwork(void) {
    texFinalize(&texture);
    return;
}



/*** RENDERING ****************************************************************/

/* Given a ray x(t) = p + t d. Finds the color where that ray hits the scene (or 
the background) and loads the color into the rgb parameter. */
void getSceneColor(const double p[3], const double d[3], double rgb[3]) {
    rayIntersection ray;
    int intersectedBody;
    int count = 0;
    double bound = rayINFINITY;

    /*NEW (KB+SL): this is to keep track of the final ray intersection, to be used for texturing*/
    rayIntersection rayFinal;
    for(int i = 0; i < BODYNUM; i++){
        getIntersection(radii[i], &isoms[i], p, d, bound, &ray);    
        if(ray.t < bound && ray.t != rayNONE){
            intersectedBody = i;
            bound = ray.t;
            rayFinal.t = ray.t;
        }
    }
    if(bound == rayINFINITY){
        vec3Set(0.0, 0.0, 0.0, rgb);
        return;
    }
    /* NEW (KB+SL): calculate texture coordinates and normal of a point */
    double texCoords[2], unitNormal[3];
    getTexCoordsAndNormal(radii[intersectedBody], &isoms[intersectedBody], p, d, &rayFinal, texCoords, unitNormal);
    vec3Set(colors[intersectedBody][0], colors[intersectedBody][1], colors[intersectedBody][2], rgb);

    /*NEW (KB+SL): modulating in textures*/
    double sample[3];
    texSample(&texture, texCoords[0], texCoords[1], sample);
    vecModulate(3, rgb, sample, rgb);
    
}

void render(void) {
    /* Build a 4x4 matrix that (along with homogeneous division) takes screen 
    coordinates (x0, x1, 0, 1) to the corresponding world coordinates. */
    
    /* YOUR CODE GOES HERE. (MINE IS 10 LINES.) */
    double camIsom[4][4], projectionInverseIsometry[4][4], cPInv[4][4], invViewport[4][4], cPInvVInv[4][4];

    isoGetHomogeneous(&(camera.isometry), camIsom);

    if(camera.projectionType == camPERSPECTIVE){
        camGetInversePerspective(&camera, projectionInverseIsometry);
    }
    else{
        camGetInverseOrthographic(&camera, projectionInverseIsometry);
    }

    mat444Multiply(camIsom, projectionInverseIsometry, cPInv);

    mat44InverseViewport(SCREENWIDTH, SCREENHEIGHT, invViewport);

    mat444Multiply(cPInv, invViewport, cPInvVInv);

    /* Declare p and maybe compute d. */
    double p[4], d[3];
    if (camera.projectionType == camORTHOGRAPHIC){
        double v[3] = {0, 0, -1};
        isoRotateDirection(&camera.isometry, v, d);
    }

    /* Each screen point is chosen to be on the near plane. */
    double screen[4] = {0.0, 0.0, 0.0, 1.0};
    for (int i = 0; i < SCREENWIDTH; i += 1) {
        screen[0] = i;
        for (int j = 0; j < SCREENHEIGHT; j += 1) {
            screen[1] = j;
            /* Compute p and maybe also d. */
            mat441Multiply(cPInvVInv, screen, p);
            //apply homogeneous division
            vecScale(4, 1/p[3], p, p);
            if (camera.projectionType == camPERSPECTIVE)
                vecSubtract(3, p, camera.isometry.translation, d);

            /* Set the pixel to the color of that ray. */
            double rgb[3];
            getSceneColor(p, d, rgb);
            pixSetRGB(i, j, rgb[0], rgb[1], rgb[2]);
        }
    }
}



/*** USER INTERFACE ***********************************************************/

void handleKey(
        int key, int shiftIsDown, int controlIsDown, int altOptionIsDown, 
        int superCommandIsDown) {
    if (key == GLFW_KEY_I)
        cameraPhi -= 0.1;
    else if (key == GLFW_KEY_K)
        cameraPhi += 0.1;
    else if (key == GLFW_KEY_J)
        cameraTheta -= 0.1;
    else if (key == GLFW_KEY_L)
        cameraTheta += 0.1;
    else if (key == GLFW_KEY_U)
        cameraRho *= 1.1;
    else if (key == GLFW_KEY_O)
        cameraRho *= 0.9;
    else if (key == GLFW_KEY_P) {
        if (camera.projectionType == camORTHOGRAPHIC)
            camSetProjectionType(&camera, camPERSPECTIVE);
        else
            camSetProjectionType(&camera, camORTHOGRAPHIC);
    }
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);
}

void handleTimeStep(double oldTime, double newTime) {
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf(
            "info: handleTimeStep: %f frames/s\n", 1.0 / (newTime - oldTime));
    double rotAxis[3] = {1.0 / sqrt(3.0), 1.0 / sqrt(3.0), 1.0 / sqrt(3.0)};
    double rotMatrix[3][3];
    mat33AngleAxisRotation(newTime, rotAxis, rotMatrix);
    for (int k = 0; k < BODYNUM; k += 1)
        isoSetRotation(&(isoms[k]), rotMatrix);
    render();
}

int main(void) {
    if (pixInitialize(SCREENWIDTH, SCREENHEIGHT, "650mainTexturing") != 0)
        return 1;
    if (initializeArtwork() != 0) {
        pixFinalize();
        finalizeArtwork();
        return 2;
    }
    pixSetKeyDownHandler(handleKey);
    pixSetKeyRepeatHandler(handleKey);
    pixSetTimeStepHandler(handleTimeStep);
    pixRun();
    finalizeArtwork();
    pixFinalize();
    return 0;
}

