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
#include "080triangle.c"

/*
    Global to cycle through the 12 triangles that need to be rendered
*/
int triNum = 1;

// global RGB
double rgb[3] = {1.0, -0.5, 0.0};

/*
Handle keyboard input for demo
*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
                 int altOptionIsDown, int superCommandIsDown)
{
    double a[2], b[2], c[2];

    if (key == GLFW_KEY_ENTER)
    {
        pixClearRGB(0.0, 0.0, 0.0);
        switch (triNum)
        {
        case 1:
            a[0] = 50;
            a[1] = 100;
            b[0] = 300;
            b[1] = 100;
            c[0] = 50;
            c[1] = 300;

            printf("case 1\n");
            triRender(a, b, c, rgb);
            break;
        case 2:
            a[0] = 50;
            a[1] = 100;
            b[0] = 300;
            b[1] = 100;
            c[0] = 150;
            c[1] = 300;

            printf("case 2\n");
            triRender(a, b, c, rgb);
            break;
        case 3:
            a[0] = 50;
            a[1] = 100;
            b[0] = 300;
            b[1] = 100;
            c[0] = 300;
            c[1] = 300;

            printf("case 3\n");
            triRender(a, b, c, rgb);
            break;
        case 4:
            a[0] = 50;
            a[1] = 100;
            b[0] = 300;
            b[1] = 100;
            c[0] = 300;
            c[1] = 300;

            printf("case 4\n");
            printf("\tshould be the same as case 3, switchin up points\n");
            triRender(b, c, a, rgb);
            break;
        case 5:
            a[0] = 50;
            a[1] = 100;
            b[0] = 300;
            b[1] = 100;
            c[0] = 300;
            c[1] = 300;

            printf("case 5\n");
            printf("\tshould be the same as case 3, switchin up points\n");
            triRender(c, a, b, rgb);
            break;
        case 6:
            a[0] = 50;
            a[1] = 300;
            b[0] = 300;
            b[1] = 150;
            c[0] = 300;
            c[1] = 300;

            printf("case 6\n");
            triRender(a, b, c, rgb);
            break;
        case 7:
            a[0] = 50;
            a[1] = 300;
            b[0] = 150;
            b[1] = 50;
            c[0] = 300;
            c[1] = 300;

            printf("case 7\n");
            triRender(a, b, c, rgb);
            break;
        case 8:
            a[0] = 50;
            a[1] = 200;
            b[0] = 50;
            b[1] = 50;
            c[0] = 300;
            c[1] = 200;

            printf("case 8\n");
            triRender(a, b, c, rgb);
            break;
        case 9:
            a[0] = 50;
            a[1] = 100;
            b[0] = 300;
            b[1] = 50;
            c[0] = 150;
            c[1] = 300;

            printf("case 9\n");
            triRender(a, b, c, rgb);
            break;
        case 10:
            a[0] = 50;
            a[1] = 100;
            b[0] = 150;
            b[1] = 50;
            c[0] = 300;
            c[1] = 300;

            printf("case 10\n");
            triRender(a, b, c, rgb);
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
