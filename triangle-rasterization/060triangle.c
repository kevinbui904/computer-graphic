/**
 * @file 060triangle.c
 * @author Thien K. M. Bui <buik@carleton.edu>
 * @brief triangle rasterizing algorithms
 * @version 0.1
 * @date 2022-09-15
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
    Helper functin for triRender
    This is where we actually handle the rendering,
    triRender only forces "A" to be the left most point
*/
void triRenderALeft(
    double a0, double a1, double b0, double b1, double c0, double c1,
    double r, double g, double b)
{

    /*
    There's two major cases we'll have to worry about.
    Namely, when B is to the left C and when B is to
    the right of C
    */

    if (b0 < c0)
    {

        /*
        We'll have to handle the division by 0 cases

        Note: we don't have to handle a0 = c0 and b0 = c0
        here because of counter-clockwise ness. In these cases,
        B will be to the right C
        */
        if (a0 == b0)
        {
            for (int x0 = ceil(b0); x0 <= floor(c0); x0++)
            {
                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(b1 + ((c1 - b1) / (c0 - b0)) * (x0 - b0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
        else
        {
            // first half the triangle, we run to b0
            for (int x0 = ceil(a0); x0 <= floor(b0); x0++)
            {

                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }

            // for the second half, we run from b0+1 to c0
            for (int x0 = floor(b0) + 1; x0 <= floor(c0); x0++)
            {
                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(b1 + ((c1 - b1) / (c0 - b0)) * (x0 - b0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
    }

    else
    {
        /*
        There are two division by 0 cases here
        */

        if (a0 == c0)
        {
            for (int x0 = ceil(a0); x0 <= floor(b0); x0++)
            {
                int upper = floor(c1 + ((b1 - c1) / (b0 - c0)) * (x0 - c0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }

        else if (c0 == b0)
        {
            for (int x0 = ceil(a0); x0 <= floor(b0); x0++)
            {
                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
        else
        {
            // first half of the triangle
            for (int x0 = ceil(a0); x0 <= floor(c0); x0++)
            {
                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }

            // second half of the triangle
            for (int x0 = floor(c0) + 1; x0 <= floor(b0); x0++)
            {
                int upper = floor(c1 + ((b1 - c1) / (b0 - c0)) * (x0 - c0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
    }
}

/*
Makes A the left most point,
according to Josh this reduces our code by
about 1/3
*/
void triRender(double a0, double a1, double b0, double b1, double c0, double c1,
               double r, double g, double b)
{
    printf("Triangle coord: A(%f,%f); B(%f,%f); C(%f,%f)\n", a0, a1, b0, b1, c0, c1);
    if (a0 <= b0 && a0 <= c0)
        triRenderALeft(a0, a1, b0, b1, c0, c1,
                       r, g, b);
    else if (b0 <= c0 && b0 <= a0)
        triRenderALeft(b0, b1, c0, c1, a0, a1,
                       r, g, b);
    else
        triRenderALeft(c0, c1, a0, a1, b0, b1,
                       r, g, b);
}