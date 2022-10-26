/* Contributors: Shannon Liu & Kevin Bui*/

/*** 2 x 2 Matrices ***/

/* Pretty-prints the given matrix, with one line of text per row of matrix. */
void mat22Print(const float m[2][2])
{
    int i, j;
    for (i = 0; i < 2; i += 1)
    {
        for (j = 0; j < 2; j += 1)
            printf("%f    ", m[i][j]);
        printf("\n");
    }
}

// Pretty print 3x3 matrices
void mat33Print(const float m[3][3])
{
    int i, j;
    for (i = 0; i < 3; i += 1)
    {
        for (j = 0; j < 3; j += 1)
            printf("%f    ", m[i][j]);
        printf("\n");
    }
}

// Pretty print 4x4 matrices
void mat44Print(const float m[4][4])
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
float mat22Invert(const float m[2][2], float mInv[2][2])
{
    float det = m[0][0] * m[1][1] - m[0][1] * m[1][0];
    if (det == 0)
    {
        return det;
    }
    float detInv = 1 / det;
    mInv[0][0] = detInv * m[1][1];
    mInv[0][1] = -(detInv * m[0][1]);
    mInv[1][0] = -(detInv * m[1][0]);
    mInv[1][1] = detInv * m[0][0];
    return det;
}

/* Multiplies a 2x2 matrix m by a 2-column v, storing the result in mTimesV.
The output CANNOT safely alias the input. */
void mat221Multiply(const float m[2][2], const float v[2],
                    float mTimesV[2])
{
    mTimesV[0] = (m[0][0] * v[0]) + (m[0][1] * v[1]);
    mTimesV[1] = (m[1][0] * v[0]) + (m[1][1] * v[1]);
}

/* Fills the matrix m from its two columns. The output CANNOT safely alias the
input. */
void mat22Columns(const float col0[2], const float col1[2], float m[2][2])
{
    m[0][0] = col0[0];
    m[0][1] = col1[0];
    m[1][0] = col0[1];
    m[1][1] = col1[1];
}

/* The theta parameter is an angle in radians. Sets the matrix m to the
rotation matrix corresponding to counterclockwise rotation of the plane through
the angle theta. */
void mat22Rotation(float theta, float m[2][2])
{
    m[0][0] = cos(theta);
    m[0][1] = -sin(theta);
    m[1][0] = sin(theta);
    m[1][1] = cos(theta);
}

/* Multiplies the 3x3 matrix m by the 3x3 matrix n. The output CANNOT safely
alias the input. */
void mat333Multiply(const float m[3][3], const float n[3][3], float mTimesN[3][3])
{

    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            mTimesN[i][j] = (m[i][0] * n[0][j]) + (m[i][1] * n[1][j]) + (m[i][2] * n[2][j]);
        }
    }
}

/* Multiplies the 3x3 matrix m by the 3x1 matrix v. The output CANNOT safely
alias the input. */
void mat331Multiply(const float m[3][3], const float v[3], float mTimesV[3])
{
    for (int i = 0; i < 3; i++)
    {
        mTimesV[i] = (m[i][0] * v[0]) + (m[i][1] * v[1]) + (m[i][2] * v[2]);
    }
}

/* Builds a 3x3 matrix representing 2D rotation and translation in homogeneous
coordinates. More precisely, the transformation first rotates through the angle
theta (in radians, counterclockwise), and then translates by the vector t. */
void mat33Isometry(float theta, const float t[2], float isom[3][3])
{
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

/**
 * @brief Quickly generate dim by dim matrix
 *         apparently we can set array size using
 *          preceding parameters...
 *
 * @param dim
 * @param identity
 */
void identityMat(int dim, float identity[dim][dim])

{
    for (int i = 0; i < dim; i++)
    {
        for (int j = 0; j < dim; j++)
        {
            if (i == j)
            {
                identity[i][j] = 1;
            }
            else
            {
                identity[i][j] = 0;
            }
        }
    }
}

/*Given two 3x3 matrices, add the values*/
void mat33Add(const float m[3][3], const float n[3][3], float mAddN[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            mAddN[i][j] = m[i][j] + n[i][j];
        }
    }
}

/*Given a 3x3 matrix and a constant, modulate the matrix*/
void mat33Mod(const float m, const float n[3][3], float mModN[3][3])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            mModN[i][j] = m * n[i][j];
        }
    }
}

/* Computes the transpose M^T of the given matrix M. The output CANNOT safely
alias the input. */
void mat33Transpose(const float m[3][3], float mT[3][3])
{
    for (int i = 0; i < 3; i += 1)
        for (int j = 0; j < 3; j += 1)
            mT[i][j] = m[j][i];
}

