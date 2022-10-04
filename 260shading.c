/**
 * @file 260shading.c
 * @author Thien K. M. Bui <buik@carleton.edu>, Shannon Liu <lius3@carleton.edu>
 * @brief define struc definition for fragment shader with varyDim
 * @version 0.1
 * @date 2022-09-19
 *
 * @copyright Copyright (c) 2022
 *
 */


#include <stdio.h>

/**
 *type def of shaShading, used in fragment shader
*/
typedef struct shaShading shaShading;

struct shaShading{
    int unifDim; //universal variable dimension
    int attrDim; // attribute dimension, used in interpolation
    int texNum; // Number of texture we're dealing with
    int varyDim; // number of varying
    void (*shadeVertex)(int, const double*, int, const double*, int, double*); //function pointer to shadeVertex
    void (*shadeFragment)(int, const double*, int, const texTexture**, int, const double*, double*); //function pointer to shadeFragment
};


