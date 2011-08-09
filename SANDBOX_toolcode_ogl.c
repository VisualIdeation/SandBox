/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* base is the XYZ coordinates of the base
   percent is the percent that should be red
*/

void SANDBOX_drawThermometer(float *base, float percent)
{
	float	vertX, vertY, vertZ;
	float	radball, redheight, whiteheight;
	float	sphereDat[4];

	if (percent < 0.0)
		percent = 0;
	if (percent > 1.0)
		percent = 1.0;

#define radther 0.02
	radball = 2 * radther;
	redheight = 0.2 * percent;
	whiteheight = 0.2 - redheight;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/* draw red sphere */
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_gred);						/* IGL: lmbind(MATERIAL, SANDBOX_gred); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.8, 0.1, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_gred); */
#endif

#if 0
	sphereDat[0] = vertX;
	sphereDat[1] = vertY;
	sphereDat[2] = vertZ;
	sphereDat[3] = radball;
	sphdraw(sphereDat);
#else
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);				/* IGL: translate(...); */
		WRS_drawsphere();								/* IGL: sphdraw(sphereDat); */
	glPopMatrix();								/* IGL: popmatrix(); */
#endif

	/* draw red cylinder */
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - radther, vertY + radball + redheight, vertZ - radther);	/* IGL: translate(...); */
		glScalef(radther, redheight, radther);						/* IGL: scale(...); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */

	/* draw white cylinder */
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);
	glEnable(GL_FRONT);							/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - radther, vertY + radball + 2 * redheight + whiteheight, vertZ - radther);	/* IGL: translate(...); */
		glScalef(radther, whiteheight, radther);					/* IGL: scale(...); */
		SANDBOX_drawcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* base is the XYZ coordinates of the base
   percent is the percent that should be blue - curently not used
*/

void SANDBOX_drawBeaker(float *base, float percent)
{
	float	vertX, vertY, vertZ;
	float	blueheight, whiteheight;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	if (percent < 0.0)
		percent = 0;
	if (percent > 1.0)
		percent = 1.0;

#define bluebeak 0.08
	blueheight = 0.16 * percent;
	whiteheight = 0.16 - blueheight;

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_water);						/* IGL: lmbind(MATERIAL, SANDBOX_water); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.10, 0.3, 0.8, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_water); -- NOTE: original has specular: 0.10, 0.3, 0.8 */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - bluebeak, vertY + blueheight, vertZ - bluebeak);		/* IGL: translate(...); */
		glScalef(bluebeak, blueheight, bluebeak);					/* IGL: scale(...); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - bluebeak, vertY + 2 * blueheight + whiteheight, vertZ - bluebeak);	/* IGL: translate(...); */
		glScalef(bluebeak, whiteheight, bluebeak);						/* IGL: scale(...); */
		SANDBOX_drawcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawSat(float *base)
{
	float	vertX, vertY, vertZ;
	float	percent;

	percent = (SANDBOX_G.frameCounter % SANDBOX_SPINRATE) / (float) SANDBOX_SPINRATE *360;

	if (percent < 0.0)
		percent = 0;
	if (percent > 360.0)
		percent = 360.0;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/*************/
	/* draw body */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ);						/* IGL: translate(...); */
		glScalef(0.05, 0.05, 0.05);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_grey);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
	glEnable(GL_FRONT);					
