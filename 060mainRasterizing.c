/**
 * @file mainLinking.c
 * @author Thien K. M. Bui <buik@carleton.edu>
 * @brief adapted from Professor Josh Davis
 * @version 0.1
 * @date 2022-09-16
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include "040pixel.h"
#include "060triangle.c"

/*
    Global to cycle through the 12 triangles that need to be rendered
*/
int triNum = 1;

/*
Handle keyboard input for demo
*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
                 int altOptionIsDown, int superCommandIsDown)
{
    if (key == GLFW_KEY_ENTER)
    {
        pixClearRGB(0.0, 0.0, 0.0);
        switch (triNum)
        {
        case 1:

            printf("case 1\n");
            triRender(50, 100, 300, 100, 50, 300, 1.0, 1.0, 1.0);
            break;
        case 2:
            printf("case 2\n");
            triRender(50, 100, 300, 100, 150, 300, 1.0, 1.0, 1.0);
            break;
        case 3:
            printf("case 3\n");
            triRender(50, 100, 300, 100, 300, 300, 1.0, 1.0, 1.0);
            break;
        case 4:
            printf("case 4\n");
            printf("\tshould be the same as case 3, switchin up points\n");
            triRender(300, 300, 50, 100, 300, 100, 1.0, 1.0, 1.0);
            break;
        case 5:
            printf("case 5\n");
            printf("\tshould be the same as case 3, switchin up points\n");
            triRender(300, 100, 300, 300, 50, 100, 1.0, 1.0, 1.0);
            break;
        case 6:
            printf("case 6\n");
            triRender(50, 300, 300, 150, 300, 300, 1.0, 1.0, 1.0);
            break;
        case 7:
            printf("case 7\n");
            triRender(50, 300, 150, 50, 300, 300, 1.0, 1.0, 1.0);
            break;
        case 8:
            printf("case 8\n");
            triRender(50, 200, 50, 50, 300, 200, 1.0, 1.0, 1.0);
            break;
        case 9:
            printf("case 9\n");
            triRender(50, 100, 300, 50, 150, 300, 1.0, 1.0, 1.0);
            break;
        case 10:
            printf("case 10\n");
            triRender(50, 100, 150, 50, 300, 300, 1.0, 1.0, 1.0);
            break;
        default:
            printf("That's all for the demo folks, hope I didn't miss any cases\n");
            exit(1);
        }
        triNum++;
    }
}
int main(void)
{
    /* Make a 512 x 512 window with the title 'Pixel Graphics'. This function
    returns 0 if no error occurred. */
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    /* Register the callbacks (defined above) with the user interface, so that
    they are called as needed during pixRun (invoked below). */
    pixSetKeyUpHandler(handleKeyUp);

    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);
    /* Run the event loop. The callbacks that were registered above are
    invoked as needed. */
    pixRun();
    /* In this course, whenever you call an 'initialize' function to set up some
    resources, you must later call a matching 'finalize' function to clean up
    those resources. */
    pixFinalize();
    return 0;
}