/* Given a length-1 3D vector axis and an angle theta (in radians), builds the
rotation matrix for the rotation about that axis through that angle. */
void mat33AngleAxisRotation(float theta, const float axis[3], float rot[3][3])
{
    float identity[3][3];
    identityMat(3, identity);

    float U[3][3] = {{0, -axis[2], axis[1]},
                      {axis[2], 0, -axis[0]},
                      {-axis[1], axis[0], 0}};

    float sinModU[3][3];
    mat33Mod(sin(theta), U, sinModU);

    float Usq[3][3] = {{(axis[0] * axis[0]) - 1, axis[0] * axis[1], axis[0] * axis[2]},
                        {axis[0] * axis[1], axis[1] * axis[1] - 1, axis[1] * axis[2]},
                        {axis[0] * axis[2], axis[1] * axis[2], axis[2] * axis[2] - 1}};
    float CosModUsq[3][3];
    mat33Mod(1 - cos(theta), Usq, CosModUsq);

    float M[3][3];
    mat33Add(identity, sinModU, M);
    mat33Add(M, CosModUsq, rot);
}

/* Given two length-1 3D vectors u, v that are perpendicular to each other.
Given two length-1 3D vectors a, b that are perpendicular to each other. Builds
the rotation matrix that rotates u to a and v to b. */
void mat33BasisRotation(const float u[3], const float v[3], const float a[3],
                        const float b[3], float rot[3][3])
{
    float w[3], c[3];

    vec3Cross(u, v, w);
    vec3Cross(a, b, c);

    float R[3][3] = {{u[0], v[0], w[0]},
                      {u[1], v[1], w[1]},
                      {u[2], v[2], w[2]}};

    float S[3][3] = {{a[0], b[0], c[0]},
                      {a[1], b[1], c[1]},
                      {a[2], b[2], c[2]}};

    float RT[3][3];
    mat33Transpose(R, RT);
    mat333Multiply(S, RT, rot);
}

/* Computes the transpose M^T of the given matrix M. The output CANNOT safely
alias the input. */
void mat44Transpose(const float m[4][4], float mT[4][4])
{
    for (int i = 0; i < 4; i += 1)
        for (int j = 0; j < 4; j += 1)
            mT[i][j] = m[j][i];
}

/* Multiplies m by n, placing the answer in mTimesN. The output CANNOT safely
alias the input. */
void mat444Multiply(const float m[4][4], const float n[4][4], float mTimesN[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            mTimesN[i][j] = (m[i][0] * n[0][j]) + (m[i][1] * n[1][j]) + (m[i][2] * n[2][j]) + (m[i][3] * n[3][j]);
        }
    }
}

/* Multiplies m by v, placing the answer in mTimesV. The output CANNOT safely
alias the input. */
void mat441Multiply(const float m[4][4], const float v[4], float mTimesV[4])
{
    for (int i = 0; i < 4; i++)
    {
        mTimesV[i] = (m[i][0] * v[0]) + (m[i][1] * v[1]) + (m[i][2] * v[2]) + (m[i][3] * v[3]);
    }
}

/* Given a rotation and a translation, forms the 4x4 homogeneous matrix
representing the rotation followed in time by the translation. */
void mat44Isometry(const float rot[3][3], const float trans[3], float isom[4][4])
{
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            isom[i][j] = rot[i][j];
        }
    }

    for (int i = 0; i < 3; i++)
    {
        isom[i][3] = trans[i];
    }

    for (int i = 0; i < 3; i++)
    {
        isom[3][i] = 0;
    }

    isom[3][3] = 1;
}

/* Sets its argument to the 4x4 zero matrix (which consists entirely of 0s). */
void mat44Zero(float m[4][4]){
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            m[i][j] = 0.0;
        }
    }
}

/* Multiplies the transpose of the 3x3 matrix m by the 3x1 matrix v. To 
clarify, in math notation it computes M^T v. The output CANNOT safely alias the 
input. */

//figure out faster method
void mat331TransposeMultiply(const float m[3][3], const float v[3], float mTTimesV[3]){
 
    //NOTE:
    /*
        tranposing is swapping row and columns, so in our multiplication we just have to 
        swap row by column to achieve the same computation
    */
    for (int i = 0; i < 3; i++)
    {
        mTTimesV[i] = (m[0][i] * v[0]) + (m[1][i] * v[1]) + (m[2][i] * v[2]);
    }

}

/* Builds a 4x4 matrix for a viewport with lower left (0, 0) and upper right 
(width, height). This matrix maps a projected viewing volume 
[-1, 1] x [-1, 1] x [-1, 1] to screen [0, w] x [0, h] x [0, 1] (each interval 
in that order). */
void mat44Viewport(float width, float height, float view[4][4]){
    mat44Zero(view);
    view[0][0] = width / 2;
    view[0][3] = width / 2;
    
    view[1][1] = height / 2;
    view[1][3] = height / 2;

    view[2][2] = 0.5;
    view[2][3] = 0.5;
    view[3][3] = 1.0;
}

/* Inverse to the matrix produced by mat44Viewport. */
void mat44InverseViewport(float width, float height, float view[4][4]){
    mat44Zero(view);
    view[0][0] = 2 / width;
    view[0][3] = -1.0;
    
    view[1][1] = 2 / height;
    view[1][3] = -1.0;

    view[2][2] = 2.0;
    view[2][3] = -1.0;
    view[3][3] = 1.0;
}