#else
	glColor4f(0.4, 0.4, 0.42, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ);						/* IGL: translate(...); */
		glScalef(0.1, 0.01, 0.01);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/******************************/
	/* draw rotating solar panels */
	/******************************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.15, vertY, vertZ);					/* IGL: translate(...); */
		glRotatef(percent, 1.0, 0.0, 0.0);						/* IGL: rot(percent, 'X'); */
		glScalef(0.05, 0.01, 0.1);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.15, vertY, vertZ);					/* IGL: translate(...); */
		glRotatef(360 - percent, 1.0, 0.0, 0.0);					/* IGL: rot(360-percent, 'X'); */
		glScalef(0.05, 0.01, 0.1);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.15, vertY, vertZ);					/* IGL: translate(...); */
		glRotatef(percent, 1.0, 0.0, 0.0);						/* IGL: rot(percent, 'X'); */
		glScalef(0.035, 0.015, 0.085);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.15, vertY, vertZ);					/* IGL: translate(...); */
		glRotatef(360 - percent, 1.0, 0.0, 0.0);					/* IGL: rot(360-percent, 'X'); */
		glScalef(0.035, 0.015, 0.085);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawHelo(float *base)
{
	float	vertX, vertY, vertZ;
	float	percent;

	percent = (SANDBOX_G.frameCounter % SANDBOX_SPINRATE) * 10.0;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/*************/
	/* draw body */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_grey);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.4, 0.4, 0.42, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ);						/* IGL: translate(...); */
		glScalef(0.04, 0.04, 0.08);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY - 0.01, vertZ - 0.09);				/* IGL: translate(...); */
		glScalef(0.04, 0.03, 0.01);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ + 0.12);					/* IGL: translate(...); */
		glScalef(0.015, 0.015, 0.04);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.05, vertZ);					/* IGL: translate(...); */
		glScalef(0.01, 0.02, 0.01);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/**************/
	/* draw skids */
	/**************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);				
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.03, vertY - 0.08, vertZ + 0.01);				/* IGL: translate(...); */
		glScalef(0.01, 0.005, 0.07);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.03, vertY - 0.08, vertZ + 0.01);				/* IGL: translate(...); */
		glScalef(0.01, 0.005, 0.07);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.03, vertY - 0.06, vertZ + 0.02);				/* IGL: translate(...); */
		glScalef(0.005, 0.02, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.03, vertY - 0.06, vertZ + 0.02);				/* IGL: translate(...); */
		glScalef(0.005, 0.02, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/***************/
	/* draw rotors */
	/***************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);
	glEnable(GL_FRONT);			/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.07, vertZ);					/* IGL: translate(...); */
		glRotatef(-percent, 0.0, 1.0, 0.0);						/* IGL: rot(-percent, 'Y'); */
		glScalef(0.01, 0.004, 0.12);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.07, vertZ);					/* IGL: translate(...); */
		glRotatef(-(90 + percent), 0.0, 1.0, 0.0);					/* IGL: rot(-(90+percent), 'Y'); */
		glScalef(0.01, 0.004, 0.12);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/********************/
	/* draw tail rotors */
	/********************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.016, vertY, vertZ + 0.15);				/* IGL: translate(...); */
		glRotatef(-percent, 1.0, 0.0, 0.0);						/* IGL: rot(-percent, 'X'); */
		glScalef(0.001, 0.05, 0.004);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.016, vertY, vertZ + 0.15);				/* IGL: translate(...); */
		glRotatef(-(90 + percent), 1.0, 0.0, 0.0);					/* IGL: rot(-(90+percent), 'X'); */
		glScalef(0.001, 0.05, 0.004);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawAirplane(float *base)
{
	float	vertX, vertY, vertZ;
	float	percent;

	percent = (SANDBOX_G.frameCounter % SANDBOX_SPINRATE) * 10.0;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/*************/
	/* draw body */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ); 						/* IGL: translate(...); */
		glScalef(0.03, 0.03, 0.14);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ - 0.14);					/* IGL: translate(...); */
		glRotatef(-90, 1.0, 0.0, 0.0);							/* IGL: rot(-90, 'X'); */
		glScalef(0.03, 0.08, 0.03);							/* IGL: scale(...); */
		SANDBOX_drawpyramid();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*************/
	/* draw tail */
	/*************/
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.07, vertZ + 0.11);				/* IGL: translate(...); */
		glScalef(0.002, 0.04, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.03, vertZ + 0.09);				/* IGL: translate(...); */
		glScalef(0.002, 0.08, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawpyramid();
	glPopMatrix();								/* IGL: popmatrix(); */

	/**************/
	/* draw wings */
	/**************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_grey);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.4, 0.4, 0.42, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ - 0.01);					/* IGL: translate(...); */
		glScalef(0.16, 0.004, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ - 0.03);					/* IGL: translate(...); */
		glRotatef(-90, 1.0, 0.0, 0.0);							/* IGL: rot(-90, 'X'); */
		glScalef(0.16, 0.03, 0.004);							/* IGL: scale(...); */
		SANDBOX_drawpyramid();
	glPopMatrix();								/* IGL: popmatrix(); */


	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ + 0.11);					/* IGL: translate(...); */
		glScalef(0.08, 0.004, 0.015);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ + 0.095);					/* IGL: translate(...); */
		glRotatef(-90, 1.0, 0.0, 0.0);							/* IGL: rot(-90, 'X'); */
		glScalef(0.08, 0.02, 0.004);							/* IGL: scale(...); */
		SANDBOX_drawpyramid();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*******************/
	/* draw propellers */
	/*******************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.09, vertY - 0.004 - 0.01, vertZ - 0.05);			/* IGL: translate(...); */
		glRotatef(-percent, 0.0, 0.0, 1.0);						/* IGL: rot(-percent, 'Z'); */
		glScalef(0.005, 0.05, 0.001);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.09, vertY - 0.004 - 0.01, vertZ - 0.05);			/* IGL: translate(...); */
		glRotatef(-(percent), 0.0, 0.0, 1.0);						/* IGL: rot(-percent, 'Z'); */
		glScalef(0.005, 0.05, 0.001);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.09, vertY - 0.004 - 0.01, vertZ - 0.05 + 0.02);		/* IGL: translate(...); */
		glScalef(0.01, 0.01, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX + 0.09, vertY - 0.004 - 0.01, vertZ - 0.05 + 0.02);		/* IGL: translate(...); */
		glScalef(0.01, 0.01, 0.02);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawCamera(float *base)
{
	float	vertX, vertY, vertZ;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/*************/
	/* draw body */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.05, vertZ);					/* IGL: translate(...); */
		glScalef(0.05, 0.05, 0.1);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.05 + 0.05 + 0.02, vertZ);				/* IGL: translate(...); */
		glScalef(0.04, 0.04, 0.04);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*************/
	/* draw lens */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.02, vertY - 0.04 + 0.05, vertZ - 0.04);			/* IGL: translate(...); */
		glScalef(0.06, 0.04, 0.04);							/* IGL: scale(...); */
		glRotatef(90, 0.0, 0.0, 1.0);							/* IGL: rot(90, 'Z'); */
		SANDBOX_drawcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.02, vertY - 0.039 + 0.05, vertZ - 0.039);		/* IGL: translate(...); */
		glScalef(0.061, 0.038, 0.038);							/* IGL: scale(...); */
		glRotatef(90, 0.0, 0.0, 1.0);							/* IGL: rot(90, 'Z'); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawProjector(float *base)
{
	float	vertX, vertY, vertZ;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/*************/
	/* draw body */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);	
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 0.05, vertZ);					/* IGL: translate(...); */
		glScalef(0.04, 0.075, 0.12);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */


	/**************/
	/* draw reels */
	/**************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_grey);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
	glEnable(GL_FRONT);	
#else
	glColor4f(0.4, 0.4, 0.42, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.06, vertY + 0.04 + 0.05 - 0.04, vertZ - 0.13);		/* IGL: translate(...); */
		glScalef(0.01, 0.06, 0.06);							/* IGL: scale(...); */
		glRotatef(90, 0.0, 0.0, 1.0);							/* IGL: rot(90, 'Z'); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.06, vertY + 0.04 + 0.05, vertZ + 0.08);			/* IGL: translate(...); */
		glScalef(0.01, 0.06, 0.06);							/* IGL: scale(...); */
		glRotatef(90, 0.0, 0.0, 1.0);							/* IGL: rot(90, 'Z'); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */


	/*************/
	/* draw lens */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);						
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.02, vertY + 0.05, vertZ + 0.12);				/* IGL: translate(...); */
		glScalef(0.03, 0.03, 0.06);							/* IGL: scale(...); */
		glRotatef(90, 1.0, 0.0, 0.0);							/* IGL: rot(90, 'X'); */
		SANDBOX_drawcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);							
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.02 + 0.002, vertY + 0.05 - 0.002, vertZ + 0.12 - 0.002);	/* IGL: translate(...); */
		glScalef(0.028, 0.028, 0.058);							/* IGL: scale(...); */
		glRotatef(90, 1.0, 0.0, 0.0);							/* IGL: rot(90, 'X'); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */

}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawNote(float *base)
{
	float	vertX, vertY, vertZ;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/**************/
	/* draw paper */
	/**************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);							
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY, vertZ);						/* IGL: translate(...); */
		glScalef(0.002, 0.16, 0.09);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/***************/
	/* draw pencil */
	/***************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_yellow);						/* IGL: lmbind(MATERIAL, SANDBOX_yellow); */
	glEnable(GL_FRONT);							
#else
	glColor4f(0.5, 0.5, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_yellow); -- NOTE: original has specular: 0.7, 0.7, 0.0 */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - 0.01, vertY, vertZ + .05);					/* IGL: translate(...); */
		glScalef(0.005, 0.1, 0.005);							/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawMike(float *base)
{
	float	vertX, vertY, vertZ;
	float	radball, redheight, whiteheight;
	float	sphereDat[4];

#define radther 0.02
	radball = 2 * radther;
	redheight = 0.15;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	/* draw grey sphere */
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_grey);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
	glEnable(GL_FRONT);						
