/**
 * @file 170shading.c
 * @author Thien K. M. Bui <buik@carleton.edu>, Shannon Liu <lius3@carleton.edu>
 * @brief define struc definition for fragment shader
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
    int texNum; // no clue what this is for ********
};

