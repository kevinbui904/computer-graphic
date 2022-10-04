/* Contributors: Shannon Liu & Kevin Bui*/

/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(const double m[2][2])
{
    int i, j;
    for (i = 0; i < 2; i += 1)
    {
        for (j = 0; j < 2; j += 1)
            printf("%f    ", m[i][j]);
        printf("\n");
    }
}

//Pretty print 3x3 matrices
void mat33Print(const double m[3][3])
{
    int i, j;
    for (i = 0; i < 3; i += 1)
    {
        for (j = 0; j < 3; j += 1)
            printf("%f    ", m[i][j]);
        printf("\n");
    }
}

//Pretty print 4x4 matrices
void mat44Print(const double m[4][4])
{
    int i, j;
    for (i = 0; i < 4; i += 1)
    {
        for (j = 0; j < 4; j += 1)
            printf("%f    ", m[i][j]);
        printf("\n");
    }
}

/* Returns the determinant of the matrix m. If the determinant is 0.0, then the
matrix is not invertible, and mInv is untouched. If the determinant is not 0.0,
then the matrix is invertible, and its inverse is placed into mInv. The output
CANNOT safely alias the input. */
double mat22Invert(const double m[2][2], double mInv[2][2])
{
    double det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
    if (det == 0)
    {
        return det;
    }
    double detInv = 1 / det;
    mInv[0][0] = detInv * m[1][1];
    mInv[0][1] = -(detInv * m[0][1]);
    mInv[1][0] = -(detInv * m[1][0]);
    mInv[1][1] = detInv * m[0][0];
    return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV.
The output CANNOT safely alias the input. */
void mat221Multiply(const double m[2][2], const double v[2],
                    double mTimesV[2])
{
    mTimesV[0] = (m[0][0] * v[0]) + (m[0][1] * v[1]);
    mTimesV[1] = (m[1][0] * v[0]) + (m[1][1] * v[1]);
}

/* Fills the matrix m from its two columns. The output CANNOT safely alias the
input. */
void mat22Columns(const double col0[2], const double col1[2], double m[2][2])
{
    m[0][0] = col0[0];
    m[0][1] = col1[0];
    m[1][0] = col0[1];
    m[1][1] = col1[1];
}

/* The theta parameter is an angle in radians. Sets the matrix m to the
rotation matrix corresponding to counterclockwise rotation of the plane through
the angle theta. */
void mat22Rotation(double theta, double m[2][2])
{
    m[0][0] = cos(theta);
    m[0][1] = -sin(theta);
    m[1][0] = sin(theta);
    m[1][1] = cos(theta);
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. The output CANNOT safely 
alias the input. */
void mat333Multiply(const double m[3][3], const double n[3][3], double mTimesN[3][3]){

    for(int i = 0; i<3; i++){
        for(int j = 0; j<3; j++){
            mTimesN[i][j] = (m[i][0]*n[0][j]) + (m[i][1]*n[1][j]) + (m[i][2]*n[2][j]);
        }
    }
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. The output CANNOT safely 
alias the input. */
void mat331Multiply(const double m[3][3], const double v[3], double mTimesV[3]){
    for(int i = 0; i<3; i++){
        mTimesV[i] = (m[i][0]*v[0]) + (m[i][1]*v[1]) + (m[i][2]*v[2]);
    }
}

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous 
coordinates. More precisely, the transformation first rotates through the angle 
theta (in radians, counterclockwise), and then translates by the vector t. */
void mat33Isometry(double theta, const double t[2], double isom[3][3]){
    isom[0][0] = cos(theta);
    isom[0][1] = -sin(theta);
    isom[0][2] = t[0];

    isom[1][0] = sin(theta);
    isom[1][1] = cos(theta);
    isom[1][2] = t[1];

    isom[2][0] = 0;
    isom[2][1] = 0;
    isom[2][2] = 1;
}

void identityMat(int dim, const double identity[]){
    for (int i = 0; i < dim; i++){
        for (int j = 0; j < dim; j++){
            if (i == j){
                identity[i][j] = 1;
            }
            else{
                identity[i][j] = 0;
            }
        }
    }
}

/*Given two 3x3 matrices, add the values*/
void mat33Add(const double m[3][3], const double n[3][3], double mAddN[3][3]){
    for(int i = 0; i<3; i++){
        for(int j = 0; j<3; j++){
            mAddN[i][j] = m[i][j] + n[i][j];
        }
    }
}

/*Given a 3x3 matrix and a constant, modulate the matrix*/
void mat33Mod(const double m, const double n[3][3], double mModN[3][3]){
    for(int i = 0; i<3; i++){
        for(int j = 0; j<3; j++){
            mModN[i][j] = m * n[i][j];
        }
    }
}

/* Computes the transpose M^T of the given matrix M. The output CANNOT safely 
alias the input. */
void mat33Transpose(const double m[3][3], double mT[3][3]) {
    for (int i = 0; i < 3; i += 1)
        for (int j = 0; j < 3; j += 1)
            mT[i][j] = m[j][i];
}

/* Given a length-1 3D vector axis and an angle theta (in radians), builds the 
rotation matrix for the rotation about that axis through that angle. */
void mat33AngleAxisRotation(double theta, const double axis[3], double rot[3][3]){
    double identity[3][3];
    identityMat(3, identity);
    
    double U[3][3] = {{0, -axis[2], axis[1]},
                      {axis[2], 0, -axis[0]}, 
                      {-axis[1], axis[0], 0}};

    double SinModU[3][3];
    mat33Mod(sin(theta), U, SinModU);

    double Usq[3][3] = {{(axis[0]^2) -1, axis[0]*axis[1], axis[0]*axis[2]},
                        {axis[0]*axis[1], axis[1]^2 -1, axis[1]*axis[2]}, 
                        {axis[0]*axis[2], axis[1]*axis[2], axis[2]^2 -1}};
    double CosModUsq[3][3];
    mat33Mod(1-cos(theta), Usq, CosModUsq);
    
    double M[3][3];
    mat33Add(identity, SinModU, M);
    mat33Add(M, CosModUsq, M);
}

/* Given two length-1 3D vectors u, v that are perpendicular to each other. 
Given two length-1 3D vectors a, b that are perpendicular to each other. Builds 
the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(const double u[3], const double v[3], const double a[3], 
        const double b[3], double rot[3][3]){
    double w[3], c[3];

    vec3Cross(u, v, w);
    vec3Cross(a, b, c);
    
    double R[3][3] = {{u[0], u[1], u[2]},
                      {v[0], v[1], v[2]},
                      {w[0], w[1], w[2]}};

    double S[3][3] = {{a[0], a[1], a[2]},
                      {b[0], b[1], b[2]},
                      {c[0], c[1], c[2]}};

    double RT[3][3];
    mat33Transpose(R, RT);

    mat333Multiply(RT, S, rot);
}

/* Computes the transpose M^T of the given matrix M. The output CANNOT safely 
alias the input. */
void mat44Transpose(const double m[4][4], double mT[4][4]) {
    for (int i = 0; i < 4; i += 1)
        for (int j = 0; j < 4; j += 1)
            mT[i][j] = m[j][i];
}

/* Multiplies m by n, placing the answer in mTimesN. The output CANNOT safely 
alias the input. */
void mat444Multiply(const double m[4][4], const double n[4][4], double mTimesN[4][4]){
    for(int i = 0; i<4; i++){
        for(int j = 0; j<4; j++){
            mTimesN[i][j] = (m[i][0]*n[0][j]) + (m[i][1]*n[1][j]) + (m[i][2]*n[2][j]) + (m[i][3]*n[3][j]);
        }
    }
}

/* Multiplies m by v, placing the answer in mTimesV. The output CANNOT safely 
alias the input. */
void mat441Multiply(const double m[4][4], const double v[4], double mTimesV[4]){
    for(int i = 0; i<4; i++){
        mTimesV[i] = (m[i][0]*v[0]) + (m[i][1]*v[1]) + (m[i][2]*v[2]) + (m[i][3]*v[3]);
    }
}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix 
representing the rotation followed in time by the translation. */
void mat44Isometry(const double rot[3][3], const double trans[3], double isom[4][4]){
    for(int i = 0; i<3; i++){
        for(int j = 0; j<3; j++){
            isom[i][j] = rot[i][j];
        }
    }

    for (int i = 0; i<3; i++){
        isom[i][3] = trans[i];
    }

    for (int i = 0; i<3; i++){
        isom[3][i] = 0;
    }

    isom[3][3]=1;
}