#else
	glColor4f(0.4, 0.4, 0.42, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

#if 0
	sphereDat[0] = vertX;
	sphereDat[1] = vertY + redheight + radball;
	sphereDat[2] = vertZ;
	sphereDat[3] = radball;
	sphdraw(sphereDat);
#else
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);				/* IGL: translate(...); */
		WRS_drawsphere();								/* IGL: sphdraw(sphereDat); */
	glPopMatrix();								/* IGL: popmatrix(); */
#endif

	/* draw white cylinder */
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						 /* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);							
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX - radther, vertY, vertZ - radther);				/* IGL: translate(...); */
		glScalef(radther, redheight, radther);						/* IGL: scale(...); */
		SANDBOX_drawclosedcylinder();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawWind(float *base, float direction, float amount)
{
	float	vertX, vertY, vertZ;
	float	sockheight;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	if (direction < 0.0)
		direction = 0;
	if (direction > 360.0)
		direction = 360.0;

	sockheight = 0.16;

	/*************/
	/* draw pole */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);							
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + sockheight, vertZ);					/* IGL: translate(...); */
		glScalef(0.015, sockheight, 0.015);						/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*************/
	/* draw sock */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_yellow);						/* IGL: lmbind(MATERIAL, SANDBOX_yellow); */
	glEnable(GL_FRONT);							
