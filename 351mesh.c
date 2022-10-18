
/* Contributors: Shannon Liu & Kevin Bui*/

/*** Public: For header file ***/

/*** Creating and destroying ***/

/* Feel free to read the struct's members, but don't write them, except through 
the accessors below such as meshSetTriangle, meshSetVertex. */
typedef struct meshMesh meshMesh;
struct meshMesh {
	int triNum, vertNum, attrDim;
	int *tri;						/* triNum * 3 ints */
	double *vert;					/* vertNum * attrDim doubles */
};

/* Initializes a mesh with enough memory to hold its triangles and vertices. 
Does not actually fill in those triangles or vertices with useful data. When 
you are finished with the mesh, you must call meshFinalize to deallocate its 
backing resources. */
int meshInitialize(meshMesh *mesh, int triNum, int vertNum, int attrDim) {
	mesh->tri = (int *)malloc(triNum * 3 * sizeof(int) +
		vertNum * attrDim * sizeof(double));
	if (mesh->tri != NULL) {
		mesh->vert = (double *)&(mesh->tri[triNum * 3]);
		mesh->triNum = triNum;
		mesh->vertNum = vertNum;
		mesh->attrDim = attrDim;
	}
    
	return (mesh->tri == NULL);
}

/* Sets the trith triangle to have vertex indices i, j, k. */
void meshSetTriangle(meshMesh *mesh, int tri, int i, int j, int k) {
	if (0 <= tri && tri < mesh->triNum) {
		mesh->tri[3 * tri] = i;
		mesh->tri[3 * tri + 1] = j;
		mesh->tri[3 * tri + 2] = k;
	}
}

/* Returns a pointer to the trith triangle. For example:
	int *triangle13 = meshGetTrianglePointer(&mesh, 13);
	printf("%d, %d, %d\n", triangle13[0], triangle13[1], triangle13[2]); */
int *meshGetTrianglePointer(const meshMesh *mesh, int tri) {
	if (0 <= tri && tri < mesh->triNum)
		return &mesh->tri[tri * 3];
	else
		return NULL;
}

/* Sets the vertth vertex to have attributes attr. */
void meshSetVertex(meshMesh *mesh, int vert, const double attr[]) {
	int k;
    //printf("enter meshSetVertex\n");
	if (0 <= vert && vert < mesh->vertNum)
		for (k = 0; k < mesh->attrDim; k += 1){
            mesh->vert[mesh->attrDim * vert + k] = attr[k];
            //printf("vertex %i: %f\n", k, mesh->vert[mesh -> attrDim * vert + k]);
        }			
}

/* Returns a pointer to the vertth vertex. For example:
	double *vertex13 = meshGetVertexPointer(&mesh, 13);
	printf("x = %f, y = %f\n", vertex13[0], vertex13[1]); */
double *meshGetVertexPointer(const meshMesh *mesh, int vert) {
	if (0 <= vert && vert < mesh->vertNum)
		return &mesh->vert[vert * mesh->attrDim];
	else
		return NULL;
}

/* Deallocates the resources backing the mesh. This function must be called 
when you are finished using a mesh. */
void meshFinalize(meshMesh *mesh) {
	free(mesh->tri);
}

/*** Writing and reading files ***/

/* Helper function for meshInitializeFile. */
int meshFileError(
        meshMesh *mesh, FILE *file, const char *cause, const int line) {
	fprintf(stderr, "error: meshInitializeFile: %s at line %d\n", cause, line);
	fclose(file);
	meshFinalize(mesh);
	return 3;
}

