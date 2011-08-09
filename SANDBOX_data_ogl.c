/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_beepit()
{
	/*printf("\n"); fflush(stdout); */
	/* system("sfplay SANDBOX_sounds/thunk.aif &");  */

#ifdef SOUND

	*sNote = actorMessage(A_BeginNote, "FS", *buttonactor, "thunk.aiff");
#endif

}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

int SANDBOX_inSpecificRange(float x, float y, float z, float xrange, float yrange, float zrange)
{
	float		xval, yval, zval;

	if (SANDBOX_G.rotFix == SANDBOX_NO_ROTATE) {
		xval = SANDBOX_G.wandx;
		yval = SANDBOX_G.wandy;
		zval = SANDBOX_G.wandz;
	}
	if (SANDBOX_G.rotFix == SANDBOX_ROTATE_RC) {	/* right to center */
		xval = -SANDBOX_G.wandz;
		yval = SANDBOX_G.wandy;
		zval = SANDBOX_G.wandx;
	}

	if (SANDBOX_G.rotFix == SANDBOX_ROTATE_LC) {	/* left to center */
		xval = SANDBOX_G.wandz;
		yval = SANDBOX_G.wandy;
		zval = -SANDBOX_G.wandx;
	}

	if ((xval <= x + xrange) && (xval >= x - xrange) && (zval <= z + zrange) && (zval >= z - zrange) && (yval <= y + yrange) && (yval >= y - yrange))
		return (1);
	else	return (0);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

int SANDBOX_inRange(float x, float y, float z, float range)
{
	return (SANDBOX_inSpecificRange(x, y, z, range, range, range));
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_error(char *message, char *specific)
{
	fprintf(stderr, "%s -- '%s'\n", message, specific);
	exit(1);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_lessDetail()
{
	glShadeModel(GL_FLAT);	/* IGL: shademodel(FLAT); */
	SANDBOX_G.showDetail = 0;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_moreDetail()
{
	glShadeModel(GL_SMOOTH);	/* IGL: shademodel(GOURAUD); */
	SANDBOX_G.showDetail = 1;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_saveSites()
{
register int	i, j;
	FILE	*locationFile;

	locationFile = fopen("SANDBOX_location.vals", "w");

	for (i = 0; i < SANDBOX_MAXTOOLS + 1; i++) {
		for (j = 0; j < SANDBOX_MAXSITE; j++) {
			fprintf(locationFile, "%d ", gridTool[i][j].EE);
			fprintf(locationFile, "%d ", gridTool[i][j].NN);
			fprintf(locationFile, "%d ", gridTool[i][j].elev);
		}
	}
	fclose(locationFile);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_loadSites()
{
register int	i, j;
	int	siteEE, siteNN, elevation;
	int	oldToolInHand;
	FILE	*locationFile;

	locationFile = fopen("SANDBOX_location.vals", "r");

	oldToolInHand = SANDBOX_G.toolInHand;

	for (i = 0; i < SANDBOX_MAXTOOLS + 1; i++) {
		for (j = 0; j < SANDBOX_MAXSITE; j++) {
			fscanf(locationFile, "%d ", &siteEE);
			fscanf(locationFile, "%d ", &siteNN);
			fscanf(locationFile, "%d ", &elevation);
			SANDBOX_G.toolInHand = i;

			if (siteEE >= 0)
				SANDBOX_addToolToGrid(siteEE, siteNN, elevation);
		}
	}

	fclose(locationFile);

	SANDBOX_G.toolInHand = oldToolInHand;
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* The image is 512 by 512. I am storing it as a 256 by 256 */
/* I am clipping 128 off all 4 sides to convert sat photo -> site photo */

void SANDBOX_getPlaneImage(char *filename, char *picture)
{
register int	x, y;
	FILE	*imageFile;
	float	ratio, r, theta;
	float	angleConvert;
	int	centerOrigx, centerOrigy;
	int	centerNewx, centerNewy;
	int	newx, newy;
	int	xrad, yrad;
	char	*orig;
	char	pixel, min, max;

	min = 255;
	max = 0;

	angleConvert = -10.0 * SANDBOX_PI / 180.0;

	centerOrigx = centerOrigy = 256;
	centerNewx = centerNewy = 128;

	imageFile = fopen(filename, "rb");
	if (imageFile == NULL)
		SANDBOX_error("Cannot find image file", filename);

	orig = (char *) malloc(2 * SANDBOX_G.heightOfImage * 2 * SANDBOX_G.widthOfImage);
	if (orig == NULL)
		SANDBOX_error("Not enough memory", "SANDBOX_getPlaneImage");

	/******************************/
	/* read in all the image data */
	/******************************/

	fread(orig, 1, (2 * SANDBOX_G.heightOfImage * 2 * SANDBOX_G.widthOfImage), imageFile);

	fclose(imageFile);

	/**********************************/
	/* crop and rotate the image data */
	/**********************************/

	for (y = 0; y < SANDBOX_G.heightOfImage; y++) {
		yrad = y - centerNewy;
		for (x = 0; x < SANDBOX_G.widthOfImage; x++) {
			xrad = x - centerNewx;

			r = sqrt(xrad * xrad + yrad * yrad);
			if (xrad) {
				theta = atan2(yrad, xrad);
			} else {
				if (y > centerNewy)
					theta = SANDBOX_PI * 0.5;
				else	theta = -SANDBOX_PI * 0.5;
			}
			theta += angleConvert;

			newx = (int)((r * cos(theta) + centerOrigx + 0.5));
			newy = (int)((r * sin(theta) + centerOrigy + 0.5));

			pixel = orig[newx + newy * 2 * SANDBOX_G.widthOfImage];

			picture[x + y * SANDBOX_G.widthOfImage] = pixel;

			if (pixel < min)
				min = pixel;
			if (pixel > max)
				max = pixel;
		}
	}
	free(orig);

	/***********************/
	/*  enhance image data */
	/***********************/

	ratio = 255.0 / (max - min);

	for (y = 0; y < SANDBOX_G.heightOfImage * SANDBOX_G.widthOfImage; y++)
		picture[y] = (char)(ratio * (picture[y] - min) + 0.5);
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* this needs improvements
	- a way to void out missing data
*/

void SANDBOX_getSiteData()
{
	struct elevInfo
	{
		int		totalElevation;
		int		elevationCounter;
	};

	FILE		*siteFile;
	int		id;
	unsigned long	north, east;
	int		latD, latM, latS;
	int		lonD, lonM, lonS;
	int		elevation;
	int		c, c2, c3;
	int		zeroFlag;
	int		totalElevation, elevationCounter;
	int		minElevation;
	int		averageValueElevation;
	float		ranger2;

	struct elevInfo elevationArray[SANDBOX_TMESHSIZE][SANDBOX_TMESHSIZE];

	totalElevation = 0;
	elevationCounter = 0;
	minElevation = 9999;
	SANDBOX_G.maxElevation = 0;

	for (c = 0; c < SANDBOX_TMESHSIZE; c++) {
		for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++) {
			elevationArray[c][c2].totalElevation = 0;
			elevationArray[c][c2].elevationCounter = 0;
		}
	}

	siteFile = fopen("SANDBOX_site.vals", "r");
	if (siteFile == NULL) {
		fprintf(stderr, "Cannot find site data file: '%s', exiting\n", "SANDBOX_site.vals");
		exit(1);
	}

	while (EOF != fscanf(siteFile, "%d %lu %lu %d %d %d %d %d %d %d \n",
			&id, &north, &east, &latD, &latM, &latS, &lonD, &lonM, &lonS, &elevation)) {
		printf("did one scan, id = %d, north= %d, east = %d, elevation = %d\n", id, north, east, elevation);
		siteData[id].NN = (int)((4334000 - north) / 200);
		siteData[id].EE = (int)((east - 705000) / 200);
		siteData[id].elevation = elevation;

		if (elevation > 0) {
			totalElevation += elevation;
			elevationCounter++;
			if (elevation > SANDBOX_G.maxElevation)
				SANDBOX_G.maxElevation = elevation;
			if (elevation < minElevation)
				minElevation = elevation;
			elevationArray[siteData[id].EE / (100 / SANDBOX_TMESHSIZE)][siteData[id].NN / (100 / SANDBOX_TMESHSIZE)].totalElevation += elevation;
			elevationArray[siteData[id].EE / (100 / SANDBOX_TMESHSIZE)][siteData[id].NN / (100 / SANDBOX_TMESHSIZE)].elevationCounter++;
		}
	}
	fclose(siteFile);

	/*****************************************/
	/* compute average elevation             */
	/*  the max should scale to 1, min to -1 */
	/*****************************************/

	SANDBOX_G.averageElevation = (minElevation + SANDBOX_G.maxElevation) / 2;
	averageValueElevation = totalElevation / elevationCounter;

	/*******************************************/
	/* average out the elevations at each grid */
	/*******************************************/

	for (c = 0; c < SANDBOX_TMESHSIZE; c++) {
		for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++)
			if (elevationArray[c][c2].elevationCounter > 0)
				SANDBOX_finalElev[c][c2] = elevationArray[c][c2].totalElevation / elevationArray[c][c2].elevationCounter;
			else	SANDBOX_finalElev[c][c2] = 0;
	}
	/******************************************************/
	/* interpolate / extrapolate missing elevation values */
	/******************************************************/

	/* better */
	c3 = 0;
	zeroFlag = 1;
	while (zeroFlag && (c3 < 10)) {
		for (c = 0; c < SANDBOX_TMESHSIZE; c++)
			for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++)
				if (SANDBOX_finalElev[c][c2] == 0)
					SANDBOX_finalElev[c][c2] = SANDBOX_interExtra(SANDBOX_finalElev, c, c2, SANDBOX_TMESHSIZE, 3);

		zeroFlag = 0;
		for (c = 0; c < SANDBOX_TMESHSIZE; c++)
			for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++)
				if (SANDBOX_finalElev[c][c2] == 0)
					zeroFlag++;
		c3++;
	}

	/* last chance */
	zeroFlag = 1;
	while (zeroFlag) {
		for (c = 0; c < SANDBOX_TMESHSIZE; c++)
			for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++)
				if (SANDBOX_finalElev[c][c2] == 0)
					SANDBOX_finalElev[c][c2] = SANDBOX_interExtra(SANDBOX_finalElev, c, c2, SANDBOX_TMESHSIZE, 1);

		zeroFlag = 0;
		for (c = 0; c < SANDBOX_TMESHSIZE; c++)
			for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++)
				if (SANDBOX_finalElev[c][c2] == 0)
					zeroFlag++;
	}

	ranger2 = SANDBOX_G.maxElevation - SANDBOX_G.averageElevation;

	for (c = 0; c < SANDBOX_TMESHSIZE; c++) {
		for (c2 = 0; c2 < SANDBOX_TMESHSIZE; c2++) {
			SANDBOX_planeElev[c][c2] = (SANDBOX_finalElev[c][c2] - SANDBOX_G.averageElevation) / ranger2;
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

int SANDBOX_interExtra(int theArray[SANDBOX_TMESHSIZE][SANDBOX_TMESHSIZE], int xcount, int ycount, int arraySize, int limit)
{
	int	sum, count;
	int	c1, c2;

	sum = 0;
	count = 0;

	for (c1 = xcount - 1; c1 <= xcount + 1; c1++)
		for (c2 = ycount - 1; c2 <= ycount + 1; c2++) {
			if ((c1 > 0) && (c1 < arraySize) && (c2 > 0) && (c2 < arraySize) && (theArray[c1][c2] != 0)) {
				sum += theArray[c1][c2];
				count += 1;
			}
		}

	if (count < limit)
		return (0);
	else
		return (sum / count);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_getToolData()
{
	int		i, j, count;
	FILE		*toolFile;

	for (i = 0; i < SANDBOX_MAXTOOLS + 1; i++)
		for (j = 0; j < SANDBOX_MAXSITE; j++)
			SANDBOX_legalSites[i][j] = -1;

	toolFile = fopen("SANDBOX_tool.vals", "r");
	if (toolFile == NULL)
		SANDBOX_error("Cannot find tool data file", "SANDBOX_tool.vals");

	while (EOF != fscanf(toolFile, "%d", &i)) {
		count = 0;
		fscanf(toolFile, "%d", &j);
		while (j != 0) {
			SANDBOX_legalSites[i][count++] = j;
			fscanf(toolFile, "%d", &j);
		}
	}
	fclose(toolFile);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

char *SANDBOX_getCalendarImage(char *filename, int width, int height, int flip)
{
	FILE		*imageFile;
	int		x, y, location;
	char		*memLoc;

	imageFile = fopen(filename, "r");
	if (imageFile == NULL)
		SANDBOX_error("Cannot find image file", filename);

	memLoc = (char *) malloc(width * height);

	if (memLoc == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_getCalendarImage");

	if (flip)
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++)
				memLoc[(height - 1 - y) * width + (width - 1 - x)] =
					(char) fgetc(imageFile);
	else {
		location = 0;
		for (y = 0; y < height; y++)
			for (x = 0; x < width; x++) {
				memLoc[location] = (char) fgetc(imageFile);
				location++;
			}
	}
	fclose(imageFile);
	return (memLoc);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_readAllData()
{
	FILE		*dataFile;
	int		date;
	int		counter;
	char		fileName[50];

	struct SANDBOX_onePieceInfo temp;

	for (date = 1; date <= 31; date++) {
		sprintf(fileName, "%s%02d%s", "SANDBOX_aug/", date, "-AUG-87");
		dataFile = fopen(fileName, "r");
		if (dataFile == NULL)
			SANDBOX_error("Cannot find aug data file", fileName);

		counter = 0;
		while (!feof(dataFile)) {
			fscanf(dataFile, "%d%d%f%f%f%f%f%f",
					&temp.dataId, &temp.dataTime,
					&temp.dataDry, &temp.dataWet, &temp.dataRain,
					&temp.dataU, &temp.dataV, &temp.dataW);

			SANDBOX_allMonthInfo[date][counter] = temp;

			counter++;
		}
	}
	fclose(dataFile);
}