#else
	glColor4f(0.5, 0.5, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_yellow); -- NOTE: original has specular: 0.7, 0.7, 0.0 */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + 2 * sockheight, vertZ);				/* IGL: translate(...); */
		glRotatef(90 - amount, 0.0, 0.0, 1.0);						/* IGL: rot(90-amount, 'Z'); */
		glRotatef(360 - direction, 1.0, 0.0, 0.0);					/* IGL: rot(360-direction, 'X'); */
		glScalef(.03, .15, .03);							/* IGL: scale(...); */
		SANDBOX_drawpyramid();
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawWind2(float *base, float x, float y, float z)
{
	float	vertX, vertY, vertZ;
	float	sockheight;
	float	horzAngle, vertAngle, xy;

	vertX = base[0];
	vertY = base[1];
	vertZ = base[2];

	sockheight = 0.16;

	/*************/
	/* draw pole */
	/*************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);							
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(vertX, vertY + sockheight, vertZ);					/* IGL: translate(...); */
		glScalef(0.015, sockheight, 0.015);						/* IGL: scale(...); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*************/
	/* draw sock */
	/*************/
	if ((x > -0.05) && (x < 0.05))
		if (y > 0)
			horzAngle = 90;
		else	horzAngle = -90;
	else	horzAngle = atan2(y, x) * 180 / SANDBOX_PI;

	xy = sqrt(pow(x, 2) + pow(y, 2));

	if ((z > -0.05) && (z < 0.05))
		vertAngle = 90;
	else	vertAngle = atan2(fabs(xy), z) * 180 / SANDBOX_PI;

	if (!((x > -0.05) && (x < 0.05) && (y > -0.05) && (y < 0.05) && (z > -0.05) && (z < 0.05)) && (x > -99) && (y > -99) && (z > -99)) {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_yellow);					/* IGL: lmbind(MATERIAL, SANDBOX_yellow); */
		glEnable(GL_FRONT);					
#else
		glColor4f(0.5, 0.5, 0.0, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_yellow); -- NOTE: original has specular: 0.7, 0.7, 0.0 */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(vertX, vertY + 2 * sockheight, vertZ);			/* IGL: translate(...); */
			glRotatef(90 - horzAngle, 0.0, 1.0, 0.0);				/* IGL: rot(90-horzAngle, 'Y'); */
			glRotatef(vertAngle, 1.0, 0.0, 0.0);					/* IGL: rot(vertAngle, 'X'); */
			glScalef(0.05, 0.2, 0.05);						/* IGL: scale(...); */
			SANDBOX_drawpyramid();
		glPopMatrix();							/* IGL: popmatrix(); */
	}
}

