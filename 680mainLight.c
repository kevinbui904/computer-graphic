
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
    material->hasAmbient = 0;
    // NEW (KB + SL): activate diffuse & specular light
    material->hasDiffuse = 0;
    material->hasSpecular = 1;
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
#define MATERUNIFDIM 4
#define GEOMUNIFDIM 1
#define TEXNUM 1

/*NEW (KB+SL): Lighting configurations*/
double cAmbientLight[3] = {1.0, 1.0, 1.0};
#define LIGHTNUM 2
lightLight lights[LIGHTNUM];
double lightTheta = 0.0;

/*NEW (KB+SL): unif, textures configurations for materials*/
#define UNIFDIM 69
double unif[UNIFDIM];

/*NEW (KB+SL): textures*/
#define TEXNUM 1
texTexture texture;
texTexture *textures[1] = {&texture};

/* NEW (KB+SL): directive for directional lighting */
void getDirectionalLighting(int unifDim, const double unif[], const isoIsometry *isometry, const double x[3], lightLighting *lighting){
    vecCopy(3, unif, lighting->cLight);
    lighting->distance = rayINFINITY;
    double directionalVec[3] = {0.0, 0.0, 1.0};
    isoRotateDirection(isometry, directionalVec, lighting->uLight);
}

/* NEW (KB+SL): directive for positional lighting */
void getPositionalLighting(int unifDim, const double unif[], const isoIsometry *isometry, const double x[3], lightLighting *lighting){
    vecCopy(3, unif, lighting->cLight);
    double pLightMinusX[3];
    vecSubtract(3, isometry->translation, x, pLightMinusX);
    lighting->distance = vecLength(3, pLightMinusX);
    vecUnit(3, pLightMinusX, lighting->uLight);
}

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
    
    
    /* NEW (KB+SL): configuring directional light and putting it into lights array*/
    double cLightDirectional[3] = {1.0, 1.0, 1.0};
    lightInitialize(&(lights[0]), 3, getDirectionalLighting);
    isoSetRotation(&(lights[0].isometry), rot);
    lightSetUniforms(&(lights[0]), 0, cLightDirectional, 3);

    /* NEW (KB+SL): configuring positional light and putting it into lights array*/
    double cLightPositional[3] = {1.0, 0.0, 0.0};
    double pLightPositional[3] = {0.57735, 0.57735, 0.57735};
    lightInitialize(&(lights[1]), 3, getPositionalLighting);
    isoSetRotation(&(lights[1].isometry), rot);
    isoSetTranslation(&(lights[1].isometry), pLightPositional);
    lightSetUniforms(&(lights[1]), 0, cLightPositional, 3);


    // NEW (KB+SL): create array for specular lighting
    double specularArray[MATERUNIFDIM] = {1.0, 1.0, 1.0, 64.0};

    for(int i = 0; i < BODYNUM; i++){
        bodyInitialize(&(bodies[i]), GEOMUNIFDIM, MATERUNIFDIM, TEXNUM, sphGetIntersection, sphGetTexCoordsAndNormal, getMaterial);
        bodySetGeometryUniforms(&(bodies[i]), 0, radii[i], 1);
        bodySetTexture(&(bodies[i]), 0, textures[0]);

        //set isometries
        isoSetTranslation(&(bodies[i].isometry), translations[i]);
        isoSetRotation(&(bodies[i].isometry), rot);

        // NEW (KB+SL): body set material for specular light
        bodySetMaterialUniforms(&(bodies[i]), 0, specularArray, MATERUNIFDIM);
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
    for(int k = 0; k < LIGHTNUM; k++){
        lightFinalize(&(lights[k]));
    }
    return;
}


/*** RENDERING ****************************************************************/

