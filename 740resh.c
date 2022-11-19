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
        /*NEW (KB+SL): setting inter to rayNone and index -1 to detect misses later*/
        inter->t = rayNONE;
        inter->t = -1;

        /*NEW (KB+SL): transform p and d into local coordinates*/
        double pLocal[3], dLocal[3];
        isoUnrotateDirection(isom, d, dLocal);
        isoUntransformPoint(isom, p, pLocal);
        /*NEW (KB+SL): need to check ALL triangles in the mesh*/
        double *vertices[3], bMinusA[3], cMinusA[3], normal[3], x[3],
                aMinusP[3], td[3], pq[2];
        int *triPointer;
        for(int triNumber = 0; triNumber < mesh->triNum; triNumber += 1){
            triPointer = meshGetTrianglePointer(mesh, triNumber);
            for(int i = 0; i < 3; i += 1){
                vertices[i] = meshGetVertexPointer(mesh, triPointer[i]);
            } 
            //compute the normal
            vecSubtract(3, vertices[1], vertices[0], bMinusA);
            vecSubtract(3, vertices[2], vertices[0], cMinusA);
            vec3Cross(bMinusA, cMinusA, normal);
            //compute t
            vecSubtract(3, vertices[0], pLocal, aMinusP);
            double normalDotD = vecDot(3, normal, dLocal);
            if(normalDotD != 0){
                double t = vecDot(3, normal, aMinusP) / vecDot(3, normal, dLocal);
                if (t <= bound && t >= rayEPSILON){
                    //check if we hit triangle
                    vecScale(3, t, dLocal, td);
                    vecAdd(3, pLocal, td, x);
                    reshGetPQ(vertices[0], bMinusA, cMinusA, x, pq);
                    if (pq[0] >= 0 && pq[1] >= 0 && pq[0]+pq[1] <= 1){
                        inter->t = t;
                        inter->index = triNumber;
                    }
                }
            }
        }
}

/* An implementation of getTexCoordsAndNormal for bodies that are reshes. 
Assumes that the data parameter points to an underlying meshMesh with attribute 
structure XYZSTNOP. */
void reshGetTexCoordsAndNormal(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        const rayIntersection *inter, double texCoords[2], double normal[3]) {
    meshMesh *mesh = (meshMesh *)data;

    /*NEW (KB+SL): transform p and d into local coordinates*/
    double pLocal[3], dLocal[3];
    isoUnrotateDirection(isom, d, dLocal);
    isoUntransformPoint(isom, p, pLocal);

    /* NEW (KB+SL): compute necessary vectors from winning triangle*/
     double *vertices[3], bMinusA[mesh->attrDim], cMinusA[mesh->attrDim], 
            x[3], td[3], pq[2], chi[mesh->attrDim], pBMinusA[mesh->attrDim], 
            qCMinusA[mesh->attrDim];
    int *triPointer;
    triPointer = meshGetTrianglePointer(mesh, inter->index);
    for(int i = 0; i < 3; i += 1){
        vertices[i] = meshGetVertexPointer(mesh, triPointer[i]);
    }
    //compute pq
    vecSubtract(mesh->attrDim, vertices[1], vertices[0], bMinusA);
    vecSubtract(mesh->attrDim, vertices[2], vertices[0], cMinusA);
    vecScale(3, inter->t, dLocal, td);
    vecAdd(3, pLocal, td, x);
    reshGetPQ(vertices[0], bMinusA, cMinusA, x, pq);

    //linear interpolate
    vecScale(mesh->attrDim, pq[0], bMinusA, pBMinusA);
    vecScale(mesh->attrDim, pq[1], cMinusA, qCMinusA);
    vecAdd(mesh->attrDim, vertices[0], pBMinusA, chi);
    vecAdd(mesh->attrDim, chi, qCMinusA, chi);

    //extracting the interpolated values
    double localNormal[3];
    vecUnit(3, &(chi[5]), localNormal);
    isoRotateDirection(isom, localNormal, normal);
    texCoords[0] = chi[3];
    texCoords[1] = chi[4];
}

