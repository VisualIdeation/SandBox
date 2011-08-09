

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_normalize(float v[3])
{
	float	d;

	d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	if (d == 0.0) {
		printf("normalize: zero length vector");
		v[0] = d = 1.0;
	}
	d = 1 / d;
	v[0] *= d;
	v[1] *= d;
	v[2] *= d;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_crossprod(float v1[3], float v2[3], float prod[3])
{
	float	p[3];	/* in case prod == v1 or v2 */

	p[0] = v1[1] * v2[2] - v2[1] * v1[2];
	p[1] = v1[2] * v2[0] - v2[2] * v1[0];
	p[2] = v1[0] * v2[1] - v2[0] * v1[1];
	prod[0] = p[0];
	prod[1] = p[1];
	prod[2] = p[2];
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_MakeCylinder(float *pt1, float *pt2, float radius, long nsides, int cap1, int cap2)
{
	long	eq = 0, i;
	float	vperp[3], uperp[3], diff[3], n1[3], n2[3], n0[3] /* dummy */ ;
	float	v0[3], v1[3], v2[3], v3[3];
	float	theta, theta1;

	n0[0] = n0[1] = n0[2] = 0.0;
	for (i = 0; i < 3; i++)
		if (pt1[i] == pt2[i])
			eq++;
	for (i = 0; i < 3; i++)
		diff[i] = pt1[i] - pt2[i];
	if (eq == 2) {		/* it's axis-aligned */
		if (pt1[0] != pt2[0]) {
			vperp[0] = vperp[2] = 0.0;
			vperp[1] = 1.0;
		} else if (pt1[1] != pt2[1]) {
			vperp[0] = vperp[1] = 0.0;
			vperp[2] = 1.0;
		} else {
			vperp[1] = vperp[2] = 0.0;
			vperp[0] = 1.0;
		}
	} else {		/* it's a general cylinder */

		if (diff[0] == 0.0) {
			vperp[0] = 0.0;
			vperp[1] = 1.0;
			vperp[2] = -diff[1] / diff[2];
		} else if (diff[1] == 0.0) {
			vperp[1] = 0.0;
			vperp[0] = 1.0;
			vperp[2] = -diff[0] / diff[2];
		} else {
			vperp[2] = 0.0;
			vperp[0] = 1.0;
			vperp[1] = -diff[0] / diff[1];
		}
		SANDBOX_normalize(vperp);
	}
	SANDBOX_crossprod(diff, vperp, uperp);
	SANDBOX_normalize(uperp);

	for (theta = 0.0; theta < 2.0 * SANDBOX_PI - CYLINDER_EPSILON; theta += 2.0 * SANDBOX_PI / nsides) {
		theta1 = theta + 2.0 * SANDBOX_PI / nsides;
		for (i = 0; i < 3; i++) {
			n1[i] = vperp[i] * cos(theta) + uperp[i] * sin(theta);
			n2[i] = vperp[i] * cos(theta1) + uperp[i] * sin(theta1);
		}
		v0[0] = pt1[0] + n1[0] * radius;
		v0[1] = pt1[1] + n1[1] * radius;
		v0[2] = pt1[2] + n1[2] * radius;
		v1[0] = pt2[0] + n1[0] * radius;
		v1[1] = pt2[1] + n1[1] * radius;
		v1[2] = pt2[2] + n1[2] * radius;
		v2[0] = pt2[0] + n2[0] * radius;
		v2[1] = pt2[1] + n2[1] * radius;
		v2[2] = pt2[2] + n2[2] * radius;
		v3[0] = pt1[0] + n2[0] * radius;
		v3[1] = pt1[1] + n2[1] * radius;
		v3[2] = pt1[2] + n2[2] * radius;

		glBegin(GL_POLYGON);					/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
		glNormal3fv(n1);					/* IGL: n3f(n1); */
		glVertex3fv(v0);					/* IGL: v3f(v0); */
		glNormal3fv(n1);					/* IGL: n3f(n1); */
		glVertex3fv(v1);					/* IGL: v3f(v1); */
		glNormal3fv(n2);					/* IGL: n3f(n1); */
		glVertex3fv(v2);					/* IGL: v3f(v2); */
		glNormal3fv(n2);					/* IGL: n3f(n1); */
		glVertex3fv(v3);					/* IGL: v3f(v3); */
		glEnd();						/* IGL: endpolygon(); */

		if (cap1 == CYLINDER_CAP) {
			float	ndiff[3];

			ndiff[0] = -diff[0];
			ndiff[1] = -diff[1];
			ndiff[2] = -diff[2];

			glBegin(GL_POLYGON);				/* IGL: bgnpolygon(); -- TODO: consider GL_TRIANGLES type */
			glNormal3fv(ndiff);				/* IGL: n3f(ndiff); */
			glVertex3fv(pt2);				/* IGL: v3f(pt2); */
			glVertex3fv(v2);				/* IGL: v3f(v2); */
			glVertex3fv(v1);				/* IGL: v3f(v1); */
			glEnd();					/* IGL: endpolygon(); */
		}

		if (cap2 == CYLINDER_CAP) {
			glBegin(GL_POLYGON);				/* IGL: bgnpolygon(); -- TODO: consider GL_TRIANGLES type */
			glNormal3fv(diff);				/* IGL: n3f(diff); */
			glVertex3fv(pt1);				/* IGL: v3f(pt1); */
			glVertex3fv(v0);				/* IGL: v3f(v0); */
			glVertex3fv(v3);				/* IGL: v3f(v3); */
			glEnd();					/* IGL: endpolygon(); */
		}
	}
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* HMMM, is this really a pyramid?  The code looks like it's drawing a cube -- and */
/*   I'm not just refering to the variable names, though they also point in that   */
/*   direction.                                                                    */
void SANDBOX_drawpyramid()
{
	float	cube[8][3] = {
			-0.0, 1.0, 0.0,
			-1.0, 0.0, 1.0,
			1.0, 0.0, 1.0,
			0.0, 1.0, 0.0,
			0.0, 1.0, -0.0,
			1.0, 0.0, -1.0,
			-1.0, 0.0, -1.0,
			-0.0, 1.0, -0.0
		};

	float	cube_normals[6][3] = {
			0.0, 0.0, 1.0,
			1.0, 0.0, 0.0,
			0.0, 0.0, -1.0,
			-1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, -1.0, 0.0
		};

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[0]);					/* IGL: n3f(cube_normals[0]); */
	glVertex3fv(cube[0]);
	glVertex3fv(cube[1]);
	glVertex3fv(cube[2]);
	glVertex3fv(cube[3]);						/* IGL: v3f(cube[0]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[1]);					/* IGL: n3f(cube_normals[1]); */
	glVertex3fv(cube[4]);
	glVertex3fv(cube[5]);
	glVertex3fv(cube[2]);
	glVertex3fv(cube[3]);						/* IGL: v3f(cube[4]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[2]);					/* IGL: n3f(cube_normals[2]); */
	glVertex3fv(cube[4]);
	glVertex3fv(cube[7]);
	glVertex3fv(cube[6]);
	glVertex3fv(cube[5]);						/* IGL: v3f(cube[4]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[3]);					/* IGL: n3f(cube_normals[3]); */
	glVertex3fv(cube[7]);
	glVertex3fv(cube[6]);
	glVertex3fv(cube[1]);
	glVertex3fv(cube[0]);						/* IGL: v3f(cube[7]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[4]);					/* IGL: n3f(cube_normals[4]); */
	glVertex3fv(cube[0]);
	glVertex3fv(cube[3]);
	glVertex3fv(cube[4]);
	glVertex3fv(cube[7]);						/* IGL: v3f(cube[0]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[5]);					/* IGL: n3f(cube_normals[5]); */
	glVertex3fv(cube[6]);
	glVertex3fv(cube[5]);
	glVertex3fv(cube[2]);
	glVertex3fv(cube[1]);						/* IGL: v3f(cube[6]; ... */
	glEnd();							/* IGL: endpolygon(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* can not put transparency modifiers here because the sat picture
wants it disabled, but the helo picture needs it enabled */

void SANDBOX_drawplane()
{

	float		cube_normals[3] = { 0.0, 1.0, 0.0 };
	float		vertex[3];
	float		ranger;
	float		xloc, xplusloc;
register int	y, x;

	ranger = SANDBOX_TMESHSIZE - 1;

	glNormal3fv(cube_normals);					/* IGL: n3f(cube_normals); */

	for (x = SANDBOX_TMESHSIZE - 2; x >= 0; x--) {
		xloc = x / ranger;
		xplusloc = (x + 1) / ranger;

		glBegin(GL_TRIANGLE_STRIP);				/* IGL: bgntmesh(); */
		for (y = SANDBOX_TMESHSIZE - 1; y >= 0; y--) {
			vertex[2] = y / ranger;

			vertex[0] = xplusloc;
			vertex[1] = SANDBOX_planeElev[x + 1][y];
			glTexCoord2f(vertex[0], vertex[2]);		/* IGL: (nothing) -- was written for auto texture coordinate generation */
			glVertex3fv(vertex);				/* IGL: v3f(vertex); */

			vertex[0] = xloc;
			vertex[1] = SANDBOX_planeElev[x][y];
			glTexCoord2f(vertex[0], vertex[2]);		/* IGL: (nothing) -- was written for auto texture coordinate generation */
			glVertex3fv(vertex);				/* IGL: v3f(vertex); */
		}
		glEnd();						/* IGL: endtmesh(); */
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawcylinder()
{
	float	xyz1[3] = { 1, -1, 1 };
	float	xyz2[3] = { 1, 1, 1 };

	SANDBOX_MakeCylinder(xyz1, xyz2, 1, SANDBOX_CYLINDERSIDES, 0, 0);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawclosedcylinder()
{
	float	xyz1[3] = { 1, -1, 1 };
	float	xyz2[3] = { 1, 1, 1 };

	SANDBOX_MakeCylinder(xyz1, xyz2, 1, SANDBOX_CYLINDERSIDES, 1, 1);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawcube()
{
	float	cube[8][3] = {
			-1.0, 1.0, 1.0,
			-1.0, -1.0, 1.0,
			1.0, -1.0, 1.0,
			1.0, 1.0, 1.0,
			1.0, 1.0, -1.0,
			1.0, -1.0, -1.0,
			-1.0, -1.0, -1.0,
			-1.0, 1.0, -1.0
		};

	float	cube_normals[6][3] = {
			0.0, 0.0, 1.0,
			1.0, 0.0, 0.0,
			0.0, 0.0, -1.0,
			-1.0, 0.0, 0.0,
			0.0, 1.0, 0.0,
			0.0, -1.0, 0.0
		};

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[0]);					/* IGL: n3f(cube_normals[0]); */
	glVertex3fv(cube[0]);
	glVertex3fv(cube[1]);
	glVertex3fv(cube[2]);
	glVertex3fv(cube[3]);						/* IGL: v3f(cube[0]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[1]);					/* IGL: n3f(cube_normals[1]); */
	glVertex3fv(cube[4]);
	glVertex3fv(cube[5]);
	glVertex3fv(cube[2]);
	glVertex3fv(cube[3]);						/* IGL: v3f(cube[4]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[2]);					/* IGL: n3f(cube_normals[2]); */
	glVertex3fv(cube[4]);
	glVertex3fv(cube[7]);
	glVertex3fv(cube[6]);
	glVertex3fv(cube[5]);						/* IGL: v3f(cube[4]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[3]);					/* IGL: n3f(cube_normals[3]); */
	glVertex3fv(cube[7]);
	glVertex3fv(cube[6]);
	glVertex3fv(cube[1]);
	glVertex3fv(cube[0]);						/* IGL: v3f(cube[7]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[4]);					/* IGL: n3f(cube_normals[4]); */
	glVertex3fv(cube[0]);
	glVertex3fv(cube[3]);
	glVertex3fv(cube[4]);
	glVertex3fv(cube[7]);						/* IGL: v3f(cube[0]; ... */
	glEnd();							/* IGL: endpolygon(); */

	glBegin(GL_POLYGON);						/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
	glNormal3fv(cube_normals[5]);					/* IGL: n3f(cube_normals[5]); */
	glVertex3fv(cube[6]);
	glVertex3fv(cube[5]);
	glVertex3fv(cube[2]);
	glVertex3fv(cube[1]);						/* IGL: v3f(cube[6]; ... */
	glEnd();							/* IGL: endpolygon(); */
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* routine to draw a sphere from Bill Shermans wrs_shapes.c file from the FreeVR tutorial */
/*  (actually, it's a icosahedron (20-sided polygon)) */
/*****************************************************************/
void WRS_drawsphere()
{
static	double	sphere_vertices[12][3] = {
			{0.0, 0.0, 1.0},	/*  0 */
			{0.89442719099991587856, 0.0, 0.44721359549995793},	/*  1 */
			{0.27639320225002104342, 0.85065080835203993366, 0.44721359549995793},	/*  2 */
			{-0.72360679774997893378, 0.52573111211913365982, 0.44721359549995793},	/*  3 */
			{-0.72360679774997893378, -0.52573111211913365982, 0.44721359549995793},	/*  4 */
			{0.27639320225002104342, -0.85065080835203993366, 0.44721359549995793},	/*  5 */
			{0.72360679774997893378, 0.52573111211913365982, -0.44721359549995793},	/*  6 */
			{-0.27639320225002104342, 0.85065080835203993366, -0.44721359549995793},	/*  7 */
			{-0.89442719099991587856, 0.0, -0.44721359549995793},	/*  8 */
			{-0.27639320225002104342, -0.85065080835203993366, -0.44721359549995793},	/*  9 */
			{0.72360679774997893378, -0.52573111211913365982, -0.44721359549995793},	/* 10 */
			{0.0, 0.0, -1.0},	/* 11 */
		};

static	int	sphere_polygons[20][3] = {
			{2, 0, 1},	/* A */
			{3, 0, 2},	/* B */
			{4, 0, 3},	/* C */
			{5, 0, 4},	/* D */
			{1, 0, 5},	/* E */
			{1, 6, 2},	/* F */
			{7, 2, 6},	/* G */
			{2, 7, 3},	/* H */
			{8, 3, 7},	/* I */
			{3, 8, 4},	/* J */
			{9, 4, 8},	/* K */
			{4, 9, 5},	/* L */
			{10, 5, 9},	/* M */
			{6, 1, 10},	/* N */
			{5, 10, 1},	/* O */
			{6, 11, 7},	/* P */
			{7, 11, 8},	/* Q */
			{8, 11, 9},	/* R */
			{9, 11, 10},	/* S */
			{10, 11, 6},	/* T */
		};
static	double	sphere_normals[20][3] = { {-10.0, -10.0, -10.0} };	/* -10.0 indicates not set */
	int	poly;

	if (sphere_normals[0][0] == -10.0) {
		for (poly = 0; poly < 20; poly++) {
			/* a simple normal is to average the three vertices */
			sphere_normals[poly][0] = sphere_vertices[sphere_polygons[poly][0]][0];
			sphere_normals[poly][1] = sphere_vertices[sphere_polygons[poly][0]][1];
			sphere_normals[poly][2] = sphere_vertices[sphere_polygons[poly][0]][2];

			sphere_normals[poly][0] += sphere_vertices[sphere_polygons[poly][1]][0];
			sphere_normals[poly][1] += sphere_vertices[sphere_polygons[poly][1]][1];
			sphere_normals[poly][2] += sphere_vertices[sphere_polygons[poly][1]][2];

			sphere_normals[poly][0] += sphere_vertices[sphere_polygons[poly][2]][0];
			sphere_normals[poly][1] += sphere_vertices[sphere_polygons[poly][2]][1];
			sphere_normals[poly][2] += sphere_vertices[sphere_polygons[poly][2]][2];

			sphere_normals[poly][0] /= 3.0;
			sphere_normals[poly][1] /= 3.0;
			sphere_normals[poly][2] /= 3.0;
		}
	}

	for (poly = 0; poly < 20; poly++) {
		glBegin(GL_POLYGON);
		glNormal3dv(sphere_normals[poly]);
		glVertex3dv(sphere_vertices[sphere_polygons[poly][0]]);
		glVertex3dv(sphere_vertices[sphere_polygons[poly][1]]);
		glVertex3dv(sphere_vertices[sphere_polygons[poly][2]]);
		glEnd();
	}
}
