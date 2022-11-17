/* A plane has no geometry uniforms. */
#define plaUNIFDIM 0

/* An implementation of getIntersection for bodies that are planes. */
void plaGetIntersection(
        int unifDim, const double unif[], const isoIsometry *isom, 
        const double p[3], const double d[3], double bound, 
        rayIntersection* inter) {
    double pLocal[3], dLocal[3];
    isoUnrotateDirection(isom, d, dLocal);
    isoUntransformPoint(isom, p, pLocal);

    if(dLocal[2] == 0){
        inter->t = rayNONE;
        return;
    }

    double t = -pLocal[2] / dLocal[2];
    if(t <= bound && t >= rayEPSILON){
        inter->t = t;
    }
    else
        inter->t = rayNONE;
    
}

/* An implementation of getTexCoordsAndNormal for bodies that are planes. */
void plaGetTexCoordsAndNormal(
        int unifDim, const double unif[], const isoIsometry *isom, 
        const double p[3], const double d[3], const rayIntersection *inter, 
        double texCoords[2], double normal[3]) {

    double pLocal[3], dLocal[3], normalLocal[3] = {0.0, 0.0, 1.0};
    isoUnrotateDirection(isom, d, dLocal);
    isoUntransformPoint(isom, p, pLocal);
    texCoords[0] = pLocal[0] + (inter->t * dLocal[0]);
    texCoords[1] = pLocal[1] + (inter->t * dLocal[1]);
    isoRotateDirection(isom, normalLocal, normal);
}