/* Initializes a mesh from a mesh file. The file format is documented at 
meshSaveFile. This function does not do as much error checking as one might 
like. Use it only on trusted, non-corrupted files, such as ones that you have 
recently created using meshSaveFile. Returns 0 on success, non-zero on failure. 
Don't forget to invoke meshFinalize when you are done using the mesh. */
int meshInitializeFile(meshMesh *mesh, const char *path) {
	FILE *file = fopen(path, "r");
	if (file == NULL) {
		fprintf(stderr, "error: meshInitializeFile: fopen failed\n");
		return 1;
	}
	int year, month, day, triNum, vertNum, attrDim;
	// Future work: Check version.
	if (fscanf(file, "Carleton College CS 311 mesh version %d/%d/%d\n", &year, 
			&month, &day) != 3) {
		fprintf(stderr, "error: meshInitializeFile: bad header at line 1\n");
		fclose(file);
		return 1;
	}
	if (fscanf(file, "triNum %d\n", &triNum) != 1) {
		fprintf(stderr, "error: meshInitializeFile: bad triNum at line 2\n");
		fclose(file);
		return 2;
	}
	if (fscanf(file, "vertNum %d\n", &vertNum) != 1) {
		fprintf(stderr, "error: meshInitializeFile: bad vertNum at line 3\n");
		fclose(file);
		return 3;
	}
	if (fscanf(file, "attrDim %d\n", &attrDim) != 1) {
		fprintf(stderr, "error: meshInitializeFile: bad attrDim at line 4\n");
		fclose(file);
		return 4;
	}
	if (meshInitialize(mesh, triNum, vertNum, attrDim) != 0) {
		fclose(file);
		return 5;
	}
	int line = 5, *tri, j, check;
	if (fscanf(file, "%d Triangles:\n", &check) != 1 || check != triNum)
		return meshFileError(mesh, file, "bad header", line);
	for (line = 6; line < triNum + 6; line += 1) {
		tri = meshGetTrianglePointer(mesh, line - 6);
		if (fscanf(file, "%d %d %d\n", &tri[0], &tri[1], &tri[2]) != 3)
			return meshFileError(mesh, file, "bad triangle", line);
		if (0 > tri[0] || tri[0] >= vertNum || 0 > tri[1] || tri[1] >= vertNum 
				|| 0 > tri[2] || tri[2] >= vertNum)
			return meshFileError(mesh, file, "bad index", line);
	}
	double *vert;
	if (fscanf(file, "%d Vertices:\n", &check) != 1 || check != vertNum)
		return meshFileError(mesh, file, "bad header", line);
	for (line = triNum + 7; line < triNum + 7 + vertNum; line += 1) {
		vert = meshGetVertexPointer(mesh, line - (triNum + 7));
		for (j = 0; j < attrDim; j += 1) {
			if (fscanf(file, "%lf ", &vert[j]) != 1)
				return meshFileError(mesh, file, "bad vertex", line);
		}
		if (fscanf(file, "\n") != 0)
			return meshFileError(mesh, file, "bad vertex", line);
	}
	// Future work: Check EOF.
	fclose(file);
	return 0;
}

/* Saves a mesh to a file in a simple custom format (not any industry 
standard). Returns 0 on success, non-zero on failure. The first line is a 
comment of the form 'Carleton College CS 311 mesh version YYYY/MM/DD'.

I now describe version 2019/01/15. The second line says 'triNum [triNum]', 
where the latter is an integer value. The third and fourth lines do the same 
for vertNum and attrDim. The fifth line says '[triNum] Triangles:'. Then there 
are triNum lines, each holding three integers between 0 and vertNum - 1 
(separated by a space). Then there is a line that says '[vertNum] Vertices:'. 
Then there are vertNum lines, each holding attrDim floating-point numbers 
(terminated by a space). */
int meshSaveFile(const meshMesh *mesh, const char *path) {
	FILE *file = fopen(path, "w");
	if (file == NULL) {
		fprintf(stderr, "error: meshSaveFile: fopen failed\n");
		return 1;
	}
	fprintf(file, "Carleton College CS 311 mesh version 2019/01/15\n");
	fprintf(file, "triNum %d\n", mesh->triNum);
	fprintf(file, "vertNum %d\n", mesh->vertNum);
	fprintf(file, "attrDim %d\n", mesh->attrDim);
	fprintf(file, "%d Triangles:\n", mesh->triNum);
	int i, j;
	int *tri;
	for (i = 0; i < mesh->triNum; i += 1) {
		tri = meshGetTrianglePointer(mesh, i);
		fprintf(file, "%d %d %d\n", tri[0], tri[1], tri[2]);
	}
	fprintf(file, "%d Vertices:\n", mesh->vertNum);
	double *vert;
	for (i = 0; i < mesh->vertNum; i += 1) {
		vert = meshGetVertexPointer(mesh, i);
		for (j = 0; j < mesh->attrDim; j += 1)
			fprintf(file, "%f ", vert[j]);
		fprintf(file, "\n");
	}
	fclose(file);
	return 0;
}

/*
	Helper function for triRender, we're applying the viewport transformation here
*/
void triClippedRender(depthBuffer *buf, const double viewport[4][4], const shaShading *sha, const double unif[], const texTexture *tex[],  double varyA[],  double varyB[],  double varyC[]){
		
		double viewportA[4];
		double viewportB[4];
		double viewportC[4];
		//copy ALL of varys into finalVarys
		vecCopy(sha->varyDim, varyA, varyA);
		vecCopy(sha->varyDim, varyB, varyB);
		vecCopy(sha->varyDim, varyC, varyC);

		//viewport transformation, we will put the viewport coordinates back into our varyings
		//after

		//we have hard coded mat441Multiply to ONLY grab up to 4 elements of vary,
		//therefore the size of vary at this stage does not matter
        mat441Multiply(viewport, varyA, viewportA);
        mat441Multiply(viewport, varyB, viewportB);
        mat441Multiply(viewport, varyC, viewportC);

		//recopy new XYZW back into our varyings
		//we are only changing the first 4 elements of our varyings
		vecCopy(4, viewportA, varyA);
		vecCopy(4, viewportB, varyB);
		vecCopy(4, viewportC, varyC);

        //homogeneous division on the entire varying vector 
        vecScale(sha->varyDim, 1/viewportA[3], varyA, varyA);
        vecScale(sha->varyDim, 1/viewportB[3], varyB, varyB);
        vecScale(sha->varyDim, 1/viewportC[3], varyC, varyC);
		printf("varyA: %f\n", varyA[0]);
		exit(1);
		printf("before trirender\n");
	    triRender(sha, buf, unif, tex, varyA, varyB, varyC);
		printf("dies after triRender\n");

}
/*** Rendering ***/

