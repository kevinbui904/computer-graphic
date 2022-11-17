/*Author: Thien K. M. Bui and Shannon Liu. Extended from professor Josh R. Davis'
    starter code. Computer Graphic Carleton College Fall 2022*/

/* A sphere has exactly one geometry uniform: its radius. */
#define sphUNIFDIM 1

/* An implementation of getIntersection for bodies that are spheres. */
void sphGetIntersection(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        double bound, rayIntersection* inter) {
    double r = unif[0];
    double c[3];
    vecCopy(3, isom->translation, c);
    double pMinusC[3], dPMinusC, dD, rSq, disc, t;
    vecSubtract(3, p, c, pMinusC);
    dPMinusC = vecDot(3, d, pMinusC);
    dD = vecDot(3, d, d);
    rSq = r * r;
    disc = dPMinusC * dPMinusC - dD * (vecDot(3, pMinusC, pMinusC) - rSq);

    if (disc <= 0) {
        inter->t = rayNONE;
        return;
    }
    disc = sqrt(disc);
    t = (-dPMinusC - disc) / dD;
    if (rayEPSILON <= t && t <= bound) {
        inter->t = t;
        return;
    }
    t = (-dPMinusC + disc) / dD;
    if (rayEPSILON <= t && t <= bound) {
        inter->t = t;
        return;
    }
    inter->t = rayNONE;
}

/* An implementation of getTexCoordsAndNormal for bodies that are spheres. */
void sphGetTexCoordsAndNormal(
        int unifDim, const double unif[], const void *data, 
        const isoIsometry *isom, const double p[3], const double d[3], 
        const rayIntersection *inter, double texCoords[2], double normal[3]) {

    double xyzWorld[3], tD[3], unNormalizedNormal[3];
    vecScale(3, inter->t, d, tD);
    vecAdd(3, p, tD, xyzWorld);
    vecSubtract(3, xyzWorld, isom->translation, unNormalizedNormal);
    vecUnit(3, unNormalizedNormal, normal);
    
    double xyzLocal[3], rho, phi, theta;
    isoUntransformPoint(isom, xyzWorld, xyzLocal);
    vec3Rectangular(xyzLocal, &rho, &phi, &theta);
    texCoords[0] = theta / (2*M_PI);
    texCoords[1] = 1 - (phi/M_PI);

}

