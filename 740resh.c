/* A resh is a ray-tracing mesh. It has no geometry uniforms outside the 
attached meshMesh. */
#define reshUNIFDIM 0

/* Given vectors a, b - a, and c - a describing a triangle, with the first three 
entries being XYZ. Given point x, with the first three entries being XYZ, such 
that (up to numerical precision) x - a = p (b - a) + q (c - a). Computes p and 
q. Returns 1 on success or 0 on failure. */
int reshGetPQ(
        const double a[], const double bMinusA[], const double cMinusA[], 
        const double x[], double pq[2]) {
    /* For the 3x2 matrix A with columns b - a, c - a, compute A^T A. */
    double aTA[2][2];
    aTA[0][0] = vecDot(3, bMinusA, bMinusA);
    aTA[0][1] = vecDot(3, bMinusA, cMinusA);
    aTA[1][0] = aTA[0][1];
    aTA[1][1] = vecDot(3, cMinusA, cMinusA);
    /* Compute the 2x2 matrix (A^T A)^-1 if possible. */
    double aTAInv[2][2];
    if (mat22Invert(aTA, aTAInv) == 0.0)
        return 0;
    /* Compute the 2x3 matrix (A^T A)^-1 A^T. */
    double aTAInvAT[2][3];
    for (int i = 0; i < 2; i += 1)
        for (int j = 0; j < 3; j += 1)
            aTAInvAT[i][j] = 
                aTAInv[i][0] * bMinusA[j] + aTAInv[i][1] * cMinusA[j];
    /* Then pq = (A^T A)^-1 A^T (x - a). */
    double xMinusA[3];
    vecSubtract(3, x, a, xMinusA);
    pq[0] = vecDot(3, aTAInvAT[0], xMinusA);
    pq[1] = vecDot(3, aTAInvAT[1], xMinusA);
    return 1;
}

/* An implementation of getIntersection for bodies that are reshes. Assumes that 
the data parameter points to an underlying meshMesh with attribute structure 
XYZSTNOP. */
void reshGetIntersection(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        double bound, rayIntersection* inter) {
    meshMesh *mesh = (meshMesh *)data;
    /* YOUR CODE GOES HERE. (MINE IS 22 LINES WITH A BIG HELPER FUNCTION.) */
}

/* An implementation of getTexCoordsAndNormal for bodies that are reshes. 
Assumes that the data parameter points to an underlying meshMesh with attribute 
structure XYZSTNOP. */
void reshGetTexCoordsAndNormal(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        const rayIntersection *inter, double texCoords[2], double normal[3]) {
    meshMesh *mesh = (meshMesh *)data;
    /* REPLACE THESE THREE LINES WITH YOUR CODE. (MINE IS 17 LINES.) */
    texCoords[0] = 0.5;
    texCoords[1] = 0.5;
    vec3Set(0.0, 0.0, 1.0, normal);
}