/*meshRender loops over the triangle in the mesh, for each triangle doing this:
	1. get the triangle's three attribute vectors from the mesh
	2. transform each attribute vector to a varying vector using the vertex shader
	3. pass those three varying vectors to triRender
	4. triRender rasterizes and interpolates based on those varyings
	4. the fragment shader receives one interpolated varying vector
	5. triRender passes the fragment shader's output color to pixSetRGB*/

/* Renders the mesh. If the mesh and the shading have differing values for 
attrDim, then prints an error message and does not render anything. */
void meshRender(
        const meshMesh *mesh, depthBuffer *buf, const double viewport[4][4],
        const shaShading *sha, const double unif[], const texTexture *tex[]) {

	//Print error message if mesh attrDim and sha attrDim differ
	if (mesh -> attrDim != sha -> attrDim){
        printf("error: mesh and shading have differing attrDim\n");
		exit(1);
        return;
    }
    double *vertices[3];
    int *triPointer;

	double varyA[sha->varyDim];
	double varyB[sha->varyDim];
	double varyC[sha->varyDim];

	//used for clipping
	double t1, t2;
	double tempA[sha->varyDim], tempB[sha->varyDim], tempC[sha->varyDim];

    for(int triNumber = 0; triNumber < mesh->triNum; triNumber += 1){
        triPointer = meshGetTrianglePointer(mesh, triNumber);
        for(int i = 0; i < 3; i += 1){
            vertices[i] = meshGetVertexPointer(mesh, triPointer[i]);
			printf("vertices num: %i\n", i);
        }   
		sha->shadeVertex(sha->unifDim, unif, sha->attrDim, vertices[0], sha->varyDim, varyA);
		sha->shadeVertex(sha->unifDim, unif, sha->attrDim, vertices[1], sha->varyDim, varyB);
		sha->shadeVertex(sha->unifDim, unif, sha->attrDim, vertices[2], sha->varyDim, varyC);
        
		printf("triNumber: %d\n", triNumber);
		//handle clipping 

		//if vertexA is clipped
		//a3 ≤ 0 or a3 < -a2
		
		// t = (a2 + a3) / (a2 + a3 - b2 - b3
		if (varyA[3] <= 0 || varyA[3] <= -(varyA[2])){
			printf("inside varyA clip\n");
			//only 2 vertices are clipped
			if (varyB[3] <= 0 || varyB[3] <= -(varyB[2]))
			{
				//only render if vertexC is not clipped
				if (varyC[3] >= 0 && varyC[3] >= -(varyC[2]))
				{
					t1 = (varyA[2] + varyA[3]) / (varyA[2] + varyA[3] - varyC[2] - varyC[3]);
					t2 = (varyB[2] + varyB[3]) / (varyB[2] + varyB[3] - varyC[2] - varyC[3]);

					//compute tempA–
					//tempA = a + t(c-a)
					vecSubtract(sha->varyDim, varyC, varyA, tempA);
					vecScale(sha->varyDim, t1, tempA, tempA);
					vecAdd(sha->varyDim, varyA, tempA, tempA);

					vecSubtract(sha->varyDim, varyC, varyB, tempB);
					vecScale(sha->varyDim, t2, tempB, tempB);
					vecAdd(sha->varyDim, varyB, tempB, tempB);
				
					triClippedRender(buf, viewport, sha, unif, tex, tempA, tempB, varyC);
				}
			}
			//only a is clipped, so we need to separate into 2 different triangles
			else
			{
				t1 = (varyA[2] + varyA[3]) / (varyA[2] + varyA[3] - varyC[2] - varyC[3]);
				t2 = (varyA[2] + varyA[3]) / (varyA[2] + varyA[3] - varyB[2] - varyB[3]);

				//scaling varyings, then calling triRender on all of these
				vecSubtract(sha->varyDim, varyC, varyA, tempA);
				vecScale(sha->varyDim, t1, tempA, tempA);
				vecAdd(sha->varyDim, varyA, tempA, tempA);
				triClippedRender(buf, viewport, sha, unif, tex, tempA, varyB, varyC);
				
				//rendering the second triangle
				vecSubtract(sha->varyDim, varyB, varyA, tempA);
				vecScale(sha->varyDim, t2, tempA, tempA);
				vecAdd(sha->varyDim, varyA, tempA, tempA);
				triClippedRender(buf, viewport, sha, unif, tex, tempA, varyB, varyC);
			}
		}
		//handle these for when B is clipped
		else if (varyB[3] <= 0 || varyB[3] <= -(varyB[2]))
		{
			printf("inside varyB clip\n");
			//only 2 vertices are clipped
			if (varyC[3] <= 0 || varyC[3] <= -(varyC[2]))
			{
				//only render if vertexA is not also clipped
				if (varyA[3] >= 0 && varyA[3] >= -(varyA[2]))
				{
					t1 = (varyB[2] + varyB[3]) / (varyB[2] + varyB[3] - varyA[2] - varyA[3]);
					t2 = (varyC[2] + varyC[3]) / (varyC[2] + varyC[3] - varyA[2] - varyA[3]);

					vecSubtract(sha->varyDim, varyA, varyB, tempB);
					vecScale(sha->varyDim, t1, tempB, tempB);
					vecAdd(sha->varyDim, varyB, tempB, tempB);

					vecSubtract(sha->varyDim, varyA, varyC, tempC);
					vecScale(sha->varyDim, t2, tempC, tempC);
					vecAdd(sha->varyDim, varyC, tempC, tempC);

					triClippedRender(buf, viewport, sha, unif, tex, varyA, tempB, tempC);
		
				}
			}
			else
			{
				printf("inside 1 vertices clip\n");

				t1 = (varyB[2] + varyB[3]) / (varyB[2] + varyB[3] - varyA[2] - varyA[3]);
				t2 = (varyB[2] + varyB[3]) / (varyB[2] + varyB[3] - varyC[2] - varyC[3]);

				printf("(t1, t2): %f, %f\n", t1, t2);

				vecSubtract(sha->varyDim, varyA, varyB, tempB);
				vecScale(sha->varyDim, t1, tempB, tempB);
				vecAdd(sha->varyDim, varyB, tempB, tempB);
			
				triClippedRender(buf, viewport, sha, unif, tex, varyA, tempB, varyC);
				printf("dies here inside first render\n");

				//rendering the second triangle
				vecSubtract(sha->varyDim, varyC, varyB, tempB);
				vecScale(sha->varyDim, t2, tempB, tempB);
				vecAdd(sha->varyDim, varyB, tempB, tempB);
				triClippedRender(buf, viewport, sha, unif, tex, varyA, tempB, varyC);

			}
		}
		else if (varyC[3] <= 0 || varyC[3] <= -(varyC[2]))
		{
			//2 vertices are clipped
			if (varyA[3] <= 0 || varyA[3] <= -(varyA[2]))
			{
				//only render if vertexB is not clipped
				if (varyB[3] >= 0 && varyB[3] >= -(varyB[2]))
				{
					t1 = (varyC[2] + varyC[3]) / (varyC[2] + varyC[3] - varyB[2] - varyB[3]);
					t2 = (varyA[2] + varyA[3]) / (varyA[2] + varyA[3] - varyB[2] - varyB[3]);

					vecSubtract(sha->varyDim, varyB, varyC, tempC);
					vecScale(sha->varyDim, t1, tempC, tempC);
					vecAdd(sha->varyDim, varyC, tempC, tempC);

					vecSubtract(sha->varyDim, varyB, varyA, tempA);
					vecScale(sha->varyDim, t2, tempA, tempA);
					vecAdd(sha->varyDim, varyA, tempA, tempA);
		
					triClippedRender(buf, viewport, sha, unif, tex, tempA, varyB, tempC);
				}
			}
			else
			{
				t1 = (varyC[2] + varyC[3]) / (varyC[2] + varyC[3] - varyB[2] - varyB[3]);
				t2 = (varyC[2] + varyC[3]) / (varyC[2] + varyC[3] - varyA[2] - varyA[3]);

				vecSubtract(sha->varyDim, varyB, varyC, tempC);
				vecScale(sha->varyDim, t1, tempC, tempC);
				vecAdd(sha->varyDim, varyC, tempC, tempC);
				triClippedRender(buf, viewport, sha, unif, tex, varyA, varyB, tempC);

				//rendering the second triangle
				vecSubtract(sha->varyDim, varyA, varyC, tempC);
				vecScale(sha->varyDim, t2, tempC, tempC);
				vecAdd(sha->varyDim, varyC, tempC, tempC);
				triClippedRender(buf, viewport, sha, unif, tex, varyA, varyB, tempC);
			}
		}
		else{
			triClippedRender(buf, viewport, sha, unif, tex, varyA, varyB, varyC);
		}
    }
}