/* Given a ray x(t) = p + t d. Finds the color where that ray hits the scene (or 
the background) and loads the color into the rgb parameter. */
void getSceneColor(
        int bodyNum, const bodyBody bodies[], const double cAmbient[3], 
        int lightNum, const lightLight lights[], const double p[3], 
        const double d[3], double rgb[3]){
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

    if(material.hasAmbient || material.hasDiffuse)
        vecCopy(3, material.cDiffuse, rgb);
    else
        vec3Set(0.0, 0.0, 0.0, rgb);

    /*NEW (KB+SL): rendering based on material received*/
    if(material.hasAmbient)
        vecModulate(3, cAmbient, rgb, rgb);
    
    if(material.hasDiffuse){
        double tD[3], xyzWorld[3];
        vecScale(3, rayFinal.t, d, tD);
        vecAdd(3, p, tD, xyzWorld);
        //loop over every light to get the lighting effects
        double iDiff, modCLightCDiff[3], diffusedLight[3], totalDiffusedLight[3] = {0.0, 0.0, 0.0};
        lightLighting lighting; 
        for(int i = 0; i < lightNum; i++){
            lightGetLighting(&(lights[i]), xyzWorld, &lighting);
            iDiff = fmax(0.0, vecDot(3, unitNormal, lighting.uLight));
            vecModulate(3, lighting.cLight, material.cDiffuse, modCLightCDiff);
            vecScale(3, iDiff, modCLightCDiff, diffusedLight);
            vecAdd(3, diffusedLight, totalDiffusedLight, totalDiffusedLight);
        }
        vecAdd(3, rgb, totalDiffusedLight, rgb);


    }

    if(material.hasSpecular){
        double tD[3], xyzWorld[3];
        vecScale(3, rayFinal.t, d, tD);
        vecAdd(3, p, tD, xyzWorld);
        //loop over every light to get the lighting effects
        double  specularLight[3], totalSpecularLight[3] = {0.0, 0.0, 0.0}, 
        dCamera[3], uCamera[3], uReflected[3], uNormDotULight, specularCoefficient, iDiff;

        //calculating uCamera
        vecScale(3, -1.0, d, dCamera);
        vecUnit(3, dCamera, uCamera);
        
        lightLighting lighting; 
        for(int i = 0; i < lightNum; i++){
            lightGetLighting(&(lights[i]), xyzWorld, &lighting);
            
            
            iDiff = vecDot(3, unitNormal, lighting.uLight);

            //calculate uReflected 
            uNormDotULight = vecDot(3, unitNormal, lighting.uLight);
            vecScale(3, uNormDotULight, unitNormal, uReflected);
            vecScale(3, 2, uReflected, uReflected);
            vecSubtract(3, uReflected, lighting.uLight, uReflected);
        
            specularCoefficient = vecDot(3, uReflected, uCamera);    

            //special case if iDiff or specularCoefficient <= 0
            if(iDiff <= 0 || specularCoefficient <= 0){
                specularCoefficient = 0.0;
            }
            else{
                specularCoefficient = pow(specularCoefficient, bodies[intersectedBody].materUnif[3]);
            }
            
      
            //NOTE: the second parameter is specularColor, it's in the first 3 uniforms of materUNIF
            vecModulate(3, lighting.cLight, &(bodies[intersectedBody].materUnif[0]), specularLight);
            vecScale(3, specularCoefficient, specularLight, specularLight);

            vecAdd(3, specularLight, totalSpecularLight, totalSpecularLight);
        }
        vecAdd(3, rgb, totalSpecularLight, rgb);
    }
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
            getSceneColor(BODYNUM, bodies, cAmbientLight, LIGHTNUM, lights, p, d, rgb);
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
    else if (key == GLFW_KEY_LEFT)
        lightTheta += 0.1;
    else if (key == GLFW_KEY_RIGHT)
    {
        lightTheta -= 0.1;
        printf("lightTheta: %f\n", lightTheta);
    }
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
    
    /*
    NEW (KB+SL): to rotate the light (only along the x axis though)
    */
    double axisRotation[3] = {0.57735, 0.57735, 0.57735}, lightRotation[3][3];
    mat33AngleAxisRotation(lightTheta, axisRotation, lightRotation);
    isoSetRotation(&(lights[0].isometry), lightRotation);

    render();
}

int main(void) {
    if (pixInitialize(SCREENWIDTH, SCREENHEIGHT, "680mainLight") != 0)
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

