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

// globals

// used for colors
double red = 1.0, green = 1.0, blue = 1.0;

// detecting brush
int mouseIsDown = 0;

// brush size
double strokeSize = 0.0;

/*
This function just sets all of our key bindings
(e.g. pressing "r" sets the brush color to red, "c" sets to cyan,
"ESC" quits the program)
*/
void handleKeyUp(int key, int shiftIsDown, int controlIsDown,
                 int altOptionIsDown, int superCommandIsDown)
{
    // key behaviours, first 8 determines colors
    switch (key)
    {

    case GLFW_KEY_R:
        red = 1.0;
        green = 0.0;
        blue = 0.0;
        break;

    case GLFW_KEY_G:
        red = 0.0;
        green = 1.0;
        blue = 0.0;
        break;

    case GLFW_KEY_B:
        red = 0.0;
        green = 0.0;
        blue = 1.0;
        break;

    case GLFW_KEY_C:
        red = 0.0;
        green = 1.0;
        blue = 1.0;
        break;

    case GLFW_KEY_M:
        red = 1.0;
        green = 0.0;
        blue = 1.0;
        break;

    case GLFW_KEY_Y:
        red = 1.0;
        green = 1.0;
        blue = 0.0;
        break;

    case GLFW_KEY_K:
        red = 0.0;
        green = 0.0;
        blue = 0.0;
        break;

    case GLFW_KEY_W:
        red = 1.0;
        green = 1.0;
        blue = 1.0;
        break;

    // clear windows to black on DEL
    case GLFW_KEY_DELETE:
        pixClearRGB(0.0, 0.0, 0.0);
        break;

    // terminates program on ESCAPE
    case GLFW_KEY_ESCAPE:
        printf("Closing program...\n");
        exit(1);
    default:
        printf("Invalid key, we'll add it later ;D \n");
    }
}

/* Similarly, the following callbacks handle some mouse interactions. */
void handleMouseUp(double x, double y, int button, int shiftIsDown,
                   int controlIsDown, int altOptionIsDown, int superCommandIsDown)
{
    mouseIsDown = 0;
}

/*
This is the bulk of the drawing component. Whenever the mouse move, we'll
draw some pixels to the window.
*/
void handleMouseMove(double x, double y)
{
    if (mouseIsDown)
    {
        double curX = x;
        double curY = y;

        for (double i = x; i <= x + strokeSize; i++)
        {
            // this is actually an accident, I don't really know
            // why increasing x and y like this makes the stroke brush-like
            // BUT IT WORKS
            pixSetRGB(curX, curY, red, green, blue);
            curY++;
            curX++;
        }
    }
}

/*
We use the mouse to set the size of our brush,
scroll up to make brush bigger, down to minimize
*/
void handleMouseScroll(double xOffset, double yOffset)
{
    // make stroke smaller
    if (yOffset > 0)
    {
        // smallest strokeSize can be is 0
        if (strokeSize > 0)
        {
            strokeSize--;
        }
    }

    // make stroke bigger
    else if (yOffset < 0)
    {
        strokeSize++;
    }
    // printf("handleMouseScroll: xOffset %f, yOffset %f, strokeSize %f\n", xOffset, yOffset, strokeSize);
}

/*
Just a signal to stop brush from drawing
*/
void handleMouseDown(double x, double y, int button, int shiftIsDown,
                     int controlIsDown, int altOptionIsDown, int superCommandIsDown)
{
    mouseIsDown = 1;
}

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
    pixSetKeyUpHandler(handleKeyUp);
    pixSetMouseUpHandler(handleMouseUp);
    pixSetMouseDownHandler(handleMouseDown);
    pixSetMouseMoveHandler(handleMouseMove);
    pixSetMouseScrollHandler(handleMouseScroll);
    pixSetTimeStepHandler(handleTimeStep);

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
