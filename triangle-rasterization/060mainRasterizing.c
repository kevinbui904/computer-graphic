/**
 * @file mainLinking.c
 * @author Thien K. M. Bui <buik@carleton.edu>
 * @brief adapted from Professor Josh Davis
 * @version 0.1
 * @date 2022-09-15
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

/* This callback is called once per animation frame. As parameters it receives
the time for the current frame and the time for the previous frame. Both times
are measured in seconds since some distant past time. */
void handleTimeStep(double oldTime, double newTime)
{
    if (floor(newTime) - floor(oldTime) >= 1.0)
        printf("handleTimeStep: %f frames/s\n", 1.0 / (newTime - oldTime));
}

int main(void)
{
    /* Make a 512 x 512 window with the title 'Pixel Graphics'. This function
    returns 0 if no error occurred. */
    if (pixInitialize(512, 512, "Pixel Graphics") != 0)
        return 1;
    /* Register the callbacks (defined above) with the user interface, so that
    they are called as needed during pixRun (invoked below). */
    pixSetTimeStepHandler(handleTimeStep);
    /* Clear the window to black. */
    pixClearRGB(0.0, 0.0, 0.0);

    triRender(25, 100, 200, 50, 300, 200, 1.0, 1.0, 1.0);
    /* Run the event loop. The callbacks that were registered above are
    invoked as needed. */
    pixRun();
    /* In this course, whenever you call an 'initialize' function to set up some
    resources, you must later call a matching 'finalize' function to clean up
    those resources. */
    pixFinalize();
    return 0;
}
