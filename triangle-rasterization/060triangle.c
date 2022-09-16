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

void triRender(
    double a0, double a1, double b0, double b1, double c0, double c1,
    double r, double g, double b)
{

    /*
    Whenever a is not left most, it messes with our algorithm a bit,
    so we'll detect it and make it left most

    We'll just keep swapping the points around until a0 is left most
    */

    //    while ((a0 > b0) || (a0 > c0))
    //    {
    //         double temp0 = c0;
    //         double temp1 = c1;

    //         c0 = temp0;
    //         c1 = temp1;

    //    }
    /*
        whenever a0 = b0, a0 = c0, or b0 = a0, we'll have a division by 0
        error, so we just have to handle these differently
    */

    /*
        For the first three cases, at least 2 of the points
        are on a vertical line, so we only need to draw
        1/2 of our triangle
    */

    if (a0 == b0)
    {
        // horizontal outer loop

        /*
            If B is below A, our upper and lower limits will be swapped
            from if B is above A
        */
        if (b1 < a1)
        {
            for (int x0 = ceil(a0); x0 < floor(c0); x0++)
            {
                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(b1 + ((c1 - b1) / (c0 - b0) * (x0 - b0)));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
        else
        {
            for (int x0 = ceil(a0); x0 < floor(c0); x0++)
            {
                int upper = floor(b1 + ((c1 - b1) / (c0 - b0) * (x0 - b0)));
                int lower = ceil(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
    }
    else if (a0 == c0)
    {
        /*
            This is the same as the a0 == b0 case
        */
        if (c1 < a1)
        {
            // horizontal outer loop
            for (int x0 = ceil(a0); x0 < floor(b0); x0++)
            {
                int upper = floor(c1 + ((b1 - c1) / (b0 - c0) * (x0 - c0)));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
            }
        }
        else
        {
            // horizontal outer loop
            for (int x0 = ceil(a0); x0 < floor(b0); x0++)
            {
                int upper = floor(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                int lower = ceil(c1 + ((b1 - c1) / (b0 - c0) * (x0 - c0)));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    pixSetRGB(x0, x1, r, g, b);
                }
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
        printf("this case is normal\n");
    }

    // double upper = a1 + ((b1-a1) / (b0 - a0))*(x0 - a0)
}