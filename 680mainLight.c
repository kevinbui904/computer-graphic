
/*
    Author: Thien K. M. Bui and Shannon Liu, extended from 
        Professor Josh Davis' starter code. Computer Graphics Fall '22
        Carleton College
*/


/* On macOS, compile with...
    clang 680mainLight.c 040pixel.o -lglfw -framework OpenGL -framework Cocoa -framework IOKit
On Ubuntu, compile with...
    cc 680mainLight.c 040pixel.o -lglfw -lGL -lm -ldl
*/
#include <stdio.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"

#include "650vector.c"
#include "280matrix.c"
#include "300isometry.c"
#include "300camera.c"
#include "660ray.c"

/*NEW (KB+SL): used to intialize textures*/
#include "150texture.c"

/*NEW (KB+SL): to use bodyBody abstraction*/
#include "670body.c"
#include "670sphere.c"

/* NEW (KB+SL): for light effects*/
#include "680light.c"

#define SCREENWIDTH 512
#define SCREENHEIGHT 512

#include <stdlib.h>


/* Based on the uniforms, textures, rayIntersection, and texture coordinates, 
outputs a material. */
void getMaterial(
        int unifDim, const double unif[], int texNum, const texTexture *tex[], 
        const rayIntersection *inter, const double texCoords[2], 
        rayMaterial *material){
    /*This is very specific to Day 24 work*/
    material->hasAmbient = 1;
    material->hasDiffuse = 0;
    material->hasSpecular = 0;
    material->hasMirror = 0;
    material->hasTransmission = 0;
    texSample(tex[0], texCoords[0], texCoords[1], material->cDiffuse);
}


/*** ARTWORK ******************************************************************/

camCamera camera;
double cameraTarget[3] = {0.0, 0.0, 0.0};
double cameraRho = 10.0, cameraPhi = M_PI / 3.0, cameraTheta = M_PI / 3.0;

/* Four spheres. */
#define BODYNUM 4
bodyBody bodies[BODYNUM];

/*bodies configurations*/
#define MATERUNIFDIM 0
#define GEOMUNIFDIM 1
#define TEXNUM 1

/*NEW (KB+SL): Lighting configurations*/
double cAmbientLight[3] = {1.0, 0.0, 0.0};

/*NEW (KB+SL): unif, textures configurations for materials*/
#define UNIFDIM 69
double unif[UNIFDIM];

/*NEW (KB+SL): textures*/
#define TEXNUM 1
texTexture texture;
texTexture *textures[1] = {&texture};

int initializeArtwork(void) {
    /* NEW (KB+SL): initializing a texture*/
    if (texInitializeFile(&texture, "./140imageCat.jpg") != 0)
    {
        printf("unable to initialize texture\n");
        return 2;
    }

    /* NEW (KB+SL): initializing bodies to be spheres*/
    //NOTE: the radii is made like this because of how bodySetGeometryUniform is configured
    double radii[BODYNUM][1] = {{1.0}, {0.5}, {0.5}, {0.5}};
    double geomUnif[GEOMUNIFDIM];
    double translations[BODYNUM][3] = {
        {0.0, 0.0, 0.0},
        {1.0, 0.0, 0.0},
        {0.0, 1.0, 0.0},
        {0.0, 0.0, 1.0}
    };
    double rot[3][3] = {{1.0, 0.0, 0.0}, {0.0, 1.0, 0.0}, {0.0, 0.0, 1.0}};

    for(int i = 0; i < BODYNUM; i++){
        bodyInitialize(&(bodies[i]), GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, sphGetIntersection, sphGetTexCoordsAndNormal, getMaterial);
        bodySetGeometryUniforms(&(bodies[i]), 0, radii[i], 1);
        bodySetTexture(&(bodies[i]), 0, textures[0]);

        //set isometries
        isoSetTranslation(&(bodies[i].isometry), translations[i]);
        isoSetRotation(&(bodies[i].isometry), rot);

    }
    camSetProjectionType(&camera, camPERSPECTIVE);
    camSetFrustum(
        &camera, M_PI / 6.0, cameraRho, 10.0, SCREENWIDTH, SCREENHEIGHT);
    camLookAt(&camera, cameraTarget, cameraRho, cameraPhi, cameraTheta);
    return 0;
}

void finalizeArtwork(void) {
    texFinalize(&texture);
    for(int i = 0; i < BODYNUM; i++){
        bodyFinalize(&(bodies[i]));
    }
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
        bodyGetIntersection(&(bodies[i]), p, d, bound, &ray); 
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
    bodyGetTexCoordsAndNormal(&(bodies[intersectedBody]), p, d, &rayFinal, texCoords, unitNormal);

    /*NEW (KB+SL): get the material specification at this point*/
    rayMaterial material;
    bodyGetMaterial(&(bodies[intersectedBody]), &rayFinal, texCoords, &material);

    /*NEW (KB+SL): rendering based on material received*/
    if(material.hasAmbient){
        vecModulate(3, cAmbientLight, material.cDiffuse, rgb);
    }

    // vecModulate(3, rgb, sample, rgb);
    
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
        isoSetRotation(&(bodies[k].isometry), rotMatrix);
    render();
}

int main(void) {
    if (pixInitialize(SCREENWIDTH, SCREENHEIGHT, "670mainBody") != 0)
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

