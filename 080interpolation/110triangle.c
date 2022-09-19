/**
 * @file 110triangle.c
 * @author Thien K. M. Bui <buik@carleton.edu>
 * @brief triangle interpolation + rasterizing algorithm
 * @version 0.1
 * @date 2022-09-18
 *
 * @copyright Copyright (c) 2022
 *
 */

/*
to make sure that RBG value is never negative
*/

void calculateRBG(const double d, const double chi[3], const double rgb[3], double modulatedRGB[3])
{

    vecModulate(d, chi, rgb, modulatedRGB);
    if (modulatedRGB[0] < 0)
    {
        printf("chir: %f,%f,%f\n", chi[0], chi[1], chi[2]);
        printf("rgb:%f,%f,%f\n", rgb[0], rgb[1], rgb[2]);
        printf("modulated: %f,%f,%f\n", modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
        exit(1);
    }

    if (modulatedRGB[1] < 0)
    {
        printf("chir: %f,%f,%f\n", chi[0], chi[1], chi[2]);
        printf("rgb:%f,%f,%f\n", rgb[0], rgb[1], rgb[2]);
        printf("modulated: %f,%f,%f\n", modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
        exit(1);
    }
    if (modulatedRGB[2] < 0)
    {
        printf("chir: %f,%f,%f\n", chi[0], chi[1], chi[2]);
        printf("rgb:%f,%f,%f\n", rgb[0], rgb[1], rgb[2]);
        printf("modulated: %f,%f,%f\n", modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
        exit(1);
    }
}
/*
findVectPQ:
    find p, q that will be used to calculate chi in interpolation
    check interpolation note for formula of matrix A
*/
void findVectPQ(const double a[2], const double x[2], const double invMatrixA[2][2], double vectPQ[2])
{
    double xMinusA[2];
    vecSubtract(2, x, a, xMinusA);
    mat221Multiply(invMatrixA, xMinusA, vectPQ);
    mat22Print(invMatrixA);
    printf("A: %f, %f\n", a[0], a[1]);

    printf("x: %f, %f\n", x[0], x[1]);

    printf("xMinusA: %f, %f\n", xMinusA[0], xMinusA[1]);
}

/*
Calculate chi
*/
void interpolate(const double alpha[3], const double betMinAlp[3], const double gamMinAlp[3], const double vectPQ[2], double chi[3])
{
    // set chi to be alpha
    vec3Set(alpha[0], alpha[1], alpha[2], chi);
    printf("alpha: %f,%f,%f\n", alpha[0], alpha[1], alpha[2]);
    double qGamMinAlp[3], pBetMinAlp[3];

    // printf("vecSubtract: %f,%f,%f\n", betMinAlp[0], betMinAlp[1], betMinAlp[2]);
    // printf("vecSubtract 2: %f,%f,%f\n", gamMinAlp[0], gamMinAlp[1], gamMinAlp[2]);

    vecScale(3, vectPQ[0], betMinAlp, pBetMinAlp);
    vecScale(3, vectPQ[1], gamMinAlp, qGamMinAlp);

    printf("pBetMinAlp: %f,%f,%f, P: %f\n", pBetMinAlp[0], pBetMinAlp[1], pBetMinAlp[2], vectPQ[0]);
    printf("qGamMinAlp: %f,%f,%f, Q: %f\n", qGamMinAlp[0], qGamMinAlp[1], qGamMinAlp[2], vectPQ[1]);

    // not adding alpha again because chi starts off with alpha's values
    vecAdd(3, chi, pBetMinAlp, chi);
    vecAdd(3, chi, qGamMinAlp, chi);
}
/*
    Helper functin for triRender
    This is where we actually handle the rendering,
    triRender only forces "A" to be the left most point
*/
void triRenderALeft(
    const double A[2], const double B[2], const double C[2],
    const double rgb[3], const double alpha[3], const double beta[3],
    const double gamma[3])
{

    // rebinding so formulas are a bit easier to see
    double a0 = A[0];
    double a1 = A[1];
    double b0 = B[0];
    double b1 = B[1];
    double c0 = C[0];
    double c1 = C[1];

    // calculate the inverse matrix of A
    double matrixA[2][2], invMatrixA[2][2];
    double col0[2], col1[2];
    double det;

    vecSubtract(2, B, A, col0);
    vecSubtract(2, C, A, col1);

    // compute A and invA
    mat22Columns(col0, col1, matrixA);
    det = mat22Invert(matrixA, invMatrixA);

    // if det is 0, then our matrix is not invertible,
    // just quit the program for now
    if (det == 0)
    {
        mat22Print(invMatrixA);
        printf("Error: Matrix not invertible\n");
        exit(1);
    }

    // reserving memories for x and chi
    // betaMinusAlpha and gammaMinutesAlpha
    // are used to calculate chi later
    double x[2];
    double chi[3], betMinAlp[3], gamMinAlp[3], modulatedRGB[3];
    double vectPQ[2];

    vecSubtract(3, beta, alpha, betMinAlp);
    vecSubtract(3, gamma, alpha, gamMinAlp);
    printf("betMinAlp: %f,%f,%f, \n", betMinAlp[0], betMinAlp[1], betMinAlp[2]);
    printf("gamMinAlp: %f,%f,%f, \n", gamMinAlp[0], gamMinAlp[1], gamMinAlp[2]);

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
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    calculateRBG(3, chi, rgb, modulatedRGB);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
                }
            }
        }
        else
        {
            // printf("then here\n");
            // first half the triangle, we run to b0
            for (int x0 = ceil(a0); x0 <= floor(b0); x0++)
            {

                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    // mat22Print(invMatrixA);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    // printf("chir: %f,%f,%f\n", chi[0], chi[1], chi[2]);
                    // printf("rgb:%f,%f,%f\n", rgb[0], rgb[1], rgb[2]);
                    calculateRBG(3, chi, rgb, modulatedRGB);
                    // printf("color before abs: %f,%f,%f\n", modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);

                    // printf("color: %f,%f,%f\n", modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
                }
            }

            // for the second half, we run from b0+1 to c0
            for (int x0 = floor(b0) + 1; x0 <= floor(c0); x0++)
            {
                int upper = floor(a1 + ((c1 - a1) / (c0 - a0)) * (x0 - a0));
                int lower = ceil(b1 + ((c1 - b1) / (c0 - b0)) * (x0 - b0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    // printf("chir: %f,%f,%f\n", chi[0], chi[1], chi[2]);

                    calculateRBG(3, chi, rgb, modulatedRGB);
                    // printf("color: %f,%f,%f\n", modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
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
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    calculateRBG(3, chi, rgb, modulatedRGB);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
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
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    calculateRBG(3, chi, rgb, modulatedRGB);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
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
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    calculateRBG(3, chi, rgb, modulatedRGB);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
                }
            }

            // second half of the triangle
            for (int x0 = floor(c0) + 1; x0 <= floor(b0); x0++)
            {
                int upper = floor(c1 + ((b1 - c1) / (b0 - c0)) * (x0 - c0));
                int lower = ceil(a1 + ((b1 - a1) / (b0 - a0)) * (x0 - a0));
                for (int x1 = lower; x1 <= upper; x1++)
                {
                    x[0] = x0;
                    x[1] = x1;
                    findVectPQ(A, x, invMatrixA, vectPQ);
                    interpolate(alpha, beta, gamma, vectPQ, chi);
                    calculateRBG(3, chi, rgb, modulatedRGB);
                    pixSetRGB(x0, x1, modulatedRGB[0], modulatedRGB[1], modulatedRGB[2]);
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
void triRender(
    const double a[2], const double b[2], const double c[2],
    const double rgb[3], const double alpha[3], const double beta[3],
    const double gamma[3])
{
    // printf("Triangle coord: A(%f,%f); B(%f,%f); C(%f,%f)\n", a[0], a[1], b[0], b[1], c[0], c[1]);
    if (a[0] <= b[0] && a[0] <= c[0])
    {
        // printf("hits here\n");
        triRenderALeft(a, b, c,
                       rgb, alpha, beta, gamma);
    }

    else if (b[0] <= c[0] && b[0] <= a[0])
    {

        triRenderALeft(b, c, a,
                       rgb, beta, gamma, alpha);
    }

    else
    {

        triRenderALeft(c, a, b,
                       rgb, gamma, alpha, beta);
    }
}