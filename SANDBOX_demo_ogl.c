/*****************************************************************************
home of the SANDBOX code
(c) 1993, 1994 Andrew Johnson
*****************************************************************************/

#undef	TOOGL_METHOD		/* define this to use material coloring, etc. as created by the toogl program */
#define	PO2_TEXTURES		/* define this to change to textures sized as powers of two */

#include <GL/gl.h>		/* IGL: #include <gl/{gl.h,device.h,sphere.h}> */
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "hershey.h"

#ifdef CAVE
#  include "cave_ogl.h"
#else
#  include <freevr.h>
#endif


#undef SOUND
#ifdef SOUND
#include <vssClient.h>
#endif

#define ON 1
#define OFF 0

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#define SANDBOX_DESKTOP		0
#define SANDBOX_THREEWALL	1

/* These images are 512x512 8bit images.                             */
/* To display them with ImageMagick:                                 */
/*   % display -depth 8 -size 512x512 gray:sat{0,1,2,3,4,5,6,7}.raw  */
#define SANDBOX_PLANEIMAGE	"SANDBOX_sat/sat0.raw"
#define SANDBOX_SATIMAGE1	"SANDBOX_sat/sat1.raw"
#define SANDBOX_SATIMAGE2	"SANDBOX_sat/sat2.raw"
#define SANDBOX_SATIMAGE3	"SANDBOX_sat/sat3.raw"
#define SANDBOX_SATIMAGE4	"SANDBOX_sat/sat4.raw"
#define SANDBOX_SATIMAGE5	"SANDBOX_sat/sat5.raw"
#define SANDBOX_SATIMAGE6	"SANDBOX_sat/sat6.raw"
#define SANDBOX_SATIMAGE7	"SANDBOX_sat/sat7.raw"

#define CENTERX			0.0
#define CENTERY			5.0
#define CENTERZ			0.0
#define SANDBOX_CAVESIZE	5.0

#define SANDBOX_PI		3.14159265
#define SANDBOX_GMT_ADJUST	-6

#define SANDBOX_DO		1
#define SANDBOX_DRAW		0

#define SANDBOX_NOTOOL		0
#define SANDBOX_THERMOMETERTOOL	1
#define SANDBOX_WINDTOOL	4
#define SANDBOX_BEAKERTOOL	7
#define SANDBOX_SATTOOL		2
#define SANDBOX_AIRPLANETOOL	5
#define SANDBOX_HELOTOOL	8
#define SANDBOX_CAMERATOOL	9
#define SANDBOX_NOTETOOL	6
#define SANDBOX_EMPTYTOOL	3

#define SANDBOX_MAXTOOLS	10
#define SANDBOX_MAXSITE		12

#define SANDBOX_ACTIVEGRIDSIZE	0.1

#define SANDBOX_TOOLSIZE	0.25

#define SANDBOX_TOOLMENUX	(CENTERX + SANDBOX_CAVESIZE - 1)
#define SANDBOX_TOOLMENUY	(CENTERY + 0.0)
#define SANDBOX_TOOLMENUZ	(CENTERZ - 1)

#define SANDBOX_DATEMENUX	(CENTERX - SANDBOX_CAVESIZE + 2)
#define SANDBOX_DATEMENUY	(CENTERY + 0.0)
#define SANDBOX_DATEMENUZ	(CENTERZ - 1)

#define SANDBOX_PLANEY		(CENTERY - 2.0)
#define SANDBOX_PLANESIZE	SANDBOX_CAVESIZE - 1.0

#define	SANDBOX_CYLINDERSIDES	16

#define SANDBOX_SUNZ		3.5

  /*****************************************/
 /* if I am compiling the desktop version */
/*****************************************/

#if SANDBOX_DESKTOP

#undef  SANDBOX_TOOLMENUZ
#define SANDBOX_TOOLMENUZ	(CENTERZ + 3.0)

#undef  SANDBOX_DATEMENUZ
#define SANDBOX_DATEMENUZ	(CENTERZ + 3.0)

#undef SANDBOX_TOOLMENUY
#define SANDBOX_TOOLMENUY	(CENTERY + 3.0)

#undef SANDBOX_DATEMENUY
#define SANDBOX_DATEMENUY	(CENTERY + 3.0)

#undef SANDBOX_PLANEY
#define SANDBOX_PLANEY		(CENTERY - 1.0)

#undef SANDBOX_PLANESIZE
#define SANDBOX_PLANESIZE	(SANDBOX_CAVESIZE - 3.5)

#undef  SANDBOX_CYLINDERSIDES
#define SANDBOX_CYLINDERSIDES	8

#undef SANDBOX_SUNZ
#define SANDBOX_SUNZ		2.0

#undef SANDBOX_DATEMENUX
#define SANDBOX_DATEMENUX	(CENTERX - SANDBOX_CAVESIZE + 1)

#endif

  /*********************************************/
 /* if I am compiling the SIGGRAPH 94 version */
/*********************************************/

#if SANDBOX_THREEWALL

#undef  SANDBOX_DATEMENUZ
#define SANDBOX_DATEMENUZ	(CENTERZ - 1.0)

#undef  SANDBOX_TOOLMENUZ
#define SANDBOX_TOOLMENUZ	(CENTERZ + 2.5)	/* was 3.0 */

#undef SANDBOX_SUNZ
#define SANDBOX_SUNZ		6.0

#endif

/*********************************************/

#define SANDBOX_NAVMENUZ	(CENTERZ - SANDBOX_CAVESIZE + 1)

#define SANDBOX_PLANEMENUYMAX	(CENTERY - 0.5)

#define SANDBOX_TOOLMENUXSCALE	0.05
#define SANDBOX_TOOLMENUYSCALE	1.0
#define SANDBOX_TOOLMENUZSCALE	1.0

#define SANDBOX_DATEMENUXSCALE	0.05
#define SANDBOX_DATEMENUYSCALE	1.0
#define SANDBOX_DATEMENUZSCALE	1.0
#define SANDBOX_CALENDARSCALE	1.5

/* BS: NOTE: I upped these by a factor of 10 over the Onyx for modern day Linux PC's */
#define SANDBOX_INITFRAMES	14400	/* 96 for indy, 1440 for onyx */
#define SANDBOX_TOOLANIMRATE	600	/* 20 for indy 60 for onyx */
#define SANDBOX_SPINRATE	720	/* 36 for indy 72 for onyx */
#define SANDBOX_WINDRATE	720	/* 36 for indy 72 for onyx */

#define SANDBOX_BUTTONSIZE	0.07

#define SANDBOX_TMESHSIZE	15	/* granularity of the mesh 15 */

#define SANDBOX_PLANEELEVATION	0.4

#define SANDBOX_NODATE		99
#define SANDBOX_MAXDATES	32

#define SANDBOX_GRAPHMAX	(SANDBOX_INITFRAMES / 4)

#define CYLINDER_NOCAP		0
#define CYLINDER_CAP		1

#define CYLINDER_EPSILON	0.0001

#define SANDBOX_NO_ROTATE	0
#define SANDBOX_ROTATE_RC	1
#define SANDBOX_ROTATE_LC	2


#if 0				/* BStest */
#define SANDBOX_EnableTransparency() \
	glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA); \
	if ((GL_SRC_ALPHA) == GL_ONE && ( GL_ONE_MINUS_SRC_ALPHA) == GL_ZERO) \
		glDisable(GL_BLEND); \
	else	glEnable(GL_BLEND);
#define SANDBOX_DisableTransparency() \
	 glBlendFunc(GL_ONE,  GL_ZERO); \
	if ((GL_ONE) == GL_ONE && ( GL_ZERO) == GL_ZERO) \
		glDisable(GL_BLEND); \
	else	glEnable(GL_BLEND);
#elif 1
#define SANDBOX_EnableTransparency() \
	glBlendFunc(GL_SRC_ALPHA,  GL_ONE_MINUS_SRC_ALPHA); glEnable(GL_BLEND);	/* IGL: blendfunction(BF_SA, BF_MSA); */
#define SANDBOX_DisableTransparency() \
	glBlendFunc(GL_ONE, GL_ZERO);  glDisable(GL_BLEND);	/* IGL: blendfunction(BF_ONE, BF_ZERO); */
#else
#define SANDBOX_EnableTransparency() ;
#define SANDBOX_DisableTransparency() ;
#endif
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void	SANDBOX_graph_init();
void	SANDBOX_mem_init();

void	SANDBOX_toolMenu();
void	SANDBOX_navigationMenu();
void	SANDBOX_dateTimeMenu();
void	SANDBOX_plane();
void	SANDBOX_toolsOnPlane();
void	SANDBOX_sky();
void	SANDBOX_drawWand();

void	SANDBOX_toolMenuUpdate();
void	SANDBOX_dateTimeMenuUpdate();
void	SANDBOX_planeUpdate();
void	SANDBOX_toolsOnPlaneUpdate();
void	SANDBOX_skyUpdate();

void	SANDBOX_drawThermometer(float *, float);
void	SANDBOX_drawBeaker(float *, float);
void	SANDBOX_drawSat(float *);
void	SANDBOX_drawHelo(float *);
void	SANDBOX_drawAirplane(float *);
void	SANDBOX_drawWind(float *, float, float);
void	SANDBOX_drawWind2(float *, float, float, float);
void	SANDBOX_drawCamera(float *);
void	SANDBOX_drawMike(float *);
void	SANDBOX_drawProjector(float *);

void	SANDBOX_drawcylinder();
void	SANDBOX_drawclosedcylinder();

void	SANDBOX_addToolToGrid(int, int, int);
void	SANDBOX_removeToolFromGrid(int, int);

void	SANDBOX_lessDetail();
void	SANDBOX_moreDetail();
void	SANDBOX_planeOptions(int, float, float, float);

void	SANDBOX_getPlaneImage(char *, char *);
void	SANDBOX_getSiteData();
void	SANDBOX_getToolData();
void	SANDBOX_saveSites();
void	SANDBOX_loadSites();

void	SANDBOX_selectDeselectDate(int date);
char	*SANDBOX_getCalendarImage(char *, int, int, int);
void	SANDBOX_checkDay();
void	SANDBOX_getDataRow(int, int, int, int, int);

void	SANDBOX_error(char *, char *);
void	SANDBOX_beepit();
int	SANDBOX_inRange(float, float, float, float);
int	SANDBOX_inSpecificRange(float, float, float, float, float, float);

void	SANDBOX_satOptions(int, float, float, float);
void	SANDBOX_sunmoonOptions(int, float, float, float);

int	SANDBOX_interExtra(int[SANDBOX_TMESHSIZE][SANDBOX_TMESHSIZE], int, int, int, int);

void	SANDBOX_drawText(float, float, char *, float, float, float);
void	SANDBOX_drawMenuText(char *, float, float, float);
void	SANDBOX_holidaySnaps(int, int, float *);
void	SANDBOX_texts(int, int, int, float *);

void	SANDBOX_MakeCylinder(float *pt1, float *pt2, float radius, long nsides, int cap1, int cap2);

void	SANDBOX_drawcube();
void	SANDBOX_drawpyramid();
void	SANDBOX_drawplane();
void	WRS_drawsphere();

void	SANDBOX_loadSitePic(int, int);
void	SANDBOX_readAllData();

void	SANDBOX_sound();
void	cicadaNoise(float level);

void	SANDBOX_toolOptions(int, int, int, float, float, float);

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#ifdef CAVE
void	*shared_arena;	/* for shared memory in CAVE */
#endif


/****************************************************/
#ifdef SOUND
/****************************************************/
void	EndSnailNotes(), EndHypNotes(), EndHylNotes();
int	BgnSnailNotes(), BgnHypNotes(), BgnHylNotes();


/****************************************************/
#endif
/****************************************************/

float	*buttonactor, *thermoactor, *windactor, *beakeractor;
float	*sNote, *tNote, *wNote, *bNote;


GLint	yo[4];		/* bstest */

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

  /************************/
 /* Tool and Site arrays */
/************************/

struct SANDBOX_siteInfo
{
	int	stationID;
	int	NN, EE;
	int	elevation;
};

struct SANDBOX_dailyData
{
	float	value, value2, value3;
	int	time;
};

struct SANDBOX_eachDay
{
struct	SANDBOX_dailyData	*day;
	int			ok;
};

struct SANDBOX_gridToolLoc
{
	int		NN;
	int		EE;
	int		elev;
struct SANDBOX_eachDay	*month;
	float		nowData, nowData2, nowData3;
	int		counter;
	float		lastX[SANDBOX_GRAPHMAX];
	int		displayGraph, displaySound, displayQuant;
	int		max, min;
};

struct SANDBOX_onePieceInfo
{
	int	dataId, dataTime;
	float	dataDry, dataWet, dataRain, dataU, dataV, dataW;
};

  /***********/
 /* colours */
/***********/
unsigned long	SANDBOX_backColour = 0;
float		SANDBOX_whiteColour[4] = { 1.0, 1.0, 1.0, 1.0 };

int		SANDBOX_brown = 1, SANDBOX_green = 2,	/* grid */
		SANDBOX_gred = 3,	/* thermometer, graph */
		SANDBOX_blue = 4,	/* pallets */
		SANDBOX_white = 5, SANDBOX_bwhite = 6, SANDBOX_yellow = 7, SANDBOX_water = 8, SANDBOX_gtyello = 9,	/* sun, letters */
		SANDBOX_grey = 10, SANDBOX_tyello = 11,	/* site hilite */
		SANDBOX_black = 12, SANDBOX_tred = 13,	/* site hilite */
		SANDBOX_moon = 14, SANDBOX_gblue = 15,	/* graph */
		SANDBOX_oblue = 16,	/* helicopter overlay colour */
		SANDBOX_cwhite = 17;	/* spotlight */
int		GLLIST_18 = 18,
		GLLIST_19 = 19,
		GLLIST_20 = 20, GLLIST_21 = 21, GLLIST_22 = 22, GLLIST_23 = 23, GLLIST_24 = 24;
int		GL_TEXLIST_1 = 101,	/* ground terrain texture */
		GL_TEXLIST_2 = 102,	/* calendar texture */
		GL_TEXLIST_3 = 103,	/* ground terrain overlay texture */
		GL_TEXLIST_1463 = 104,
		GL_TEXLIST_2328 = 105,
		GL_TEXLIST_3120 = 106,
		GL_TEXLIST_4338 = 107, GL_TEXLIST_6468 = 108, GL_TEXLIST_6911 = 109, GL_TEXLIST_8639 = 110;

int		SANDBOX_legalSites[SANDBOX_MAXTOOLS + 1][SANDBOX_MAXSITE];

struct SANDBOX_siteInfo siteData[1000];

char		*SANDBOX_pics[10000];

struct SANDBOX_gridToolLoc gridTool[SANDBOX_MAXTOOLS + 1][SANDBOX_MAXSITE];

struct SANDBOX_onePieceInfo SANDBOX_allMonthInfo[32][500];

  /*****************************************/
 /* globals to control the plane location */
/*****************************************/

int		SANDBOX_finalElev[SANDBOX_TMESHSIZE][SANDBOX_TMESHSIZE];
float		SANDBOX_planeElev[SANDBOX_TMESHSIZE][SANDBOX_TMESHSIZE];

  /******************************************/
 /* globals to control the texture mapping */
/******************************************/

#if 0	/* BS -- this isn't even used! */
	/* OGLXXX TX_WRAP not supported: see glTexParameterfv man page. */
float		SANDBOX_texprops[] = { GL_TEXTURE_MIN_FILTER, GL_LINEAR, GL_TEXTURE_MAG_FILTER, GL_LINEAR, XXX_TX_WRAP, GL_CLAMP, }; /* IGL: { TX_MINFILTER, TX_BILINEAR, TX_MAGFILTER, TX_BILINEAR, TX_WRAP, TX_CLAMP, TX_NULL } */
#endif
float		SANDBOX_tevprops[] = { GL_MODULATE, };	/* IGL: { TV_MODULATE, TV_NULL } */

/* OGL: is there a problem with GL_BLEND? */
/* OGL: env_color: see glTexEnv man page */
float		SANDBOX_tevprops2[] = { GL_BLEND, GL_TEXTURE_ENV_COLOR, 0, 0, 0, 0.0, };	/* IGL: { TV_BLEND,  TV_COLOR,  0, 0, 0, 0.0, TV_NULL } */

int		pausePic;

	/***********/
	/* globals */
	/***********/
struct SANDBOXglobals
{
	float		wandx, wandy, wandz;	/* wand information */
	float		vectx, vecty, vectz;
	int		forceMoveUp, forceSelectUp, forceOptionUp;
	int		toolInHand;

	float		headX, headY, headZ;	/* head information */
	int		showGrid;

	float		planeScale;	/* plane information */
	float		planeX, planeY, planeZ;
	int		averageElevation;
	int		maxElevation;
	int		planeMenu;
	float		planeMenuX, planeMenuY, planeMenuZ;
	char		*picture;
	char		*picture0, *picture1, *picture2, *picture3, *picture4, *picture5, *picture6, *picture7;

	int		satInSky;	/* sky information */
	int		satteliteOptions, sunOptions;
	int		heloInSky;
	int		smoonSelected;
	float		sunMenuDat[3];

	float		timeOfDay;	/* time information */
	unsigned	frameCounter, timeCounter;
	int		framesPerDay;
	int		currentDay;
	int		dateSelected;
	int		thaHour, thaMinute, time24;
	int		activeDays[SANDBOX_MAXDATES];
	clock_t		startTime;	/*cicada noise */

	int		moveButton,	/* button information */
			selectButton, optionButton;

	int		showDetail;	/* general information */
	int		toolAnim;
	char		*calendar;
	char		*overlay;
	int		heightOfImage, widthOfImage;
	int		flip;
	int		rotFix;

	int		instrumentMenu;
	float		instrumentMenuX, instrumentMenuY, instrumentMenuZ;
	int		instrumentMenuTool, instrumentMenuSite;

	int		spoton;
} SANDBOX_G;

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

#include "SANDBOX_objectcode_ogl.c"
#include "SANDBOX_toolcode_ogl.c"
#include "SANDBOX_data_ogl.c"

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void demo_init()
{
	SANDBOX_mem_init();
	SANDBOX_graph_init();	/* must be in this order or crash */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
void SANDBOX_graph_init()
{
	float	brownmt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.5, 0.3, 0.2, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.5, 0.3, 0.2,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	blackmt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.0, 0.0, 0.0, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.0, 0.0, 0.0,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	greentransmt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.1, 0.2, 0.0, 1.0,
		GL_DIFFUSE, 0.4, 0.6, 0.6, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.1, 0.2, 0.0,   */
	/* IGL: DIFFUSE, 0.4, 0.6, 0.6,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	yellowtransmt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.6, 0.6, 0.0, 1.0,
		GL_DIFFUSE, 0.5, 0.5, 0.0, 1.0, GL_SPECULAR, 0.7, 0.7, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.6, 0.6, 0.0,   */
	/* IGL: DIFFUSE, 0.5, 0.5, 0.0,     SPECULAR, 0.7, 0.7, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	redmt[] = {
		GL_EMISSION, 1.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.8, 0.1, 0.1, 1.0, GL_SPECULAR, 0.95, 0.2, 0.2, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 1.0, 0.0, 0.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.8, 0.1, 0.1,     SPECULAR, 0.95, 0.2, 0.2, */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	bluemt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.1, 0.1, 0.1, 1.0,
		GL_DIFFUSE, 0.15, 0.3, 0.7, 1.0, GL_SPECULAR, 0.3, 0.3, 0.7, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.1, 0.1, 0.1,   */
	/* IGL: DIFFUSE, 0.15, 0.3, 0.7,    SPECULAR, 0.3, 0.3, 0.7,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	blueovermt[] = {
		GL_EMISSION, 0.3, 0.5, 1.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.15, 0.3, 0.7, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.3, 0.5, 1.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.15, 0.3, 0.7,    SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	blueglowmt[] = {
		GL_EMISSION, 0.5, 0.5, 1.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.15, 0.3, 0.7, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.5, 0.5, 1.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.15, 0.3, 0.7,    SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	sunmt[] = {
		GL_EMISSION, 0.9, 0.9, 0.0, 1.0, GL_AMBIENT, 0.9, 0.9, 0.0, 1.0,
		GL_DIFFUSE, 0.5, 0.5, 0.0, 1.0, GL_SPECULAR, 0.25, 0.25, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.9, 0.9, 0.0,    AMBIENT, 0.9, 0.9, 0.0,   */
	/* IGL: DIFFUSE, 0.5, 0.5, 0.0,     SPECULAR, 0.25, 0.25, 0.0, */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	watermt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.10, 0.3, 0.8, 1.0, GL_SPECULAR, 0.10, 0.3, 0.8, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.10, 0.3, 0.8,    SPECULAR, 0.10, 0.3, 0.8, */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	whitemt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.15, 0.15, 0.15, 1.0,
		GL_DIFFUSE, 0.9, 0.9, 0.9, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.15, 0.15, 0.15, */
	/* IGL: DIFFUSE, 0.9, 0.9, 0.9,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	bwhitemt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.8, 0.8, 0.8, 1.0,
		GL_DIFFUSE, 0.9, 0.9, 0.9, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.8, 0.8, 0.8,   */
	/* IGL: DIFFUSE, 0.9, 0.9, 0.9,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	moonmt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
		GL_DIFFUSE, 0.8, 0.8, 0.8, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.0, 0.0, 0.0,   */
	/* IGL: DIFFUSE, 0.8, 0.8, 0.8,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	transwhitemt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 0.5, GL_AMBIENT, 0.9, 0.9, 0.0, 0.5,
		GL_DIFFUSE, 0.8, 0.8, 0.0, 0.5, GL_SPECULAR, 0.25, 0.25, 0.0, 0.5,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.9, 0.9, 0.0,   */
	/* IGL: DIFFUSE, 0.8, 0.8, 0.0,     SPECULAR, 0.25, 0.25, 0.0, */
	/* IGL: SHININESS, 0.0,             ALPHA, 0.5,               */
	/* IGL: LMNULL                                                */

	float	clearwhitemt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 0.6, GL_AMBIENT, 0.5, 0.5, 0.5, 0.6,
		GL_DIFFUSE, 0.0, 0.0, 0.0, 0.6, GL_SPECULAR, 0.0, 0.0, 0.0, 0.6,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.5, 0.5, 0.5,   */
	/* IGL: DIFFUSE, 0.0, 0.0, 0.0,     SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 0.6,               */
	/* IGL: LMNULL                                                */

	float	transredmt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 0.6, GL_AMBIENT, 0.9, 0.2, 0.0, 0.6,
		GL_DIFFUSE, 0.8, 0.1, 0.0, 0.6, GL_SPECULAR, 0.25, 0.0, 0.0, 0.6,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.9, 0.2, 0.0,   */
	/* IGL: DIFFUSE, 0.8, 0.1, 0.0,     SPECULAR, 0.25, 0.0, 0.0, */
	/* IGL: SHININESS, 0.0,             ALPHA, 0.6,               */
	/* IGL: LMNULL                                                */

	float	greymt[] = {
		GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.2, 0.2, 0.2, 1.0,
		GL_DIFFUSE, 0.4, 0.4, 0.42, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
		GL_SHININESS, 0.0,
	};
	/* IGL: EMISSION, 0.0, 0.0, 0.0,    AMBIENT, 0.2, 0.2, 0.2,   */
	/* IGL: DIFFUSE, 0.4, 0.4, 0.42,    SPECULAR, 0.0, 0.0, 0.0,  */
	/* IGL: SHININESS, 0.0,             ALPHA, 1.0,               */
	/* IGL: LMNULL                                                */

	float	light[] = {
		GL_DIFFUSE, 1, 1, 1, 1.0,
		GL_POSITION, 0, 6, 0, 1,
	};
	/* IGL: LCOLOR, 1, 1, 1,       */
	/* IGL: POSITION, 0, 6, 0, 1,  */
	/* IGL: LMNULL                 */

	float	lm[] = {	/* light-model */
		GL_AMBIENT, 1, 1, 1, 1.0
	};
	/* IGL: AMBIENT, 1, 1, 1, */
	/* IGL: LOCALVIEWER,  1,  */	/* OGL: light model parameters need to be moved into a glLightModelf call. */
	/* IGL: LMNULL            */

	GLfloat	light0_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat	light0_diffuse[] = { 0.4, 0.4, 0.4, 1.0 };
	GLfloat	light1_ambient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat	light1_diffuse[] = { 0.6, 0.6, 0.6, 1.0 };

	int	border = 0;

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_brown, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *brownmt);		 /* IGL: lmdef(DEFMATERIAL, SANDBOX_brown, 21, brownmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_green, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *greentransmt);		 /* IGL: lmdef(DEFMATERIAL, SANDBOX_green, 21, greentransmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_gred, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *redmt);			/* IGL: lmdef(DEFMATERIAL, SANDBOX_gred, 21, redmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_blue, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *bluemt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_blue, 21, bluemt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_white, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *whitemt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_white, 21, whitemt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_bwhite, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *bwhitemt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_bwhite, 21, bwhitemt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_yellow, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *yellowtransmt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_yellow, 21, yellowtransmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_water, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *watermt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_water, 21, watermt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_gtyello, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *sunmt);			/* IGL: lmdef(DEFMATERIAL, SANDBOX_gtyello, 21, sunmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_grey, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *greymt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_grey, 21, greymt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_tyello, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *transwhitemt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_tyello, 21, transwhitemt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_black, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *blackmt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_black, 21, blackmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_tred, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *transredmt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_tred, 21, transredmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_moon, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *moonmt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_moon, 21, moonmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_gblue, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *blueglowmt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_gblue, 21, blueglowmt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_oblue, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *blueovermt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_oblue, 21, blueovermt); */
	glEndList();

	/* NOTE: at point of glCallList, TOOGL recommends the use of GL_FRONT */
	glNewList(SANDBOX_cwhite, GL_COMPILE);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *clearwhitemt);		/* IGL: lmdef(DEFMATERIAL, SANDBOX_cwhite, 21, clearwhitemt); */
	glEndList();


	/* TODO: BS: I'm not sure that these lighting commands have an effect as things are currently written */
	glNewList(GLLIST_18, GL_COMPILE);
	glLightf(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, *light);			/* IGL: lmdef(DEFLIGHT, 1, 0, light); *//* for OGL: changed glMaterialf to glLightf */
	glEndList();

	/* TODO: BS: I'm not sure that these lighting commands have an effect as things are currently written */
	glNewList(GLLIST_19, GL_COMPILE);
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, *lm);				/* IGL: lmdef(DEFLMODEL, 1, 0, lm); *//* for OGL: changed glMaterialf to glLightModelf */
	glEndList();
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &lm[1]);

	SANDBOX_EnableTransparency();

#ifdef TOOGL_METHOD
	glNewList(1, GL_COMPILE);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, SANDBOX_G.heightOfImage, SANDBOX_G.widthOfImage, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.picture);	/* IGL: texdef2d(1,1,SANDBOX_G.heightOfImage,SANDBOX_G.widthOfImage, (unsigned long *) SANDBOX_G.picture,7,SANDBOX_texprops); */
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_1);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, SANDBOX_G.heightOfImage, SANDBOX_G.widthOfImage, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.picture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	/* NOTE: this probably isn't having an effect at the moment */
	glNewList(GLLIST_20, GL_COMPILE);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, SANDBOX_tevprops);		/* IGL: tevdef(1,2,SANDBOX_tevprops); */
	glEndList();
#else
	/* TODO: this */
	glNewList(GLLIST_20, GL_COMPILE);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, SANDBOX_tevprops);
	glEndList();
#endif
#ifdef TOOGL_METHOD
	/* NOTE: this probably isn't having an effect at the moment */
	glNewList(GLLIST_21, GL_COMPILE);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, SANDBOX_tevprops2);		/* IGL: tevdef(2,7,SANDBOX_tevprops2); */
	glEndList();
#else
	/* TODO: this */
	glNewList(GLLIST_21, GL_COMPILE);
	glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, SANDBOX_tevprops2);
	glEndList();
#endif

#ifdef TOOGL_METHOD		/* in this case a modified toogl method for experimentation */
	glGenTextures(4, yo);
	glBindTexture(GL_TEXTURE_2D, 1);
	glNewList(GLLIST_22, GL_COMPILE);
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_2);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.calendar); /* IGL: texdef2d(2,1,256,256,(unsigned long *)SANDBOX_G.calendar,7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 224, 200, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.calendar); /* IGL: texdef2d(2,1,224,200,(unsigned long *)SANDBOX_G.calendar,7,SANDBOX_texprops); */
#  endif
	glEndList();
	glBindTexture(GL_TEXTURE_2D, 0);
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_2);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.calendar);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 224, 200, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.calendar);
#endif
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(GLLIST_23, GL_COMPILE);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.overlay); /* IGL: texdef2d(3,1,256,256,(unsigned long *)SANDBOX_G.overlay,7,SANDBOX_texprops); */
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_3);
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (unsigned long *)SANDBOX_G.overlay);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif

#ifdef TOOGL_METHOD
	glNewList(10 + 14 * 100 + 63, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_1463);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(10 + 23 * 100 + 28, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[23 * 100 + 28]); /* IGL: texdef2d(10+23*100+28,1,384,256,(unsigned long *)SANDBOX_pics[23*100+28],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_2328);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[23 * 100 + 28]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(10 + 31 * 100 + 20, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[31 * 100 + 20]); /* IGL: texdef2d(10+31*100+20,1,384,256,(unsigned long *)SANDBOX_pics[31*100+20],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_3120);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[31 * 100 + 20]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(10 + 43 * 100 + 38, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[43 * 100 + 38]); /* IGL: texdef2d(10+43*100+38,1,384,256,(unsigned long *)SANDBOX_pics[43*100+38],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_4338);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[43 * 100 + 38]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(10 + 64 * 100 + 68, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[64 * 100 + 68]); /* IGL: texdef2d(10+64*100+68,1,384,256,(unsigned long *)SANDBOX_pics[64*100+68],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_6468);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[64 * 100 + 68]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(10 + 69 * 100 + 11, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[69 * 100 + 11]); /* IGL: texdef2d(10+69*100+11,1,384,256,(unsigned long *)SANDBOX_pics[69*100+11],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_6911);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[69 * 100 + 11]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif
#ifdef TOOGL_METHOD
	glNewList(10 + 86 * 100 + 39, GL_COMPILE);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[86 * 100 + 39]); /* IGL: texdef2d(10+86*100+39,1,384,256,(unsigned long *)SANDBOX_pics[86*100+39],7,SANDBOX_texprops); */
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glEndList();
#else
	glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_8639);
#  ifdef PO2_TEXTURES
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[86 * 100 + 39]);
#  else
	glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[14 * 100 + 63]); /* IGL: texdef2d(10+14*100+63,1,384,256,(unsigned long *)SANDBOX_pics[14*100+63],7,SANDBOX_texprops); */
#  endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);	/* NOTE: this is probably unnecessary since another bind is coming */
#endif

	SANDBOX_moreDetail();

	 /*DELETED*/;		/* IGL: subpixel(TRUE); */

#ifdef TOOGL_METHOD
	glCallList(1);
	glEnable(GL_LIGHT1); /* this one is the 3D troublemaker *//* IGL: lmbind(LIGHT1,  1); */
#endif
#ifdef TOOGL_METHOD		/* BS: perhaps GL_LIGHTING should be the flag to enable/disable */
	glCallList(1);
	glEnable(GL_LIGHTING);			 			/* IGL: lmbind(LMODEL, 1); */
#elif 1
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	glEnable(GL_LIGHT1);			/* BS: the light1 parameters weren't set before, but now I'm using them */

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	/* eg. glColor4f(0.5, 0.3, 0.2, 0.6); */
#endif
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_mem_init()
{
	char	*temp;
register int	r, i, j, k;
	FILE	*datFile;
	char	garbage[20];

	/************************************/
	/* start out with detail turned off */
	/************************************/

fprintf(stderr, "13 ");

	SANDBOX_G.toolAnim = SANDBOX_TOOLANIMRATE;

	datFile = fopen("SANDBOX_param.vals", "r");
	if (datFile == NULL) {
		fprintf(stderr, "Can't find parameter file: '%s', exiting\n", "SANDBOX_param.vals");
		exit(1);
	}

	SANDBOX_G.showGrid = 0;
	SANDBOX_G.planeMenu = 0;
	SANDBOX_G.satInSky = 0;
	SANDBOX_G.heloInSky = 0;

	fscanf(datFile, "%d %s\n", &SANDBOX_G.showGrid, garbage);
	fscanf(datFile, "%d %s\n", &SANDBOX_G.planeMenu, garbage);
	fscanf(datFile, "%d %s\n", &SANDBOX_G.satInSky, garbage);
	fscanf(datFile, "%d %s\n", &SANDBOX_G.heloInSky, garbage);

	/*************************/
	/* start out at frame #0 */
	/*************************/

	SANDBOX_G.frameCounter = 0;
	SANDBOX_G.timeCounter = 0;

	/***********************************/
	/* user starts out holding nothing */
	/***********************************/

	SANDBOX_G.toolInHand = SANDBOX_NOTOOL;
	SANDBOX_G.sunOptions = 0;
	SANDBOX_G.satteliteOptions = 0;
	SANDBOX_G.smoonSelected = 0;

	SANDBOX_G.spoton = 0;

	fscanf(datFile, "%d %s\n", &SANDBOX_G.sunOptions, garbage);
	fscanf(datFile, "%d %s\n", &SANDBOX_G.satteliteOptions, garbage);
	fscanf(datFile, "%d %s\n", &SANDBOX_G.smoonSelected, garbage);
	fclose(datFile);

	/*********************************************/
	/* user starts out with no tools on the grid */
	/*********************************************/

	SANDBOX_G.planeScale = SANDBOX_PLANESIZE;
	SANDBOX_G.planeX = CENTERX;
	SANDBOX_G.planeY = SANDBOX_PLANEY;
	SANDBOX_G.planeZ = CENTERZ;

fprintf(stderr, "12 ");

	for (i = 0; i < SANDBOX_MAXTOOLS + 1; i++) {
		for (j = 0; j < SANDBOX_MAXSITE; j++) {
			gridTool[i][j].EE = -1;
			gridTool[i][j].NN = -1;
			gridTool[i][j].elev = -1;
			gridTool[i][j].counter = 0;
			gridTool[i][j].nowData = 0;
			gridTool[i][j].nowData2 = 0;
			gridTool[i][j].nowData3 = 0;

			gridTool[i][j].month = (struct SANDBOX_eachDay *)
				calloc(32, sizeof(struct SANDBOX_eachDay));

			if (gridTool[i][j].month == NULL)
				SANDBOX_error("Cannot allocate memory for data", "SANDBOX_mem_init 1");

			for (r = 0; r < 32; r++) {
				gridTool[i][j].month[r].day = (struct SANDBOX_dailyData *)
					calloc(50, sizeof(struct SANDBOX_dailyData));

				if (gridTool[i][j].month[r].day == NULL)
					SANDBOX_error("Could not allocate memory for data.", "SANDBOX_mem_init 2");

				gridTool[i][j].month[r].ok = 0;
			}

			for (k = 0; k < SANDBOX_GRAPHMAX; k++)
				gridTool[i][j].lastX[k] = -99;

			gridTool[i][j].displayGraph = -1;
			gridTool[i][j].displaySound = -1;
			gridTool[i][j].displayQuant = -1;
		}
	}

fprintf(stderr, "11 ");

	/***********************************/
	/* no dates are initially selected */
	/***********************************/

	for (i = 0; i < SANDBOX_MAXDATES; i++)
		SANDBOX_G.activeDays[i] = SANDBOX_NODATE;

	SANDBOX_G.currentDay = SANDBOX_NODATE;

	SANDBOX_G.framesPerDay = SANDBOX_INITFRAMES;

	SANDBOX_selectDeselectDate(12);
	SANDBOX_selectDeselectDate(13);
	SANDBOX_selectDeselectDate(14);

fprintf(stderr, "10 ");

	/***************************/
	/* load in data from files */
	/***************************/

	SANDBOX_getSiteData();

fprintf(stderr, "9 ");

	SANDBOX_readAllData();

fprintf(stderr, "8 ");

	/*********************************************************/
	/* load up the legal sites for all the tools on the grid */
	/*********************************************************/

fprintf(stderr, "7 ");

	SANDBOX_getToolData();

fprintf(stderr, "6 ");

	/**********************/
	/* set up site photos */
	/**********************/

	for (i = 0; i <= 9999; i++)
		SANDBOX_pics[i] = NULL;

	SANDBOX_loadSitePic(14, 63);
	SANDBOX_loadSitePic(23, 28);
	SANDBOX_loadSitePic(31, 20);
	SANDBOX_loadSitePic(43, 38);
	SANDBOX_loadSitePic(64, 68);
	SANDBOX_loadSitePic(69, 11);
	SANDBOX_loadSitePic(86, 39);

fprintf(stderr, "5 ");

	/*********************/
	/* set up sat images */
	/*********************/

	SANDBOX_G.heightOfImage = 256;
	SANDBOX_G.widthOfImage = 256;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 3");
	else	SANDBOX_G.picture0 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 4");
	else	SANDBOX_G.picture1 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 5");
	else	SANDBOX_G.picture2 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 6");
	else	SANDBOX_G.picture3 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 7");
	else	SANDBOX_G.picture4 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 8");
	else	SANDBOX_G.picture5 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 9");
	else	SANDBOX_G.picture6 = temp;

	temp = (char *)malloc(SANDBOX_G.widthOfImage * SANDBOX_G.heightOfImage);
	if (temp == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_mem_init 10");
	else	SANDBOX_G.picture7 = temp;

fprintf(stderr, "4 ");

	SANDBOX_getPlaneImage(SANDBOX_PLANEIMAGE, SANDBOX_G.picture0);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE1, SANDBOX_G.picture1);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE2, SANDBOX_G.picture2);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE3, SANDBOX_G.picture3);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE4, SANDBOX_G.picture4);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE5, SANDBOX_G.picture5);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE6, SANDBOX_G.picture6);
	SANDBOX_getPlaneImage(SANDBOX_SATIMAGE7, SANDBOX_G.picture7);

	SANDBOX_G.picture = SANDBOX_G.picture0;

fprintf(stderr, "3 ");

	/*******************/
	/* set up calendar */
	/*******************/

#ifdef PO2_TEXTURES
	SANDBOX_G.calendar = SANDBOX_getCalendarImage("pics/SANDBOX_aug87_pot.raw", 256, 256, 1);
#else
	SANDBOX_G.calendar = SANDBOX_getCalendarImage("pics/SANDBOX_aug87.raw", 224, 200, 1);
#endif

fprintf(stderr, "2 ");

	/************************/
	/* set up plane overlay */
	/************************/

	SANDBOX_G.overlay = SANDBOX_getCalendarImage("pics/SANDBOX_moisture.raw", 256, 256, 0);

fprintf(stderr, "1 ");

	/***********************/
	/* set up default font */
	/***********************/

	hfont("times.r");

	/******************/
	/* set it running */
	/******************/

	pausePic = 0;
	SANDBOX_G.flip = 0;

	SANDBOX_G.startTime = 0;
	SANDBOX_G.rotFix = SANDBOX_NO_ROTATE;

	/* SANDBOX_G.toolInHand = SANDBOX_BEAKERTOOL; */


	/* load in the initial instruments at sites */
	/* SANDBOX_loadSites(); */

	fprintf(stderr, "<boom>\n");
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void demo_update()
{
	float	convertBlue, convertRed;
	clock_t	start, finish;

	start = clock();

#if 0 /* BS: for debugging */
printf("SANDBOX_G.timeCounter = %d, frameCounter = %d\n", SANDBOX_G.timeCounter, SANDBOX_G.frameCounter);
#endif

	/************************************/
	/* get locations from the trackers */
	/***********************************/
#ifdef CAVE
#  if 0				/* old style */
	CAVEGetHead(SANDBOX_G.headX, SANDBOX_G.headY, SANDBOX_G.headZ);
	CAVEGetWand(SANDBOX_G.wandx, SANDBOX_G.wandy, SANDBOX_G.wandz);
	CAVEGetWandFront(SANDBOX_G.vectx, SANDBOX_G.vecty, SANDBOX_G.vectz);
#  else
	CAVEGetPosition(CAVE_HEAD, &SANDBOX_G.headX);
	CAVEGetPosition(CAVE_WAND, &SANDBOX_G.wandx);
	CAVEGetVector(CAVE_WAND_FRONT, &SANDBOX_G.vectx);
#  endif

	SANDBOX_G.wandx += SANDBOX_G.vectx;
	SANDBOX_G.wandy += SANDBOX_G.vecty;
	SANDBOX_G.wandz += SANDBOX_G.vectz;

#else /* FreeVR version */
	vrPoint		head;
	vrPoint		wand;
	vrVector	wand_fore;

	vrPointGetRWFrom6sensor(&head, 0);
	vrPointGetRWFrom6sensor(&wand, 1);
	vrVectorGetRWFrom6sensorDir(&wand_fore, 1, VRDIR_FORE);

	SANDBOX_G.headX = head.v[VR_X];
	SANDBOX_G.headY = head.v[VR_Y];
	SANDBOX_G.headZ = head.v[VR_Z];

	SANDBOX_G.wandx = wand.v[VR_X] + wand_fore.v[VR_X];
	SANDBOX_G.wandy = wand.v[VR_Y] + wand_fore.v[VR_Y];
	SANDBOX_G.wandz = wand.v[VR_Z] + wand_fore.v[VR_Z];

#endif

	/*********************************/
	/* get the values of the buttons */
	/*********************************/

#ifdef CAVE
	SANDBOX_G.moveButton = CAVEBUTTON1;
	SANDBOX_G.selectButton = CAVEBUTTON2;
	SANDBOX_G.optionButton = CAVEBUTTON3;
#else
	SANDBOX_G.moveButton = vrGet2switchValue(1);
	SANDBOX_G.selectButton = vrGet2switchValue(2);
	SANDBOX_G.optionButton = vrGet2switchValue(3);
#endif

	/******************************/
	/* check for released buttons */
	/******************************/

	if (SANDBOX_G.moveButton == OFF)
		SANDBOX_G.forceMoveUp = 0;
	if (SANDBOX_G.selectButton == OFF)
		SANDBOX_G.forceSelectUp = 0;
	if (SANDBOX_G.optionButton == OFF)
		SANDBOX_G.forceOptionUp = 0;

	/***************************************/
	/* decide whether to show the sun/moon */
	/***************************************/

	SANDBOX_checkDay();

	if (SANDBOX_G.currentDay == SANDBOX_NODATE)
		SANDBOX_G.dateSelected = 0;
	else
		SANDBOX_G.dateSelected = 1;

	if (!pausePic) {
		SANDBOX_G.timeOfDay = (SANDBOX_G.timeCounter % SANDBOX_G.framesPerDay) / (float)SANDBOX_G.framesPerDay;
		SANDBOX_G.thaHour = (int)(SANDBOX_G.timeOfDay * 24);
		SANDBOX_G.thaMinute = (int)((SANDBOX_G.timeOfDay * 24 - SANDBOX_G.thaHour) * 60);
		SANDBOX_G.time24 = (int)(SANDBOX_G.thaHour * 100) + SANDBOX_G.thaMinute;

		if (SANDBOX_G.dateSelected) {
			convertBlue = 2 * 255 * fabs(SANDBOX_G.timeOfDay - 0.5);
			convertRed = 4 * 40 * fabs(fabs(SANDBOX_G.timeOfDay - 0.5) - 0.25);
			SANDBOX_backColour =
				(long)(255 - convertBlue) * 256 * 256 + (40 - convertRed);
		} else {
			SANDBOX_backColour = 0;
		}

		if (SANDBOX_DESKTOP || SANDBOX_THREEWALL)
			SANDBOX_G.rotFix = SANDBOX_ROTATE_RC;
		SANDBOX_toolMenuUpdate();
		SANDBOX_G.rotFix = SANDBOX_NO_ROTATE;

		if (SANDBOX_DESKTOP)
			SANDBOX_G.rotFix = SANDBOX_ROTATE_LC;
		SANDBOX_dateTimeMenuUpdate();
		SANDBOX_G.rotFix = SANDBOX_NO_ROTATE;

		SANDBOX_planeUpdate();
		SANDBOX_toolsOnPlaneUpdate();
		SANDBOX_skyUpdate();

#ifdef SOUND
		SANDBOX_sound();
#endif


		if (!SANDBOX_G.smoonSelected && !SANDBOX_G.sunOptions && SANDBOX_G.dateSelected)
			SANDBOX_G.timeCounter++;

		SANDBOX_G.frameCounter++;
	}

	/*************************************/
	/* press the D key for screen dump.  */
	/*************************************/

#ifdef CAVE
	if (CAVEgetbutton(CAVE_DKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		char	command[128];
	static	int	count = 0;
		long	screenSizeX, screenSizeY, screenLocX, screenLocY;

		 /*DELETED*/;	/* IGL: getsize(&screenSizeX,&screenSizeY); */
		 /*DELETED*/;	/* IGL: getorigin(&screenLocX, &screenLocY); */

		sprintf(command, "scrsave screen%03d %ld %ld %ld %ld", count, screenLocX, screenLocX + screenSizeX, screenLocY, screenLocY + screenSizeY);

		count++;
		system(command);
		SANDBOX_beepit();
	}

	/******************************/
	/* press the Y key to pause.  */
	/******************************/
#if 0				/* NOTE: commented out in original source code -- I prefer "#if 0" */
#ifdef CAVE
	if (CAVEgetbutton(CAVE_YKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		pausePic = !pausePic;
		SANDBOX_beepit();
	}
#endif

	/*************************************/
	/* press the M key for more detail.  */
	/*************************************/

#ifdef CAVE
	if (CAVEgetbutton(CAVE_MKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		SANDBOX_moreDetail();
		SANDBOX_beepit();
	}

	/***********************************/
	/* press the N key for no detail.  */
	/***********************************/

#ifdef CAVE
	if (CAVEgetbutton(CAVE_NKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		SANDBOX_lessDetail();
		SANDBOX_beepit();
	}

	/*************************************/
	/* press the I key to load in sites  */
	/*************************************/

#ifdef CAVE
	if (CAVEgetbutton(CAVE_IKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		SANDBOX_loadSites();
		SANDBOX_beepit();
	}

	/*********************************/
	/* press the O key to save sites */
	/*********************************/

#ifdef CAVE
	if (CAVEgetbutton(CAVE_OKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		SANDBOX_saveSites();
		SANDBOX_beepit();
	}

	/**********************************/
	/* press the Y key to fake a grab */
	/**********************************/
#if 0				/* NOTE: commented out in original source code -- I prefer "#if 0" */
#ifdef CAVE
	if (CAVEgetbutton(CAVE_YKEY))
#else /* FreeVR */
	if (0)			/* TODO: implement additional button inputs for FreeVR */
#endif
	{
		SANDBOX_G.toolInHand = SANDBOX_CAMERATOOL;
		SANDBOX_beepit();
	}
#endif


	finish = clock();
#if 0
fprintf(stderr, "compute time: %8u", (finish-start)/10000);
#endif
}


/*****************************************************************/
/* Bill's test code */
/*****************************************************************/

/**********************************************************************/
/* Make a checker pattern for use as a texture map.  Size is xsize by */
/*   ysize, and the color scheme is either GL-color vs. black, or     */
/*   GL-color vs. transparent hole.  Transparency is set by the trans */
/*   argument.                                                        */
unsigned int *make_texture_checker_pattern(int xsize, int ysize, int trans)
{
unsigned int		*pixels;
	int		xcount;
	int		ycount;

	pixels = (unsigned int *) vrShmemAlloc(sizeof(unsigned int) * xsize * ysize);

	for (xcount = 0; xcount < xsize; xcount++) {
		for (ycount = 0; ycount < ysize; ycount++) {
			if (xcount % 2 == ycount % 2) {
				pixels[ycount * xsize + xcount] = 0xFFFFFFFF;
			} else {
				pixels[ycount * xsize + xcount] = (trans ? 0x60606000 : 0x606060FF);
			}
		}
	}

	return pixels;
}




/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* DEMO DRAW                                                     */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void demo_draw()
{
	clock_t	start, finish;
	float	loc[3];
	GLfloat	light0_pos[4] = { 0.0, 10.0, 0.0, 1.0 };
	GLfloat	light1_pos[4] = { 0.0,  5.0, 5.0, 1.0 };

//printf(RED_TEXT "drawing a frame\n" NORM_TEXT);

	start = clock();

#if 1 /* BS: cardnial VR sin #1 -- world simulation should NOT be done in the rendering */
	/**************************************/
	/* update on every other call to draw */
	/**************************************/
	if (SANDBOX_G.flip == 0)
		demo_update();
	SANDBOX_G.flip = !SANDBOX_G.flip;
#endif

	/*******************************/
	/* bind lights (for simulator) */
	/*******************************/
#if 0				/* NOTE: commented out in original source code -- I prefer "#if 0" */
	glCallList(1); 								/* IGL: lmbind(LIGHT1, 1); */
	glEnable(GL_LIGHT1);

	glCallList(1); 								/* IGL: lmbind(LMODEL, 1); */
	glEnable(LMODEL);
#endif
	glLightfv(GL_LIGHT0, GL_POSITION, light0_pos);
	glLightfv(GL_LIGHT1, GL_POSITION, light1_pos);

	/**********************************/
	/* clear the buffers and draw sky */
	/**********************************/
#if 0				/* NOTE: commented out in original source code -- I prefer "#if 0" */
/* force white backdrop for pictures */
	SANDBOX_backColour = (long)(255 * 256 * 256 + 255 * 256 + 255);
/* force black backdrop for pictures */
	SANDBOX_backColour = (long)(0);
#endif

#ifdef TOOGL_METHOD
	glClearDepth(1.0);
	glClearColor(((float)((SANDBOX_backColour) & 0xff)) / 255.,
			(float)((SANDBOX_backColour) >> 8 & 0xff) / 255.,
			(float)((SANDBOX_backColour) >> 16 & 0xff) / 255.,
			(float)((SANDBOX_backColour) >> 24 & 0xff) / 255.);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	/* IGL: czclear(SANDBOX_backColour, getgdesc(GD_ZMAX)); */
#else
	glClearDepth(1.0);
	glClearColor(((float)((SANDBOX_backColour) & 0xff)) / 255.,
			(float)((SANDBOX_backColour) >> 8 & 0xff) / 255.,
			(float)((SANDBOX_backColour) >> 16 & 0xff) / 255.,
			(float)((SANDBOX_backColour) >> 24 & 0xff) / 255.);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);	/* IGL: czclear(SANDBOX_backColour, getgdesc(GD_ZMAX)); */
#endif
#if 1	/* { */

	/****************************/
	/* primary drawing routines */
	/****************************/
	SANDBOX_EnableTransparency();
	SANDBOX_G.rotFix = SANDBOX_NO_ROTATE;

	SANDBOX_plane();

	SANDBOX_toolsOnPlane();

	SANDBOX_drawWand();

	glPushMatrix();								/* IGL: pushmatrix(); */
#if 0				/* NOTE: commented out in original source code -- I prefer "#if 0" */
		if (SANDBOX_THREEWALL)
			glRotatef(45, 0.0, 1.0, 0.0);						/* IGL: rot(45, 'y'); */
#endif
		SANDBOX_DisableTransparency();
		SANDBOX_navigationMenu();
		SANDBOX_EnableTransparency();
	glPopMatrix();								/* IGL: popmatrix(); */

	SANDBOX_sky();

	glPushMatrix();								/* IGL: pushmatrix(); */
		if (SANDBOX_DESKTOP || SANDBOX_THREEWALL) {
			SANDBOX_G.rotFix = SANDBOX_ROTATE_RC;
			glRotatef(90, 0.0, 1.0, 0.0);						/* IGL: rot(90, 'y'); */
		}
		SANDBOX_toolMenu();
		SANDBOX_G.rotFix = SANDBOX_NO_ROTATE;
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		if (SANDBOX_DESKTOP) {
			SANDBOX_G.rotFix = SANDBOX_ROTATE_LC;
			glRotatef(-90, 0.0, 1.0, 0.0);						/* IGL: rot(-90, 'y'); */
		}
		SANDBOX_dateTimeMenu();
		SANDBOX_G.rotFix = SANDBOX_NO_ROTATE;
	glPopMatrix();								/* IGL: popmatrix(); */

	if (SANDBOX_G.spoton) {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_cwhite);
		glEnable(GL_FRONT);						/* IGL: lmbind(MATERIAL, SANDBOX_cwhite); */
#else
		glColor4f(0.5, 0.5, 0.5, 0.6);					/* IGL: lmbind(MATERIAL, SANDBOX_cwhite); */
#endif

		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(SANDBOX_G.headX - 1, 5, SANDBOX_G.headZ - 1);		/* IGL: translate(); */
			glScalef(1, 5, 1);							/* IGL: scale(); */
			SANDBOX_drawcylinder();
		glPopMatrix();							/* IGL: popmatrix(); */
	}
#else /* } { */
	//glDisable(GL_TEXTURE_2D);
	//SANDBOX_EnableTransparency();
	//SANDBOX_plane(); 
	//SANDBOX_drawWand();
	//SANDBOX_dateTimeMenu();
#endif /* } */

	finish = clock();
#if 0
	fprintf(stderr, "     draw time: %8u\n", (finish - start) / 10000);
#endif
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/* SANDBOX DRAW WAND                                             */
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawWand()
{
	float	wand[3];

	/*******************************/
	/* the user has a tool in hand */
	/*******************************/
	if (SANDBOX_G.toolInHand) {
		SANDBOX_DisableTransparency();

		glPushMatrix();	/* IGL: pushmatrix(); */
			/*   CAVEWandOrientation; */
			wand[0] = SANDBOX_G.wandx;
			wand[1] = SANDBOX_G.wandy;
			wand[2] = SANDBOX_G.wandz;

			switch (SANDBOX_G.toolInHand) {
			case SANDBOX_THERMOMETERTOOL:
				SANDBOX_drawThermometer(wand, (SANDBOX_G.frameCounter % (SANDBOX_G.toolAnim + 5)) / (float) SANDBOX_G.toolAnim);
				break;
			case SANDBOX_BEAKERTOOL:
				SANDBOX_drawBeaker(wand, (SANDBOX_G.frameCounter % SANDBOX_G.toolAnim) / (float) SANDBOX_G.toolAnim);
				break;
			case SANDBOX_SATTOOL:
				SANDBOX_drawSat(wand);
				break;
			case SANDBOX_HELOTOOL:
				SANDBOX_drawHelo(wand);
				break;
			case SANDBOX_AIRPLANETOOL:
				SANDBOX_drawAirplane(wand);
				break;
			case SANDBOX_NOTETOOL:
				SANDBOX_drawNote(wand);
				break;
			case SANDBOX_CAMERATOOL:
				SANDBOX_drawCamera(wand);
				break;
			case SANDBOX_WINDTOOL:
				SANDBOX_drawWind(wand, (SANDBOX_G.frameCounter % SANDBOX_WINDRATE) / (float) SANDBOX_WINDRATE * 360, 0.0);
				break;
			}
		glPopMatrix();	/* IGL: popmatrix(); */
	}

	/*********************************/
	/* show the position of the wand */
	/*********************************/
	SANDBOX_EnableTransparency();

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_tyello);				 		/* IGL: lmbind(MATERIAL,SANDBOX_tyello); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.8, 0.8, 0.0, 0.5);						/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(SANDBOX_G.wandx, SANDBOX_G.wandy, SANDBOX_G.wandz);		/* IGL: translate(); */
		glScalef(0.01, 0.01, 0.25);							/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(SANDBOX_G.wandx, SANDBOX_G.wandy, SANDBOX_G.wandz);		/* IGL: translate(); */
		glScalef(0.25, 0.01, 0.01);							/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(SANDBOX_G.wandx, (SANDBOX_G.wandy - SANDBOX_CAVESIZE) / 2.0, SANDBOX_G.wandz);	/* IGL: translate(); */
		glScalef(0.01, fabs(SANDBOX_G.wandy + SANDBOX_CAVESIZE) / 2.0, 0.01);		/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */


}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* check this to see if timeCounter shouldnt replace frameCounter */

void SANDBOX_checkDay()
{
	int	counter;

	if (SANDBOX_G.currentDay != SANDBOX_NODATE)
		for (counter = 0; SANDBOX_G.activeDays[counter] < SANDBOX_G.currentDay; counter++) ;	/* yup */
	else	counter = 0;

	if (SANDBOX_G.activeDays[counter] == SANDBOX_G.currentDay) {
		if ((SANDBOX_G.timeCounter % SANDBOX_G.framesPerDay) == 0)
			if (SANDBOX_G.activeDays[counter + 1] != SANDBOX_NODATE)
				SANDBOX_G.currentDay = SANDBOX_G.activeDays[counter + 1];
			else	SANDBOX_G.currentDay = SANDBOX_G.activeDays[0];
	} else {		/* current day doesnt exist */

		if (SANDBOX_G.activeDays[counter] == SANDBOX_NODATE)
			SANDBOX_G.currentDay = SANDBOX_G.activeDays[0];
		else	SANDBOX_G.currentDay = SANDBOX_G.activeDays[counter];
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_toolMenuUpdate()
{
	float	ztrans, ytrans;
register int	k;

	if ((SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp)
		for (k = 0; k <= SANDBOX_MAXTOOLS - 1; ++k) {
			ztrans = SANDBOX_TOOLMENUZ + (k % 3 - 1) * 0.6;
			ytrans = SANDBOX_TOOLMENUY + 0.5 - (k / 3) * 0.6;

			if (SANDBOX_inSpecificRange(SANDBOX_TOOLMENUX, ytrans, ztrans, 0.5, SANDBOX_TOOLSIZE, SANDBOX_TOOLSIZE)) {
				SANDBOX_G.toolInHand = k + 1;
				SANDBOX_G.forceSelectUp = 1;
				SANDBOX_beepit();

				if ((k + 1) == SANDBOX_EMPTYTOOL)
					SANDBOX_G.toolInHand = SANDBOX_NOTOOL;
			}
		}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_toolMenu()
{

	float	toolLoc[3];
	float	ztrans, ytrans, xtrans;
register int	k;
	float	brownmt[] = {
			GL_EMISSION, 0.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
			GL_DIFFUSE, 0.5, 0.3, 0.2, 1.0, GL_SPECULAR, 0.0, 0.0, 0.0, 1.0,
			GL_SHININESS, 0.0,
		};

	SANDBOX_DisableTransparency();

#ifdef TOOGL_METHOD		/* BStest */
	glCallList(SANDBOX_brown); 						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
	glEnable(GL_FRONT);
#else
#  if 0
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_NORMALIZE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	glMaterialf(GL_FRONT_AND_BACK, GL_AMBIENT, brownmt[6]);
	glMaterialf(GL_FRONT_AND_BACK, GL_DIFFUSE, brownmt[11]);
#endif
	glColor4f(0.5, 0.3, 0.2, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(SANDBOX_TOOLMENUX, SANDBOX_TOOLMENUY, SANDBOX_TOOLMENUZ);		/* IGL: translate(); */
		glScalef(SANDBOX_TOOLMENUXSCALE, SANDBOX_TOOLMENUYSCALE, SANDBOX_TOOLMENUZSCALE);/*IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	xtrans = SANDBOX_TOOLMENUX;

	for (k = 0; k < SANDBOX_MAXTOOLS - 1; ++k) {
		ztrans = SANDBOX_TOOLMENUZ + (k % 3 - 1) * 0.6;
		ytrans = SANDBOX_TOOLMENUY + 0.5 - (k / 3) * 0.6;
#if 0				/* NOTE: commented out in original source code -- I prefer "#if 0" */
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_grey); 					/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.4, 0.4, 0.42, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(SANDBOX_TOOLMENUX - 0.04, ytrans, ztrans);			/* IGL: translate(); */
			glScalef(0.04, SANDBOX_TOOLSIZE, SANDBOX_TOOLSIZE);			/* IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */
#endif

		/***********************************************************/
		/* hilite individual tools if the wand is 'near' that tool */
		/***********************************************************/
		if (SANDBOX_inSpecificRange(SANDBOX_TOOLMENUX, ytrans, ztrans, 0.5, SANDBOX_TOOLSIZE, SANDBOX_TOOLSIZE)) {
			SANDBOX_EnableTransparency();

#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(SANDBOX_TOOLMENUX - 0.04, ytrans, ztrans);		/* IGL: translate(); */
				glScalef(0.045, SANDBOX_TOOLSIZE + 0.001, SANDBOX_TOOLSIZE + 0.001);/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */

			SANDBOX_DisableTransparency();
		}

		/******************************************/
		/* show the actual tools on the tool menu */
		/******************************************/
		switch (k + 1) {
		case SANDBOX_THERMOMETERTOOL:
			toolLoc[0] = xtrans - 0.2;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.05;
			toolLoc[2] = ztrans;
			SANDBOX_drawThermometer(toolLoc, (SANDBOX_G.frameCounter % (SANDBOX_G.toolAnim + 5)) / (float) SANDBOX_G.toolAnim);
			break;

		case SANDBOX_BEAKERTOOL:
			toolLoc[0] = xtrans - 0.2;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.06;
			toolLoc[2] = ztrans;
			SANDBOX_drawBeaker(toolLoc, (SANDBOX_G.frameCounter % SANDBOX_G.toolAnim) / (float) SANDBOX_G.toolAnim);
			break;

		case SANDBOX_SATTOOL:
			toolLoc[0] = xtrans - 0.3;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.3;
			toolLoc[2] = ztrans - 0.05;
			SANDBOX_drawSat(toolLoc);
			break;

		case SANDBOX_AIRPLANETOOL:
			toolLoc[0] = xtrans - 0.3;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.3;
			toolLoc[2] = ztrans - 0.05;
			SANDBOX_drawAirplane(toolLoc);
			break;

		case SANDBOX_HELOTOOL:
			toolLoc[0] = xtrans - 0.3;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.3;
			toolLoc[2] = ztrans - 0.05;
			SANDBOX_drawHelo(toolLoc);
			break;

		case SANDBOX_WINDTOOL:
			toolLoc[0] = xtrans - 0.2;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.05;
			toolLoc[2] = ztrans;
			SANDBOX_drawWind(toolLoc, (SANDBOX_G.frameCounter % SANDBOX_WINDRATE) / (float) SANDBOX_WINDRATE * 360, 0.0);
			break;

		case SANDBOX_CAMERATOOL:
			toolLoc[0] = xtrans - 0.2;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.15;
			toolLoc[2] = ztrans;
			SANDBOX_drawCamera(toolLoc);
			break;

		case SANDBOX_NOTETOOL:
			toolLoc[0] = xtrans - 0.2;
			toolLoc[1] = ytrans - SANDBOX_TOOLSIZE + 0.3;
			toolLoc[2] = ztrans;
			SANDBOX_drawNote(toolLoc);
			break;
		}
	}

	SANDBOX_EnableTransparency();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
#ifdef SOUND

void SANDBOX_sound()
{
	int		i, k, j, r, loop;
	float		lastGoodVal, prevGoodVal, graphValueOld;
	float		sumVal, val;
	int		countVal;
	float		thermoVal;

	i = SANDBOX_BEAKERTOOL;
	for (j = 0; j < SANDBOX_MAXSITE; j++)
		if ((gridTool[i][j].EE >= 0) && (gridTool[i][j].displaySound == 1)) {
			lastGoodVal = prevGoodVal = -99;
			loop = 0;
			k = gridTool[i][j].counter;
			while (!loop) {
				graphValueOld = gridTool[i][j].lastX[k];

				if (graphValueOld > -99) {
					prevGoodVal = lastGoodVal;
					lastGoodVal = gridTool[i][j].lastX[k];
				}
				k = (k + 1) % SANDBOX_GRAPHMAX;
				if (k == gridTool[i][j].counter)
					loop = 1;
			}
			if ((prevGoodVal != lastGoodVal) && (graphValueOld != -99) && (prevGoodVal != -99)) {
				r = rand() % 2;
				if (r)
					*bNote = actorMessage(A_BeginNote, "FS", *beakeractor, "drop1.aiff");
				else	*bNote = actorMessage(A_BeginNote, "FS", *beakeractor, "drop2.aiff");
			}
		}


	i = SANDBOX_THERMOMETERTOOL;
	sumVal = 0;
	countVal = 0;
	for (j = 0; j < SANDBOX_MAXSITE; j++)
		if ((gridTool[i][j].EE >= 0) && (gridTool[i][j].displaySound == 1)) {
			k = gridTool[i][j].counter - 1;
			if (k == -1)
				k = SANDBOX_GRAPHMAX - 1;
			val = gridTool[i][j].lastX[k];
			if (val != -99) {
				countVal++;
				sumVal += (val - gridTool[i][j].min) / (gridTool[i][j].max - gridTool[i][j].min);
			}
		}
	if (countVal > 0)
		thermoVal = (sumVal / countVal) * 0.25;
	else
		thermoVal = 0.0;

	if (thermoVal > 1.0)
		thermoVal = 1.0;

	actorMessage(A_SetAmpl, "FFF", *thermoactor, *tNote, thermoVal);
#if 0
	fprintf(stderr, "%f\n", thermoVal);
#endif

	i = SANDBOX_WINDTOOL;
	sumVal = 0;
	countVal = 0;
	for (j = 0; j < SANDBOX_MAXSITE; j++)
		if ((gridTool[i][j].EE >= 0) && (gridTool[i][j].displaySound == 1)) {
			k = gridTool[i][j].counter - 1;
			if (k == -1)
				k = SANDBOX_GRAPHMAX - 1;
			val = gridTool[i][j].lastX[k];
			if (val != -99) {
				countVal++;
				sumVal += (val - gridTool[i][j].min) / (gridTool[i][j].max - gridTool[i][j].min);
			}
		}
	if (countVal > 0)
		thermoVal = (sumVal / countVal);
	else
		thermoVal = 0.0;

	if (thermoVal > 1.0)
		thermoVal = 1.0;

	actorMessage(A_SetPitchNum, "FFF", *windactor, *wNote, thermoVal);
#if 0
	fprintf(stderr, "%f %f\n", countVal, thermoVal);
#endif
}

#endif

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_navigationMenu()
{
	float		Xonplane, Zonplane;
	float		planeSize;
	float		navCaveZ, navPlaneZ, modscale;
	int		i, j, k, l, loop;
	float		graphValueOld;
	float		vertexOld[3];
	char		showTime[8];
	float		adjustment;
	float		leftSide;
	int		leftHour, leftMinute;
	int		doit, showChart;
	int		oldLineWidth;
	GLfloat		redmt[] = {
				GL_EMISSION, 1.0, 0.0, 0.0, 1.0, GL_AMBIENT, 0.0, 0.0, 0.0, 1.0,
				GL_DIFFUSE, 0.8, 0.1, 0.1, 1.0, GL_SPECULAR, 0.95, 0.2, 0.2, 1.0,
				GL_SHININESS, 0.0,
			};
	int		tmp;	/* OGL: needed for glGetIntegerv() call */

	oldLineWidth = (int)(glGetIntegerv(GL_LINE_WIDTH, &tmp), tmp);		/* IGL: oldLineWidth = (int)getlwidth(); */

	/****************************/
	/* draw the navigation menu */
	/****************************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_brown); 						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.5, 0.3, 0.2, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(0.0, SANDBOX_TOOLMENUY, SANDBOX_NAVMENUZ);				/* IGL: translate(); */
		glScalef(2.0, 1.0, 0.05);							/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	planeSize = SANDBOX_G.planeScale / SANDBOX_CAVESIZE;
	if (planeSize > 1) {
		navPlaneZ = SANDBOX_NAVMENUZ + 0.05;
		navCaveZ = SANDBOX_NAVMENUZ + 0.07;
		modscale = 1.0 / planeSize;
	} else {
		navPlaneZ = SANDBOX_NAVMENUZ + 0.07;
		navCaveZ = SANDBOX_NAVMENUZ + 0.05;
		modscale = 1.0;
	}

	/*********************************/
	/* show the position of the CAVE */
	/*********************************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white); 						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(1.0, SANDBOX_TOOLMENUY, navCaveZ);					/* IGL: translate(); */
		glScalef(0.5 * modscale, 0.5 * modscale, 0.02);					/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/**********************************/
	/* show the position of the plane */
	/**********************************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_green); 						/* IGL: lmbind(MATERIAL,SANDBOX_green); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.4, 0.6, 0.6, 1.0);						/* IGL: lmbind(MATERIAL,SANDBOX_green); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(1.0, SANDBOX_TOOLMENUY, navPlaneZ);				/* IGL: translate(); */
		glScalef(planeSize * 0.5 * modscale, planeSize * 0.5 * modscale, 0.02);		/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*********************************/
	/* show the position of the user */
	/*********************************/
	Xonplane = (SANDBOX_G.headX / SANDBOX_CAVESIZE) * modscale;
	Zonplane = (SANDBOX_G.headZ / SANDBOX_CAVESIZE) * modscale;

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_blue); 						/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.15, 0.3, 0.7, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(Xonplane * 0.5 + 1.0, -Zonplane * 0.5 + SANDBOX_TOOLMENUY, SANDBOX_NAVMENUZ + 0.1);/* IGL: translate(); */
		glScalef(0.04, 0.04, 0.04);									/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*********************************/
	/* show the position of the wand */
	/*********************************/
	Xonplane = (SANDBOX_G.wandx / SANDBOX_CAVESIZE) * modscale;
	Zonplane = (SANDBOX_G.wandz / SANDBOX_CAVESIZE) * modscale;

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_gred);						/* IGL: lmbind(MATERIAL, SANDBOX_gred); */
	glEnable(GL_FRONT);
#else
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &redmt[ 6]);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &redmt[11]);
	glColor3f(0.8, 0.1, 0.1);						/* IGL: lmbind(MATERIAL, SANDBOX_gred); -- NOTE: also supposed to be emmisve: 0.8, 0.1, 0.1 */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(Xonplane * 0.5 + 1.0, -Zonplane * 0.5 + SANDBOX_TOOLMENUY, SANDBOX_NAVMENUZ + 0.1);/* IGL: translate(); */
		glScalef(0.04, 0.04, 0.04);									/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	/*******************/
	/* show the graphs */
	/*******************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white); 						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(-0.8, SANDBOX_TOOLMENUY, SANDBOX_NAVMENUZ + 0.05);			/* IGL: translate(); */
		glScalef(1.05, 0.85, 0.02);							/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black); 						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glColor4f(1.0, 1.0, 1.0, 1.0);	/* Temp: BSTest -- for some reason lines are black, so need some contrast */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(-0.8, SANDBOX_TOOLMENUY, SANDBOX_NAVMENUZ + 0.07);			/* IGL: translate(); */
		glScalef(1, 0.8, 0.02);								/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

	showChart = 0;

	/***************************/
	/* connect up known values */
	/***************************/
	vertexOld[2] = SANDBOX_NAVMENUZ + 0.1;

	for (i = 1; i < SANDBOX_MAXTOOLS; i++) {
		doit = 0;
		switch (i) {
		case SANDBOX_THERMOMETERTOOL:
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_gred); 				/* IGL: lmbind(MATERIAL, SANDBOX_gred); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.1, 0.1, 1.0);				/* IGL: lmbind(MATERIAL, SANDBOX_gred); -- NOTE: also supposed to be emmisve 1.0, 0.0, 0.0 */
#endif
			doit = 1;
			break;

		case SANDBOX_BEAKERTOOL:
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_gblue);
			glEnable(GL_FRONT); 					/* IGL: lmbind(MATERIAL, SANDBOX_gblue); */
#else
			glColor4f(0.15, 0.3, 0.7, 1.0);				/* IGL: lmbind(MATERIAL, SANDBOX_gblue); -- NOTE: also supposed to be emmisive 0.5, 0.5, 1.0 */
#endif
			doit = 1;
			break;

		case SANDBOX_WINDTOOL:
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_gtyello);				/* IGL: lmbind(MATERIAL, SANDBOX_gtyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.5, 0.5, 0.0, 1.0);				/* IGL: lmbind(MATERIAL, SANDBOX_gtyello); -- NOTE: also supposed to be emmisive 0.9, 0.9, 0.0 */
#endif
			doit = 1;
			break;
		};

		glColor4f(1.0, 1.0, 1.0, 1.0);	/* Temp: bstest */
		if (doit)
			for (j = 0; j < SANDBOX_MAXSITE; j++)
				if ((gridTool[i][j].EE >= 0) && (gridTool[i][j].displayGraph == 1)) {
					showChart = 1;

					adjustment = gridTool[i][j].max - gridTool[i][j].min;
					glLineWidth((GLfloat)(2));		/* IGL: linewidth(3); */
					glBegin(GL_LINE_STRIP);			/* IGL: bgnline(); *//* for OGL: for multiple, independent line segments: use GL_LINES */
					k = gridTool[i][j].counter;
					l = 0;
					loop = 0;
					while (!loop) {
						graphValueOld = gridTool[i][j].lastX[k] - gridTool[i][j].min;

						vertexOld[0] = -1.7 + 1.9 * l / SANDBOX_GRAPHMAX;
						if (graphValueOld / adjustment > 1.0)
							adjustment = graphValueOld;
						if ((graphValueOld / adjustment < 0.0) && (graphValueOld > -99))
							graphValueOld = 0;

						vertexOld[1] = SANDBOX_TOOLMENUY - 0.75 + 1.2 * graphValueOld / adjustment;
						if (graphValueOld > -99) {
							glVertex3fv(vertexOld);		/* IGL: v3f(vertexOld); */
						}
						k = (k + 1) % SANDBOX_GRAPHMAX;
						l++;
						if (k == gridTool[i][j].counter)
							loop = 1;
					}
					glEnd();				/* IGL: endline(); */
				}
	}

	glLineWidth((GLfloat)(oldLineWidth));					/* IGL: linewidth(oldLineWidth); */

	if (showChart) {
		printf("showing chart\n");

		/****************/
		/* label graph  */
		/****************/
		leftSide = SANDBOX_G.timeOfDay - ((float) SANDBOX_GRAPHMAX / SANDBOX_G.framesPerDay);
		if (leftSide < 0.0)
			leftSide = 1.0 + leftSide;
		leftHour = (int)(leftSide * 24);
		leftMinute = (int)((leftSide * 24 - leftHour) * 60 + 0.5);

		sprintf(showTime, "%2d:%02d", leftHour, leftMinute);
		SANDBOX_drawText(0, 0, showTime, -1.50, SANDBOX_TOOLMENUY + 0.7, SANDBOX_NAVMENUZ + 0.1);

		sprintf(showTime, "%2d:%02d", SANDBOX_G.thaHour, SANDBOX_G.thaMinute);
		SANDBOX_drawText(0, 0, showTime, -0.15, SANDBOX_TOOLMENUY + 0.7, SANDBOX_NAVMENUZ + 0.1);
	}
}


/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/* something is amiss with the week calendar, first I need to add
7 to it, and now I need to remove that +7 from both functions
*/

void SANDBOX_dateTimeMenuUpdate()
{
	int		day, week, date;
	float		hiy, hiz, wy, wz;

	hiy = 1.46 / 6 / 2;	/* because of extra space at the bottom of the texture */
	hiz = 1.5 / 7 / 2;

	wy = SANDBOX_G.wandy;
	wz = SANDBOX_G.wandz;

	if (SANDBOX_G.rotFix == SANDBOX_ROTATE_LC) {	/* left to center */
		wz = -SANDBOX_G.wandx;
	}

	if ((SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp) {
		if (SANDBOX_inSpecificRange(SANDBOX_DATEMENUX, SANDBOX_DATEMENUY, SANDBOX_DATEMENUZ, 0.5, SANDBOX_DATEMENUYSCALE, SANDBOX_DATEMENUZSCALE)) {
			week = (int)((wy - SANDBOX_DATEMENUY - 0.75 - hiy) / (-2 * hiy) - 0.5);
			day = (int)((wz - SANDBOX_DATEMENUZ - 0.75 - hiz) / (-2 * hiz) - 0.5);
			date = week * 7 - 5 + day;

			if ((date >= 1) && (date <= 31) && (day >= 0) && (day <= 6)) {
				SANDBOX_selectDeselectDate(date);
				SANDBOX_G.forceSelectUp = 1;
				SANDBOX_beepit();
			}
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_dateTimeMenu()
{
	float		splane[4] = { 0, 0, 1, 0 };
	float		tplane[4] = { 0, 1, 0, 0 };

	float		hiy, hiz;
	float		wy, wz;
	int		day, week, date;
	int		counter;

	float		cube[8][3] = {
				0.0, 1.0, 1.0,
				0.0, 0.0, 1.0,
				1.0, 0.0, 1.0,
				1.0, 1.0, 1.0,
				1.0, 1.0, 0.0,
				1.0, 0.0, 0.0,
				0.0, 0.0, 0.0,
				0.0, 1.0, 0.0
			};

	float		cube_normals[6] = { 1.0, 0.0, 0.0 };

	SANDBOX_DisableTransparency();

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_brown);						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.5, 0.3, 0.2, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(SANDBOX_DATEMENUX, SANDBOX_DATEMENUY, SANDBOX_DATEMENUZ);		/* IGL: translate(); */
		glScalef(SANDBOX_DATEMENUXSCALE, SANDBOX_DATEMENUYSCALE, SANDBOX_DATEMENUZSCALE);/*IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white); 						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif
	glPushMatrix();								/* IGL: pushmatrix(); */
		glColor4fv(SANDBOX_whiteColour);						/* IGL: c4f(SANDBOX_whiteColour); */

		if (SANDBOX_G.showDetail) {
#if 0 /* { */
			glCallList(GLLIST_19);
			glEnable(GL_TEXTURE_2D);
#else /* } { */
	//glEnable(GL_NORMALIZE);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_2);				/* IGL: texbind(TX_TEXTURE_0, 2) -- calendar texture */
			/* TODO: OGL equivalent? */						/* IGL: tevbind(TV_ENVO, 1); */

			/* from example-13 */
			if (1) {
#if 0
				void	*texture_pixels;

				texture_pixels = (void *) make_texture_checker_pattern(4, 4, 1);

				glTexImage2D(GL_TEXTURE_2D,	/* GL_TEXTURE_2D or GL_PROXY_TEXTURE_2D */
					0,				/* level-of-detail (mip-map), 0 is base */
					4,				/* number of color components or format */
					4,				/* x-size of image */
					4,				/* y-size of image */
					0,				/* size of border (0 or 1) */
					GL_RGBA,			/* format of the pixel data */
					GL_UNSIGNED_INT_8_8_8_8,	/* type of the pixel data */
					texture_pixels);		/* pointer to the image data */
#endif

	/* NOTE: the following tests were for the original binding of the texture, which */
	/*   did not include a call to set the GL_TEXTURE_MIN_FILTER parameter.          */
#if 0
				/* all: produces "reflective texture" */
				/* calonly: produces good texturing */
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#elif 0
				/* all: produces "reflective texture" */
				/* calonly: produces good texturing */
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#elif 0
				/* all: produces no texturing */
				/* calonly: produces no texturing */
				glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#elif 0
				/* all: produces no texturing */
				/* calonly: produces no texturing */
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
#elif 0
				/* all: produces repeating sliding texture -- perhaps an "infinite reflextive texture" */
				/* calonly: produces good texturing */
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#elif 0
				/* calonly: produces no texturing */
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#elif 0
				/* calonly: produces good texturing */
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#else
				/* all: produces no texturing */
				/* calonly: produces no texturing */
				/* nop */
#endif
			}

#endif /* } */
#if 0
			glCallList(GLLIST_18);							/* IGL: tevbind ?? */
			glEnable(GL_TEXTURE_2D);
#else
			/* TODO: implement this -- if it translates */
#endif

#if 0 /* new for v3 -- 05/26/2006: this was wrong GL_EYE_LINEAR is not valid for the 2nd argument (so I'm trying GL_EYE_PLANE) */
			glTexGenfv(GL_S, GL_EYE_LINEAR, splane);				/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_LINEAR, tplane);				/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#else
			glTexGenfv(GL_S, GL_EYE_PLANE, splane);					/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_PLANE, tplane);					/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#endif
#if 0
			/* OGLXXX
			 * tg_on use: 
			 * glEnable(GL_TEXTURE_GEN_S);
			 * glEnable(GL_TEXTURE_GEN_T);
			 * texgen: translate parameters.
			 */
			glTexGenfv(GL_S, XXX_TG_ON, NULL);					/* IGL: texgen(TX_S, TG_ON, NULL); */
			/* OGLXXX
			 * tg_on use: 
			 * glEnable(GL_TEXTURE_GEN_S);
			 * glEnable(GL_TEXTURE_GEN_T);
			 * texgen: translate parameters.
			 */
			glTexGenfv(GL_T, XXX_TG_ON, NULL);					/* IGL: texgen(TX_S, TG_ON, NULL); */
#else
			//glEnable(GL_TEXTURE_GEN_S);
			//glEnable(GL_TEXTURE_GEN_T);
#endif
		}

		glTranslatef(SANDBOX_DATEMENUX + 0.05, SANDBOX_DATEMENUY - 0.75, SANDBOX_DATEMENUZ - 0.75);	/* IGL: translate(); */
		glScalef(0.01, SANDBOX_CALENDARSCALE, SANDBOX_CALENDARSCALE);			/* IGL: scale(); */

		/* NOTE: split into multi-line and added the texture coordinates to what toogl provided */
		glBegin(GL_POLYGON);								/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
		glNormal3fv(cube_normals);								/* IGL: n3f(cube_normals); */
		glTexCoord2f(0.0, 1.0);									/* IGL: no-op -- used auto texturing */
		glVertex3fv(cube[4]);									/* IGL: v3f(cube[4]); */
		glTexCoord2f(0.0, 0.0);									/* IGL: no-op -- used auto texturing */
		glVertex3fv(cube[5]);									/* IGL: v3f(cube[5]); */
		glTexCoord2f(1.0, 0.0);									/* IGL: no-op -- used auto texturing */
		glVertex3fv(cube[2]);									/* IGL: v3f(cube[2]); */
		glTexCoord2f(1.0, 1.0);									/* IGL: no-op -- used auto texturing */
		glVertex3fv(cube[3]);									/* IGL: v3f(cube[3]); */
		glEnd();									/* IGL: endpolygon(); */

		glDisable(GL_TEXTURE_2D);							/* IGL: texbind(TX_TEXTURE_0, 0); */
	glPopMatrix();								/* IGL: popmatrix(); */

	SANDBOX_EnableTransparency();

	hiy = 1.46 / 6 / 2;	/* because of extra space at the bottom of the texture */
	hiz = 1.5 / 7 / 2;

	/**************************************/
	/* hilite the currently selected days */
	/**************************************/
	for (counter = 0; SANDBOX_G.activeDays[counter] < SANDBOX_NODATE; ++counter) {
		day = ((SANDBOX_G.activeDays[counter] + 5) % 7);
		week = ((SANDBOX_G.activeDays[counter] + 5) / 7);

		if (SANDBOX_G.currentDay == SANDBOX_G.activeDays[counter]) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tred);				 /* IGL: lmbind(MATERIAL, SANDBOX_tred); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.1, 0.0, 0.6);				/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
#endif
		} else {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_oblue); 				/* IGL: lmbind(MATERIAL, SANDBOX_oblue); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.15, 0.3, 0.7, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_oblue); -- NOTE: also supposed to be emmisive: 0.3, 0.5, 1.0 */
#endif
		}

		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(SANDBOX_DATEMENUX + 0.05, SANDBOX_DATEMENUY + 0.75 - (week * 2 * hiy + hiy), SANDBOX_DATEMENUZ + 0.75 - (day * 2 * hiz + hiz));	/* IGL: translate(); */
			glScalef(0.05, hiy, hiz);						/* IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */
	}

	/*********************************************/
	/* hilite day if the wand is 'near' that day */
	/*********************************************/
	if (SANDBOX_inSpecificRange(SANDBOX_DATEMENUX, SANDBOX_DATEMENUY, SANDBOX_DATEMENUZ, 0.5, SANDBOX_DATEMENUYSCALE, SANDBOX_DATEMENUZSCALE)) {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_tyello); 					/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.8, 0.8, 0.0, 0.5);					/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

		wy = SANDBOX_G.wandy;
		wz = SANDBOX_G.wandz;

		if (SANDBOX_G.rotFix == SANDBOX_ROTATE_LC) {	/* left to center */
			wz = -SANDBOX_G.wandx;
		}

		week = (int)((wy - SANDBOX_DATEMENUY - 0.75 - hiy) / (-2 * hiy) - 0.5);
		day = (int)((wz - SANDBOX_DATEMENUZ - 0.75 - hiz) / (-2 * hiz) - 0.5);

		date = week * 7 - 5 + day;
		if ((date >= 1) && (date <= 31) && (day >= 0) && (day <= 6)) {
			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(SANDBOX_DATEMENUX + 0.05, SANDBOX_DATEMENUY + 0.75 - (week * 2 * hiy + hiy), SANDBOX_DATEMENUZ + 0.75 - (day * 2 * hiz + hiz));	/* IGL: translate(); */
				glScalef(0.05, hiy, hiz);					/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_selectDeselectDate(int date)
{
	int	counter, counter2, temp1, temp2;

	for (counter = 0; (counter < SANDBOX_MAXDATES) && (SANDBOX_G.activeDays[counter] < date); counter++) ;	/* yes ; */

	if (SANDBOX_G.activeDays[counter] == date) {
		/*******************/
		/* deselect a date */
		/*******************/
		for (counter2 = counter + 1; counter2 < SANDBOX_MAXDATES; counter2++)
			SANDBOX_G.activeDays[counter2 - 1] = SANDBOX_G.activeDays[counter2];

		SANDBOX_G.activeDays[SANDBOX_MAXDATES - 1] = SANDBOX_NODATE;
	} else {
		/***********************/
		/* select (add) a date */
		/***********************/
		temp1 = date;
		for (counter2 = counter; counter2 < (SANDBOX_MAXDATES - 1); counter2++) {
			temp2 = SANDBOX_G.activeDays[counter2];
			SANDBOX_G.activeDays[counter2] = temp1;
			temp1 = temp2;
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_planeUpdate()
{
	int	overSite = 0;
	int	i, j;
	float	location[2];

	for (i = 1; i < SANDBOX_MAXTOOLS; i++)
		for (j = 0; j < SANDBOX_MAXSITE; j++)
			if (gridTool[i][j].EE >= 0) {
				location[0] = (gridTool[i][j].EE - 50) / 50.0 * SANDBOX_G.planeScale;
				location[2] = (gridTool[i][j].NN - 50) / 50.0 * SANDBOX_G.planeScale;

				if (SANDBOX_inSpecificRange (location[0], SANDBOX_G.wandy, location[2], SANDBOX_ACTIVEGRIDSIZE, SANDBOX_CAVESIZE, SANDBOX_ACTIVEGRIDSIZE)) {
					overSite = 1;
				}
			}


	if ((SANDBOX_G.optionButton == ON) && (SANDBOX_G.wandy <= SANDBOX_PLANEMENUYMAX) && !SANDBOX_G.forceOptionUp && !SANDBOX_G.planeMenu && !overSite) {
		SANDBOX_G.planeMenu = 1;
		SANDBOX_G.planeMenuX = SANDBOX_G.wandx;
		SANDBOX_G.planeMenuY = SANDBOX_G.wandy;
		SANDBOX_G.planeMenuZ = SANDBOX_G.wandz;
		SANDBOX_beepit();
		SANDBOX_G.forceOptionUp = 1;
	}

	if ((SANDBOX_G.optionButton == ON) && (SANDBOX_G.wandy <= SANDBOX_PLANEMENUYMAX) && !SANDBOX_G.forceOptionUp && SANDBOX_G.planeMenu && !overSite) {
		SANDBOX_G.planeMenu = 0;
		SANDBOX_beepit();
		SANDBOX_G.forceOptionUp = 1;
	}

	if (SANDBOX_G.planeMenu && !SANDBOX_G.toolInHand)
		SANDBOX_planeOptions(SANDBOX_DO, SANDBOX_G.planeMenuX, SANDBOX_G.planeMenuY, SANDBOX_G.planeMenuZ);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_plane()
{
	float	left, back;
	float	gridSize, gridHeight;
register int	i;

	float	splane[4] = { 1, 0, 0, 0 };
	float	tplane[4] = { 0, 0, 1, 0 };

	/*************/
	/* gridlines */
	/*************/
	if (SANDBOX_G.showGrid) {
		SANDBOX_DisableTransparency();

		left = SANDBOX_G.planeX - SANDBOX_G.planeScale;
		back = SANDBOX_G.planeZ - SANDBOX_G.planeScale;

		gridSize = SANDBOX_G.planeScale / 10.0;
		gridHeight = SANDBOX_G.planeY + 0.25;

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_green); 					/* IGL: lmbind(MATERIAL,SANDBOX_green); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.4, 0.6, 0.6, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_green); */
#endif

#     if 1 /* {  -- this section does not break things */
		for (i = 1; i <= 19; i++) {
			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(SANDBOX_G.planeX, gridHeight, back + (i * gridSize));/*IGL: translate(); */
				glScalef(SANDBOX_G.planeScale, 0.03, 0.004);			/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}

		for (i = 1; i <= 19; i++) {
			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(left + (i * gridSize), gridHeight, SANDBOX_G.planeZ);/*IGL: translate(); */
				glScalef(0.004, 0.03, SANDBOX_G.planeScale);			/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}
#     endif /* } */
	}
#     if 1 /* {  -- this section breaks things */
	/******************/
	/* draw the plane */
	/******************/
	SANDBOX_DisableTransparency();

	glPushMatrix();								/* IGL: pushmatrix(); */
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_brown); 						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.5, 0.3, 0.2, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_brown); */
#endif

		if (SANDBOX_G.showDetail) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_bwhite); 					/* IGL: lmbind(MATERIAL, SANDBOX_bwhite); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.9, 0.9, 0.9, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_bwhite); */
#endif
#if 0 /* BS: the terrain seems too bright */
			glColor4fv(SANDBOX_whiteColour);				/* IGL: c4f(SANDBOX_whiteColour); */
#endif

#ifdef TOOGL_METHOD
			glCallList(GLLIST_19); 						/* IGL: texbind(TX_TEXTURE_0, 1); */
			glEnable(GL_TEXTURE_2D);
#  if 0 /* OGL duplicates effort */
			glCallList(GLLIST_19); 						/* IGL: tevbind(TV_ENV0, 1); */
			glEnable(GL_TEXTURE_2D);
#  endif
#else
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_1);
#endif

#if 0 /* new for v3 -- 05/26/2006: this was wrong GL_EYE_LINEAR is not valid for the 2nd argument (so I'm trying GL_EYE_PLANE) */
			glTexGenfv(GL_S, GL_EYE_LINEAR, splane);			/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_LINEAR, tplane);			/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#else
			glTexGenfv(GL_S, GL_EYE_PLANE, splane);				/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_PLANE, tplane);				/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#endif
#if 0	/* { */
#ifdef TOOGL_METHOD
			/* TODO: add glEnable(GL_TEXTURE_GEN_S); & glEnable(GL_TEXTURE_GEN_T); as per toogl instructions */
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
			glTexGenfv(GL_S, XXX_TG_ON, NULL);				/* IGL: texgen(TX_S,TG_ON,NULL); */
			glTexGenfv(GL_T, XXX_TG_ON, NULL);				/* IGL: texgen(TX_T,TG_ON,NULL); */
#else
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
#endif
#endif	/* } */
		}

		glTranslatef(SANDBOX_G.planeX - SANDBOX_G.planeScale, SANDBOX_G.planeY, SANDBOX_G.planeZ - SANDBOX_G.planeScale);/*IGL: translate(); */
		glScalef(2 * SANDBOX_G.planeScale, SANDBOX_PLANEELEVATION, 2 * SANDBOX_G.planeScale);				/* IGL: scale(); */

		SANDBOX_drawplane();

#ifdef TOOGL_METHOD
		glDisable(GL_TEXTURE_2D); 						/* IGL: texbind(TX_TEXTURE_0, 0); */
#else
		/* OGL: TODO: do we need to do an OGL command here? */
#endif
		SANDBOX_EnableTransparency();
	glPopMatrix();								/* IGL: popmatrix(); */
#endif /* } */


	if ((SANDBOX_G.showDetail) && (SANDBOX_G.heloInSky)) {
		glPushMatrix();							/* IGL: pushmatrix(); */
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_oblue); 					/* IGL: lmbind(MATERIAL, SANDBOX_oblue); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.15, 0.3, 0.7, 0.5);					/* IGL: lmbind(MATERIAL, SANDBOX_oblue); -- NOTE: in the original this was emmisive: 0.3, 0.5, 1.0 */
#endif
#ifdef TOOGL_METHOD
			glCallList(GLLIST_23); 						/* IGL: texbind(TX_TEXTURE_0, 3); */
			glEnable(GL_TEXTURE_2D);

			glCallList(GLLIST_22); 						/* IGL: tevbind(TV_ENV0, 2); */
			glEnable(GL_TEXTURE_2D);
#else
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_3);			/* IGL: texbind(TX_TEXTURE_0, 3); & tevbind(TV_ENV0, 2); --  the overlay texture */
#endif

#if 0 /* new for v3 -- 05/26/2006: this was wrong GL_EYE_LINEAR is not valid for the 2nd argument (so I'm trying GL_EYE_PLANE) */
			glTexGenfv(GL_S, GL_EYE_LINEAR, splane);			/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_LINEAR, tplane);			/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#else
			glTexGenfv(GL_S, GL_EYE_PLANE, splane);				/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_PLANE, tplane);				/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#endif
#if 0
			/* TODO: add glEnable(GL_TEXTURE_GEN_S); & glEnable(GL_TEXTURE_GEN_T); as per toogl instructions */
			glTexGenfv(GL_S, XXX_TG_ON, NULL);				/* IGL: texgen(TX_S,TG_ON,NULL); */
			glTexGenfv(GL_T, XXX_TG_ON, NULL);				/* IGL: texgen(TX_T,TG_ON,NULL); */
#else
#  if 0
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
#  endif
#endif

			/* draw the terrain plane with the overlay texture instead, and raised a slight amount */
			glTranslatef(SANDBOX_G.planeX - SANDBOX_G.planeScale, SANDBOX_G.planeY + 0.01, SANDBOX_G.planeZ - SANDBOX_G.planeScale);/* IGL: translate();*/
			glScalef(2 * SANDBOX_G.planeScale, SANDBOX_PLANEELEVATION, 2 * SANDBOX_G.planeScale);					/* IGL: scale(); */

			SANDBOX_drawplane();

			glDisable(GL_TEXTURE_2D);					/* IGL: texbind(TX_TEXTURE_0, 0); */

		glPopMatrix();							/* IGL: popmatrix(); */
	}

	/****************************/
	/* popup menu for the plane */
	/****************************/
	if (SANDBOX_G.planeMenu && !SANDBOX_G.toolInHand)
		SANDBOX_planeOptions(SANDBOX_DRAW, SANDBOX_G.planeMenuX, SANDBOX_G.planeMenuY, SANDBOX_G.planeMenuZ);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_toolsOnPlaneUpdate()
{
	float			location[3];
	float			siteEE, siteNN;
	int			j, i, c;
	int			xmiddle, ymiddle;
	int			siteheight;
	struct SANDBOX_eachDay	*everyDay;
	float			row1, row2, row3;
	int			EEVal, NNVal;
	int			overT, overS;
	char			filename[10];

	int			overSite = 0;

	for (i = 1; i < SANDBOX_MAXTOOLS; i++)
		for (j = 0; j < SANDBOX_MAXSITE; j++)
			if (gridTool[i][j].EE >= 0) {
				location[0] = (gridTool[i][j].EE - 50) / 50.0 * SANDBOX_G.planeScale;
				location[2] = (gridTool[i][j].NN - 50) / 50.0 * SANDBOX_G.planeScale;

				if (SANDBOX_inSpecificRange (location[0], SANDBOX_G.wandy, location[2], SANDBOX_ACTIVEGRIDSIZE, SANDBOX_CAVESIZE, SANDBOX_ACTIVEGRIDSIZE)) {
					overSite = 1;
					overS = j;
					overT = i;
				}
			}

	if (!SANDBOX_G.toolInHand && (SANDBOX_G.optionButton == ON) && !SANDBOX_G.forceOptionUp && overSite && !SANDBOX_G.instrumentMenu) {
		SANDBOX_G.instrumentMenu = 1;
		SANDBOX_G.instrumentMenuTool = overT;
		SANDBOX_G.instrumentMenuSite = overS;
		SANDBOX_G.instrumentMenuX = SANDBOX_G.wandx;
		SANDBOX_G.instrumentMenuY = SANDBOX_G.wandy;
		SANDBOX_G.instrumentMenuZ = SANDBOX_G.wandz;
		SANDBOX_beepit();
		SANDBOX_G.forceOptionUp = 1;
	}

	if (!SANDBOX_G.toolInHand && (SANDBOX_G.optionButton == ON) && !SANDBOX_G.forceOptionUp && overSite && SANDBOX_G.instrumentMenu) {
		SANDBOX_G.instrumentMenu = 0;
		SANDBOX_beepit();
		SANDBOX_G.forceOptionUp = 1;
	}


	/****************************/
	/* picking up an instrument */
	/****************************/
	if (!SANDBOX_G.toolInHand && (SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp)
		for (i = 1; i < SANDBOX_MAXTOOLS; i++)
			for (j = 0; j < SANDBOX_MAXSITE; j++)
				if (gridTool[i][j].EE >= 0) {
					location[0] = (gridTool[i][j].EE - 50) / 50.0 * SANDBOX_G.planeScale;
					location[2] = (gridTool[i][j].NN - 50) / 50.0 * SANDBOX_G.planeScale;

					if (SANDBOX_inSpecificRange (location[0], SANDBOX_G.wandy, location[2], SANDBOX_ACTIVEGRIDSIZE, SANDBOX_CAVESIZE, SANDBOX_ACTIVEGRIDSIZE)) {
						SANDBOX_removeToolFromGrid(i, j);
						SANDBOX_G.toolInHand = i;
						SANDBOX_G.forceSelectUp = 1;
						SANDBOX_beepit();
					}
				}

	/*************************/
	/* placing an instrument */
	/*************************/
	if (SANDBOX_G.toolInHand && (SANDBOX_G.toolInHand != SANDBOX_SATTOOL) && (SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp)
		for (i = 0; (i < SANDBOX_MAXSITE) && (SANDBOX_legalSites[SANDBOX_G.toolInHand][i] >= 0); i++) {
			EEVal = siteData[SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].EE;
			NNVal = siteData[SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].NN;

			xmiddle = EEVal / (100 / SANDBOX_TMESHSIZE);
			ymiddle = NNVal / (100 / SANDBOX_TMESHSIZE);

			siteEE = (EEVal - 50) / 50.0 * SANDBOX_G.planeScale;
			siteNN = (NNVal - 50) / 50.0 * SANDBOX_G.planeScale;

			siteheight =
				siteData[SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].elevation;

			if (SANDBOX_inSpecificRange(siteEE, SANDBOX_G.wandy, siteNN, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_CAVESIZE, SANDBOX_ACTIVEGRIDSIZE)) {
				SANDBOX_addToolToGrid(siteData [SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].  EE, siteData[SANDBOX_legalSites [SANDBOX_G.toolInHand][i]].NN, siteheight);
				SANDBOX_beepit();
				SANDBOX_G.toolInHand = SANDBOX_NOTOOL;
				SANDBOX_G.forceSelectUp = 1;
			}
		}

	/*****************************************/
	/* setup data to be displayed this frame */
	/*****************************************/
	for (i = 1; i < SANDBOX_MAXTOOLS; i++)
		if (i == SANDBOX_CAMERATOOL) {
#if 0 /* changed to #if ... #endif */
		/* COMMENTED OUT FOR SIGGRAPH */
			for (j=0;(gridTool[i][j].EE >=0);j++)
				if (SANDBOX_pics[gridTool[i][j].NN*100+gridTool[i][j].EE] == NULL)
					SANDBOX_loadSitePic(gridTool[i][j].NN,  gridTool[i][j].EE);
#endif
		} else
			for (j = 0; j < SANDBOX_MAXSITE; j++)
				if (gridTool[i][j].EE >= 0) {
					if (!SANDBOX_G.dateSelected) {
						gridTool[i][j].nowData = -99;
						gridTool[i][j].nowData2 = -99;
						gridTool[i][j].nowData3 = -99;
					} else {
						SANDBOX_getDataRow(i, j, 8, SANDBOX_G.currentDay, SANDBOX_G.time24);

						everyDay = gridTool[i][j].month;
						row1 = 0.0;

						if (everyDay[SANDBOX_G.currentDay].day != NULL) {
							for (c = 0; (everyDay[SANDBOX_G.currentDay].day[c].time < SANDBOX_G.time24) && (everyDay[SANDBOX_G.currentDay].day[c].time != -1); c++) ;	/* yup */
							{
								row1 = everyDay[SANDBOX_G.currentDay].day[c].value;
								row2 = everyDay[SANDBOX_G.currentDay].day[c].value2;
								row3 = everyDay[SANDBOX_G.currentDay].day[c].value3;
							}

							gridTool[i][j].nowData = row1;
							gridTool[i][j].nowData2 = row2;
							gridTool[i][j].nowData3 = row3;

							if (!SANDBOX_G.smoonSelected && !SANDBOX_G.sunOptions && SANDBOX_G.dateSelected) {
								if (i == SANDBOX_WINDTOOL) {
									gridTool[i][j].lastX[gridTool[i][j].counter] = sqrt(pow(row1, 2) + pow(row2, 2) + pow(row3, 2));
									if ((row1 == -99) || (row2 == -99) || (row3 == -99))
										gridTool[i][j].lastX[gridTool[i][j].counter] = -99;
								} else {
									gridTool[i][j].lastX[gridTool[i][j].counter] = row1;
								}
								gridTool[i][j].counter = (gridTool[i][j].counter + 1) % SANDBOX_GRAPHMAX;
							}
						}
					}
				}

	if (SANDBOX_G.instrumentMenu) {
		SANDBOX_toolOptions(SANDBOX_G.instrumentMenuTool, SANDBOX_G.instrumentMenuSite, SANDBOX_DO, SANDBOX_G.instrumentMenuX, SANDBOX_G.instrumentMenuY, SANDBOX_G.instrumentMenuZ);
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_toolsOnPlane()
{
	float		siteEE, siteNN;
	float		activeHeight;
	float		row1, row2, row3;
	float		ang, ang2, distAway;
	float		location[3];
	int		j, i, k, l;
	int		siteheight;
	int		xmiddle, ymiddle, tempElev, tempCount;
	int		EEVal, NNVal;
	char		textRow[10];
	char		units[5];

	/******************************************************/
	/* display all the instruments currently on the plane */
	/******************************************************/

	for (i = 1; i < SANDBOX_MAXTOOLS; i++)
		for (j = 0; j < SANDBOX_MAXSITE; j++)
			if (gridTool[i][j].EE >= 0) {
				location[0] = (gridTool[i][j].EE - 50) / 50.0 * SANDBOX_G.planeScale;
				location[2] = (gridTool[i][j].NN - 50) / 50.0 * SANDBOX_G.planeScale;

				/*************************/
				/* get elevation of tool */
				/*************************/
				xmiddle = gridTool[i][j].EE / (100 / SANDBOX_TMESHSIZE);
				ymiddle = gridTool[i][j].NN / (100 / SANDBOX_TMESHSIZE);

				tempElev = 0;
				tempCount = 0;
				for (k = xmiddle - 1; k <= xmiddle + 1; k++)
					for (l = ymiddle - 1; l <= ymiddle + 1; l++) {
						if ((k >= 0) && (k < SANDBOX_TMESHSIZE) && (l >= 0) && (l < SANDBOX_TMESHSIZE)) {
							tempElev += SANDBOX_finalElev[k][l];
							tempCount++;
						}
					}

				tempElev /= tempCount;

				if (SANDBOX_finalElev[xmiddle][ymiddle] > tempElev)
					tempElev = SANDBOX_finalElev[xmiddle][ymiddle];

				location[1] = (tempElev - SANDBOX_G.averageElevation) / (float)(SANDBOX_G.maxElevation - SANDBOX_G.averageElevation) * SANDBOX_PLANEELEVATION + SANDBOX_G.planeY + 0.05;

				/*********************/
				/* retrieve the data */
				/*********************/
				row1 = gridTool[i][j].nowData;
				row2 = gridTool[i][j].nowData2;
				row3 = gridTool[i][j].nowData3;

				/**********************/
				/* visualize the data */
				/**********************/
				switch (i) {
				case SANDBOX_THERMOMETERTOOL:
					SANDBOX_drawThermometer(location, row1 / 30.0);
					strcpy(units, "C");
					break;

				case SANDBOX_BEAKERTOOL:
					SANDBOX_drawBeaker(location, row1 / 60.0);
					strcpy(units, "mm");
					break;

				case SANDBOX_CAMERATOOL:
					SANDBOX_drawCamera(location);
					row1 = -99;
					SANDBOX_holidaySnaps(gridTool[i][j].NN, gridTool[i][j].EE, location);
					break;

				case SANDBOX_NOTETOOL:
					SANDBOX_drawNote(location);
					row1 = -99;
					SANDBOX_texts(gridTool[i][j].NN, gridTool[i][j].EE, gridTool[i][j].elev, location);
					break;

				case SANDBOX_WINDTOOL:
					SANDBOX_drawWind2(location, row1, row2, row3);
					strcpy(units, "m/s");
					if ((row1 == -99) || (row2 == -99) || (row3 == -99))
						row1 = -99;
					else	row1 = sqrt(pow(row1, 2) + pow(row2, 2) + pow(row3, 2));
					break;
				}

				/************************/
				/* determine text angle */
				/************************/
				if ((SANDBOX_G.headX - location[0]) == 0)
					ang = 0.0;
				else	ang = 90 - (180 / SANDBOX_PI * atan2((SANDBOX_G.headZ - location[2]), (SANDBOX_G.headX - location[0])));

				distAway = sqrt(pow((SANDBOX_G.headX - location[0]), 2) + pow((SANDBOX_G.headZ - location[2]), 2));

				if (distAway == 0)
					ang2 = 0.0;
				else	ang2 = -(180 / SANDBOX_PI * atan2((SANDBOX_G.headY - (location[1] + 0.5)), distAway));

				/****************************/
				/* show quantitative values */
				/****************************/
				if ((row1 > -99) && (gridTool[i][j].displayQuant == 1)) {
					sprintf(textRow, "%3.0f%s", row1, units);
					SANDBOX_drawText(ang, ang2, textRow, location[0], location[1] + 0.5, location[2]);
				}

				/*************************************/
				/* hilite site an empty wand is over */
				/*************************************/
				if (!SANDBOX_G.toolInHand) {
					if (SANDBOX_inSpecificRange (location[0], SANDBOX_G.wandy, location[2], SANDBOX_ACTIVEGRIDSIZE, SANDBOX_CAVESIZE, SANDBOX_ACTIVEGRIDSIZE)) {
#ifdef TOOGL_METHOD
						glCallList(SANDBOX_tyello); 	/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
						glEnable(GL_FRONT);
#else
						glColor4f(0.8, 0.8, 0.0, 0.5);	/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif
						glPushMatrix();			/* IGL: pushmatrix(); */
							glTranslatef(location[0], location[1], location[2]);		/* IGL: translate(); */
							glScalef(SANDBOX_ACTIVEGRIDSIZE, 0.12, SANDBOX_ACTIVEGRIDSIZE);	/* IGL: scale(); */
							SANDBOX_drawcube();
						glPopMatrix();			/* IGL: popmatrix(); */
					}
				}
			}

	/***************************/
	/* hilite the active sites */
	/***************************/
	if (SANDBOX_G.toolInHand && (SANDBOX_G.toolInHand != SANDBOX_SATTOOL)) {
		for (i = 0; (i < SANDBOX_MAXSITE) && (SANDBOX_legalSites[SANDBOX_G.toolInHand][i] >= 0); i++) {
			EEVal = siteData[SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].EE;
			NNVal = siteData[SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].NN;

			xmiddle = EEVal / (100 / SANDBOX_TMESHSIZE);
			ymiddle = NNVal / (100 / SANDBOX_TMESHSIZE);

			siteEE = (EEVal - 50) / 50.0 * SANDBOX_G.planeScale;
			siteNN = (NNVal - 50) / 50.0 * SANDBOX_G.planeScale;

			siteheight = siteData[SANDBOX_legalSites[SANDBOX_G.toolInHand][i]].elevation;

			if (SANDBOX_inSpecificRange(siteEE, SANDBOX_G.wandy, siteNN, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_CAVESIZE, SANDBOX_ACTIVEGRIDSIZE)) {
#ifdef TOOGL_METHOD
				glCallList(SANDBOX_tred); 			/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
				glEnable(GL_FRONT);
#else
				glColor4f(0.8, 0.1, 0.0, 0.6);			/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
#endif
			} else {
#ifdef TOOGL_METHOD
				glCallList(SANDBOX_tyello); 			/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
				glEnable(GL_FRONT);
#else
				glColor4f(0.8, 0.8, 0.0, 0.5);			/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif
			}

			tempElev = 0;
			tempCount = 0;
			for (k = xmiddle - 1; k <= xmiddle + 1; k++)
				for (l = ymiddle - 1; l <= ymiddle + 1; l++) {
					if ((k >= 0) && (k < SANDBOX_TMESHSIZE) && (l >= 0) && (l < SANDBOX_TMESHSIZE)) {
						tempElev += SANDBOX_finalElev[k][l];
						tempCount++;
					}
				}
			tempElev /= tempCount;


			if (SANDBOX_finalElev[xmiddle][ymiddle] > tempElev)
				tempElev = SANDBOX_finalElev[xmiddle][ymiddle];

			activeHeight = (tempElev - SANDBOX_G.averageElevation) / (float)(SANDBOX_G.maxElevation - SANDBOX_G.averageElevation) * SANDBOX_PLANEELEVATION + SANDBOX_G.planeY + 0.05;

			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(siteEE, activeHeight, siteNN);					/* IGL: translate(); */
				glScalef(SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE);/*IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */

		}
	}

	if (SANDBOX_G.instrumentMenu) {
		SANDBOX_toolOptions(SANDBOX_G.instrumentMenuTool, SANDBOX_G.instrumentMenuSite, SANDBOX_DRAW, SANDBOX_G.instrumentMenuX, SANDBOX_G.instrumentMenuY, SANDBOX_G.instrumentMenuZ);
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_getDataRow(int toolNumber, int siteNumber, int month, int date, int time)
{
	int			c, c2;
	int			q, r;
	int			flag;
	int			holdTime, holdMonth;
	struct SANDBOX_eachDay	*everyDay;
	struct SANDBOX_dailyData *oneDay;

#if 0
	FILE			*dataFile; /* commented out for SIGGRAPH */
#endif
	char			fileName[20];

	int			dataNN, dataEE, dataId, dataTime;
	float			dataDry, dataWet, dataRain, dataU, dataV, dataW;

	holdTime = time;
	holdMonth = month;

	/*****************/
	/* find the site */
	/*****************/
	q = siteNumber;
	everyDay = gridTool[toolNumber][q].month;

	/*****************************************/
	/* load in data for the day if necessary */
	/*****************************************/
	if (everyDay[date].ok == 0) {
#if 0
		everyDay[date].day = (struct SANDBOX_dailyData *) calloc(50, sizeof(struct SANDBOX_dailyData));
			  
		if (everyDay[date].day == NULL)
			SANDBOX_error("Could not allocate memory for data.", "SANDBOX_getDataRow 1");
#endif
		everyDay[date].ok = 1;

		oneDay = everyDay[date].day;

		for (c = 0; c < 50; c++) {
			oneDay[c].time = -1;
			oneDay[c].value = -1;
			oneDay[c].value2 = -1;
			oneDay[c].value3 = -1;
		}

#if 0
		/* Commented out for SIGGRAPH */
		sprintf(fileName,  "%s%02d%s", "SANDBOX_aug/",date,"-AUG-87");
		dataFile = fopen(fileName, "r");
		if (dataFile == NULL)
			SANDBOX_error("Cannot find data data file", "SANDBOX_getDataRow 2");
		fclose(dataFile);
#endif

		c2 = 0;
		flag = 1;

#if 0
		/* Commented out for SIGGRAPH */
		while(!feof(dataFile)) {
			fscanf(dataFile, "%d%d%f%f%f%f%f%f", &dataId, &dataTime, &dataDry, &dataWet, &dataRain, &dataU,  &dataV,  &dataW);
#endif

		for (r = 0; r < 480; r++) {
			dataId = SANDBOX_allMonthInfo[date][r].dataId;
			dataTime = SANDBOX_allMonthInfo[date][r].dataTime;
			dataDry = SANDBOX_allMonthInfo[date][r].dataDry;
			dataWet = SANDBOX_allMonthInfo[date][r].dataWet;
			dataRain = SANDBOX_allMonthInfo[date][r].dataRain;
			dataU = SANDBOX_allMonthInfo[date][r].dataU;
			dataV = SANDBOX_allMonthInfo[date][r].dataV;
			dataW = SANDBOX_allMonthInfo[date][r].dataW;

			dataNN = siteData[dataId].NN;
			dataEE = siteData[dataId].EE;

			if ((dataNN == gridTool[toolNumber][q].NN) && (dataEE == gridTool[toolNumber][q].EE)) {
				oneDay[c2].time = dataTime;
				switch (toolNumber) {
				case SANDBOX_THERMOMETERTOOL:
					oneDay[c2].value = dataDry;
					break;
				case SANDBOX_BEAKERTOOL:
					oneDay[c2].value = dataRain;
					break;
				case SANDBOX_WINDTOOL:
					oneDay[c2].value = dataU;
					oneDay[c2].value2 = dataV;
					oneDay[c2].value3 = dataW;
					break;
				case SANDBOX_CAMERATOOL:
					oneDay[c2].value = -99;
					break;
				}
				c2++;
			}
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_addToolToGrid(int siteEE, int siteNN, int elevation)
{
register int	q, r;

	for (q = 0; (gridTool[SANDBOX_G.toolInHand][q].EE >= 0) && (q < SANDBOX_MAXSITE); q++) ;	/* yes a ; */

	gridTool[SANDBOX_G.toolInHand][q].EE = siteEE;
	gridTool[SANDBOX_G.toolInHand][q].NN = siteNN;
	gridTool[SANDBOX_G.toolInHand][q].elev = elevation;

	gridTool[SANDBOX_G.toolInHand][q].displayGraph = 1;
	gridTool[SANDBOX_G.toolInHand][q].displaySound = 1;
	gridTool[SANDBOX_G.toolInHand][q].displayQuant = 1;

	if (SANDBOX_G.toolInHand == SANDBOX_THERMOMETERTOOL) {
		gridTool[SANDBOX_G.toolInHand][q].max = 30;
		gridTool[SANDBOX_G.toolInHand][q].min = 20;
	}

	if (SANDBOX_G.toolInHand == SANDBOX_BEAKERTOOL) {
		gridTool[SANDBOX_G.toolInHand][q].max = 60;
		gridTool[SANDBOX_G.toolInHand][q].min = 0;
	}

	if (SANDBOX_G.toolInHand == SANDBOX_WINDTOOL) {
		gridTool[SANDBOX_G.toolInHand][q].max = 10;
		gridTool[SANDBOX_G.toolInHand][q].min = 0;
	}

#if 0
	/* COMMENTED OUT FOR SIGGRAPH */

	/*******************************************/
	/* initiallize storage for 31 days of data */
	/* no data is stored in any of the days    */
	/*******************************************/
	gridTool[SANDBOX_G.toolInHand][q].month = (struct SANDBOX_eachDay *)calloc(32, sizeof(struct SANDBOX_eachDay));

	if (gridTool[SANDBOX_G.toolInHand][q].month == NULL)
		SANDBOX_error("Cannot allocate memory for data", "SANDBOX_addToolToGrid");

	for(r=0;r<32;r++) {
		gridTool[SANDBOX_G.toolInHand][q].month[r].day = NULL;
		gridTool[SANDBOX_G.toolInHand][q].month[r].ok = 0;
	}
#endif
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_removeToolFromGrid(int toolNumber, int siteNumber)
{
register int	j, k;

	gridTool[toolNumber][siteNumber].EE = -1;
	gridTool[toolNumber][siteNumber].NN = -1;
	gridTool[toolNumber][siteNumber].elev = -1;
	gridTool[toolNumber][siteNumber].counter = 0;
	gridTool[toolNumber][siteNumber].nowData = 0;
	gridTool[toolNumber][siteNumber].nowData2 = 0;
	gridTool[toolNumber][siteNumber].nowData3 = 0;

	gridTool[toolNumber][siteNumber].displayGraph = -1;
	gridTool[toolNumber][siteNumber].displaySound = -1;
	gridTool[toolNumber][siteNumber].displayQuant = -1;

	/*    gridTool[toolNumber][siteNumber].month = NULL; */
	for (k = 0; k < SANDBOX_GRAPHMAX; k++)
		gridTool[toolNumber][siteNumber].lastX[k] = -99;

	for (j = 0; j < 32; j++) {
		gridTool[toolNumber][siteNumber].month[j].ok = 0;
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_skyUpdate()
{
	float		radball;
	float		sphereDat[4];
	float		satDat[3];
	float		heloDat[3];
	float		selectDat[3];
	float		timeOfSky;
	int		sunSelected, moonSelected;

	/*****************************************/
	/* is the user playing with the sun/moon */
	/*****************************************/
	timeOfSky = (2 * SANDBOX_PI * SANDBOX_G.timeOfDay) + (SANDBOX_PI / 2);

	if (SANDBOX_G.dateSelected) {
		radball = 0.2;
		sphereDat[2] = SANDBOX_G.planeZ - SANDBOX_SUNZ;
		sphereDat[3] = radball;

		sphereDat[0] = -cos(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeX;
		sphereDat[1] = -sin(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeY;

		sunSelected = 0;
		if (SANDBOX_inRange(sphereDat[0], sphereDat[1], sphereDat[2], 2 * radball)) {
			sunSelected = 1;
			selectDat[0] = sphereDat[0];
			selectDat[1] = sphereDat[1];
			selectDat[2] = sphereDat[2];
		}

		sphereDat[0] = cos(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeX;
		sphereDat[1] = sin(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeY;

		moonSelected = 0;
		if (SANDBOX_inRange(sphereDat[0], sphereDat[1], sphereDat[2], 2 * radball)) {
			moonSelected = 1;
			selectDat[0] = sphereDat[0];
			selectDat[1] = sphereDat[1];
			selectDat[2] = sphereDat[2];
		}

		if (sunSelected || moonSelected)
			SANDBOX_G.smoonSelected = 1;
		else	SANDBOX_G.smoonSelected = 0;

		if (SANDBOX_G.smoonSelected || SANDBOX_G.sunOptions) {
			if ((SANDBOX_G.optionButton == ON) && !SANDBOX_G.forceOptionUp && !SANDBOX_G.sunOptions) {
				SANDBOX_G.sunOptions = 1;
				SANDBOX_G.sunMenuDat[0] = selectDat[0];
				SANDBOX_G.sunMenuDat[1] = selectDat[1];
				SANDBOX_G.sunMenuDat[2] = selectDat[2];

				SANDBOX_beepit();
				SANDBOX_G.forceOptionUp = 1;
			}

			if ((SANDBOX_G.optionButton == ON) && !SANDBOX_G.forceOptionUp && SANDBOX_G.sunOptions) {
				SANDBOX_G.sunOptions = 0;
				SANDBOX_beepit();
				SANDBOX_G.forceOptionUp = 1;
			}
		}
	}

	/******************************************/
	/* is the user playing with the satellite */
	/******************************************/
	satDat[0] = CENTERX;
	satDat[1] = CENTERY + 0.5;
	satDat[2] = CENTERZ - 0.5;

	if ((SANDBOX_G.toolInHand == SANDBOX_SATTOOL) && (SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp && SANDBOX_inRange(satDat[0], satDat[1], satDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
		SANDBOX_G.satInSky = 1;
		SANDBOX_beepit();
		SANDBOX_G.toolInHand = SANDBOX_NOTOOL;
		SANDBOX_G.forceSelectUp = 1;
	}

	if ((!SANDBOX_G.toolInHand) && SANDBOX_G.satInSky && (SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp && SANDBOX_inRange(satDat[0], satDat[1], satDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
		SANDBOX_G.satInSky = 0;
		SANDBOX_G.satteliteOptions = 0;
		SANDBOX_beepit();
		SANDBOX_G.toolInHand = SANDBOX_SATTOOL;
		SANDBOX_G.forceSelectUp = 1;
	}

	if (SANDBOX_G.satInSky && SANDBOX_inRange(satDat[0], satDat[1], satDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
		if ((SANDBOX_G.optionButton == ON) && !SANDBOX_G.forceOptionUp && !SANDBOX_G.satteliteOptions) {
			SANDBOX_G.satteliteOptions = 1;
			SANDBOX_beepit();
			SANDBOX_G.forceOptionUp = 1;
		}
		if ((SANDBOX_G.optionButton == ON) && !SANDBOX_G.forceOptionUp && SANDBOX_G.satteliteOptions) {
			SANDBOX_G.satteliteOptions = 0;
			SANDBOX_beepit();
			SANDBOX_G.forceOptionUp = 1;
		}
	}

	/*******************************************/
	/* is the user playing with the helicopter */
	/*******************************************/
	heloDat[0] = CENTERX;
	heloDat[1] = CENTERY + 0.0;
	heloDat[2] = CENTERZ - 0.5;

	if ((SANDBOX_G.toolInHand == SANDBOX_HELOTOOL) && (SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp && SANDBOX_inRange(heloDat[0], heloDat[1], heloDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
		SANDBOX_G.heloInSky = 1;
		SANDBOX_beepit();
		SANDBOX_G.toolInHand = SANDBOX_NOTOOL;
		SANDBOX_G.forceSelectUp = 1;
	}

	if ((!SANDBOX_G.toolInHand) && SANDBOX_G.heloInSky && (SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp && SANDBOX_inRange(heloDat[0], heloDat[1], heloDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
		SANDBOX_G.heloInSky = 0;
		SANDBOX_beepit();
		SANDBOX_G.toolInHand = SANDBOX_HELOTOOL;
		SANDBOX_G.forceSelectUp = 1;
	}

	/*********/
	/* menus */
	/*********/
	if (SANDBOX_G.satInSky && SANDBOX_G.satteliteOptions)
		SANDBOX_satOptions(SANDBOX_DO, satDat[0], satDat[1], satDat[2]);

	if (SANDBOX_G.sunOptions)
		SANDBOX_sunmoonOptions(SANDBOX_DO, SANDBOX_G.sunMenuDat[0], SANDBOX_G.sunMenuDat[1], SANDBOX_G.sunMenuDat[2]);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_sky()
{
	float		radball;
	float		sphereDat[4];
	float		selectDat[3];
	float		center[3];
	float		timeOfSky;

	timeOfSky = (2 * SANDBOX_PI * SANDBOX_G.timeOfDay) + (SANDBOX_PI / 2);

	if (SANDBOX_G.dateSelected) {
		radball = 0.2;
		sphereDat[2] = sphereDat[2] = SANDBOX_G.planeZ - SANDBOX_SUNZ;
		sphereDat[3] = radball;

		/****************/
		/* draw the sun */
		/****************/
		SANDBOX_DisableTransparency();

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_gtyello); 					/* IGL: lmbind(MATERIAL, SANDBOX_gtyello); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.5, 0.5, 0.0, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_gtyello); -- NOTE: also supposed to be emmisive 0.9, 0.9, 0.0 */
#endif

		sphereDat[0] = -cos(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeX;
		sphereDat[1] = -sin(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeY;
#if  0
		sphdraw(sphereDat);	/* Part of the libsphere option for IRISgl */
#else
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);			/* IGL: translate(); */
			WRS_drawsphere();							/* IGL: sphdraw(sphereDat); */
		glPopMatrix();							/* IGL: popmatrix(); */
#endif

		SANDBOX_EnableTransparency();

		if (SANDBOX_inRange(sphereDat[0], sphereDat[1], sphereDat[2], 2 * radball)) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);		/* IGL: translate(); */
				glScalef(2 * radball, 2 * radball, 2 * radball);		/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}

		/*****************/
		/* draw the moon */
		/*****************/
		SANDBOX_DisableTransparency();
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_moon); 					/* IGL: lmbind(MATERIAL, SANDBOX_moon); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.8, 0.8, 0.8, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_moon); */
#endif

		sphereDat[0] = cos(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeX;
		sphereDat[1] = sin(timeOfSky) * SANDBOX_G.planeScale + SANDBOX_G.planeY;
#if 0
		sphdraw(sphereDat);
#else
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);			/* IGL: translate(); */
			WRS_drawsphere();							/* IGL: sphdraw(sphereDat); */
		glPopMatrix();							/* IGL: popmatrix(); */
#endif

		SANDBOX_EnableTransparency();

		if (SANDBOX_inRange(sphereDat[0], sphereDat[1], sphereDat[2], 2 * radball)) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);		/* IGL: translate(); */
				glScalef(2 * radball, 2 * radball, 2 * radball);		/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}

		if (SANDBOX_G.sunOptions)
			SANDBOX_sunmoonOptions(SANDBOX_DRAW, SANDBOX_G.sunMenuDat[0], SANDBOX_G.sunMenuDat[1], SANDBOX_G.sunMenuDat[2]);
	}

	/*********************/
	/* draw the satellite */
	/*********************/
	sphereDat[0] = CENTERX;
	sphereDat[1] = CENTERY + 0.5;
	sphereDat[2] = CENTERZ - 0.5;

	if (SANDBOX_G.satInSky) {
		SANDBOX_drawSat(sphereDat);
	}

	/******************************/
	/* hilite the active sat site */
	/******************************/
	if (SANDBOX_G.toolInHand == SANDBOX_SATTOOL) {
		if (SANDBOX_inRange (sphereDat[0], sphereDat[1], sphereDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tred); 				/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.1, 0.0, 0.6);				/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
#endif
		} else {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif
		}

		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);				/* IGL: translate(); */
			glScalef(SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE);/*IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */
	}

	if ((!SANDBOX_G.toolInHand) && SANDBOX_G.satInSky)
		if (SANDBOX_inRange (sphereDat[0], sphereDat[1], sphereDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);					/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif
			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);				/* IGL: translate(); */
				glScalef(SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE);/*IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}

	if (SANDBOX_G.satInSky && SANDBOX_G.satteliteOptions)
		SANDBOX_satOptions(SANDBOX_DRAW, sphereDat[0], sphereDat[1], sphereDat[2]);

	/***********************/
	/* draw the helicopter */
	/***********************/
	sphereDat[0] = CENTERX;
	sphereDat[1] = CENTERY + 0.0;
	sphereDat[2] = CENTERZ - 0.5;

	center[0] = 0;		/* needed to rotate the helicopter */
	center[1] = 0;
	center[2] = 0;

	if (SANDBOX_G.heloInSky) {
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);			/* IGL: translate(); */
			glRotatef(90, 0.0, 1.0, 0.0);						/* IGL: rot(90, 'y'); */
			glBegin(GL_LINE_STRIP);
			glVertex3f(0, 0, 0);
			glEnd();								/* IGL: move(0, 0, 0); -- TODO: consider using GL_LINES or GL_POINS types */
			SANDBOX_drawHelo(center);
		glPopMatrix();							/* IGL: popmatrix(); */
	}

	/*******************************/
	/* hilite the active helo site */
	/*******************************/
	if (SANDBOX_G.toolInHand == SANDBOX_HELOTOOL) {
		if (SANDBOX_inRange (sphereDat[0], sphereDat[1], sphereDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tred); 				/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.1, 0.0, 0.6);				/* IGL: lmbind(MATERIAL, SANDBOX_tred); */
#endif
		} else {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif
		}

		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);				/* IGL: translate(); */
			glScalef(SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE);/*IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */
	}

	if ((!SANDBOX_G.toolInHand) && SANDBOX_G.heloInSky) {
		if (SANDBOX_inRange (sphereDat[0], sphereDat[1], sphereDat[2], SANDBOX_ACTIVEGRIDSIZE)) {
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_tyello); 				/*IGL: lmbind(MATERIAL, SANDBOX_tyello); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.8, 0.8, 0.0, 0.5);				/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif
			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(sphereDat[0], sphereDat[1], sphereDat[2]);				/* IGL: translate(); */
				glScalef(SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE, SANDBOX_ACTIVEGRIDSIZE);/*IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_sunmoonOptions(int doOrDraw, float x, float y, float z)
{
register int	i;
	float	buttonx, buttony, buttonz;
	float	menuLoc[3];
	float	vertex[3];
	int	oldWidth;

	menuLoc[0] = x + 0.6;
	menuLoc[1] = y;
	menuLoc[2] = z;

	buttonx = menuLoc[0];
	buttonz = menuLoc[2] + 0.02;

	if (doOrDraw == SANDBOX_DO) {
		for (i = -1; i <= 1; i++) {
			buttony = i * 0.15 + menuLoc[1];
			if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
				if ((SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp) {
					if (i == -1) {
						SANDBOX_G.framesPerDay *= 2;
						SANDBOX_G.timeCounter *= 2;
					}
					if (i == 0) ;		/* BS: what is this all about? */
					if (i == 1) {
						SANDBOX_G.framesPerDay /= 2;
						SANDBOX_G.timeCounter /= 2;
					}

					SANDBOX_G.forceSelectUp = 1;
					SANDBOX_beepit();
				}
			}
		}
	} else {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0], menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.12, 0.3, 0.05);						/* IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0] - .12, menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.48, 0.3, 0.05);						/* IGL: scale(); */
			glRotatef(90, 0.0, 0.0, 1.0);						/* IGL: rot(90, 'z'); */
			SANDBOX_drawpyramid();
		glPopMatrix();	/*						 IGL: popmatrix(); */

		int	tmp;	/* used for call to glGetIntegerv() */
		oldWidth = (int)(glGetIntegerv(GL_LINE_WIDTH, &tmp), tmp);	/* IGL: oldWidth = (int)getlwidth(); */
		glLineWidth((GLfloat)(3));					/* IGL: linewidth(3); */

		for (i = -1; i <= 1; i++) {
			buttony = i * 0.15 + menuLoc[1];
#ifdef TOOGL_METHOD
			glCallList(SANDBOX_grey);				/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.4, 0.4, 0.42, 1.0);				/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(buttonx, buttony, buttonz);				/* IGL: translate(); */
				glScalef(SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE);	/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */

			vertex[2] = buttonz + SANDBOX_BUTTONSIZE + 0.001;

			switch (i) {
			case -1:
				SANDBOX_drawMenuText(">>", buttonx, buttony, vertex[2]);
				break;

			case 0:	/* reverse (eventually) */
				break;
			case 1:
				SANDBOX_drawMenuText("<<", buttonx, buttony, vertex[2]);
				break;
			}

			if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
#ifdef TOOGL_METHOD
				glCallList(SANDBOX_tyello); 			/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
				glEnable(GL_FRONT);
#else
				glColor4f(0.8, 0.8, 0.0, 0.5);			/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

				glPushMatrix();					/* IGL: pushmatrix(); */
					glTranslatef(buttonx, buttony, buttonz);		/* IGL: translate(); */
					glScalef(SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001);	/* IGL: scale(); */
					SANDBOX_drawcube();
				glPopMatrix();					/* IGL: popmatrix(); */
			}

		}
		glLineWidth((GLfloat)(oldWidth));				/* IGL: linewidth(oldWidth); */
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_satOptions(int doOrDraw, float x, float y, float z)
{
register int	i;
	float	buttonx, buttony, buttonz;
	float	letterZ;
	float	menuLoc[3];
	char	menu[4];
	int	border = 0;

	menuLoc[0] = x + 1.1;
	menuLoc[1] = y;
	menuLoc[2] = z;

	buttony = menuLoc[1];
	buttonz = menuLoc[2] + 0.1;
	letterZ = buttonz + SANDBOX_BUTTONSIZE + 0.001;

	if (doOrDraw == SANDBOX_DO) {
		for (i = -4; i <= 4; i++) {
			buttonx = i * 0.16 + menuLoc[0];

			if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
				if ((SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp) {
					switch (i) {
					case -4:
						SANDBOX_G.picture = SANDBOX_G.picture0;
						break;
					case -3:
						break;
					case -2:
						SANDBOX_G.picture = SANDBOX_G.picture1;
						break;
					case -1:
						SANDBOX_G.picture = SANDBOX_G.picture2;
						break;
					case -0:
						SANDBOX_G.picture = SANDBOX_G.picture3;
						break;
					case 1:
						SANDBOX_G.picture = SANDBOX_G.picture4;
						break;
					case 2:
						SANDBOX_G.picture = SANDBOX_G.picture5;
						break;
					case 3:
						SANDBOX_G.picture = SANDBOX_G.picture6;
						break;
					case 4:
						SANDBOX_G.picture = SANDBOX_G.picture7;
						break;
					}
#ifdef TOOGL_METHOD
					glNewList(GLLIST_24, GL_COMPILE);	/* IGL: ... */
					glTexImage2D(GL_TEXTURE_2D, 0, 1, SANDBOX_G.heightOfImage, SANDBOX_G.widthOfImage, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.picture);	/* IGL: ... */
					glEndList();				/* IGL: ... */
#else
					glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_1);	/* IGL: ... */
					glTexImage2D(GL_TEXTURE_2D, 0, 1, SANDBOX_G.heightOfImage, SANDBOX_G.widthOfImage, border, GL_LUMINANCE, GL_UNSIGNED_BYTE, (char *)SANDBOX_G.picture);	/* IGL: ... */
					glBindTexture(GL_TEXTURE_2D, 0);	/* IGL: ... */
#endif

					SANDBOX_G.forceSelectUp = 1;
					SANDBOX_beepit();
				}
			}
		}
	} else {
		/* draw */

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0], menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.8, 0.12, 0.05);						/* IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0] - 0.8, menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.3, 0.12, 0.05);						/* IGL: scale(); */
			glRotatef(90, 0.0, 0.0, 1.0);						/* IGL: rot(90, 'z'); */
			SANDBOX_drawpyramid();
		glPopMatrix();							/* IGL: popmatrix(); */

		for (i = -4; i <= 4; i++) {
			buttonx = i * 0.16 + menuLoc[0];

#ifdef TOOGL_METHOD
			glCallList(SANDBOX_grey); 				/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.4, 0.4, 0.42, 1.0);				/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

			if (i != -3) {
				glPushMatrix();					/* IGL: pushmatrix(); */
					glTranslatef(buttonx, buttony, buttonz);				/* IGL: translate(); */
					glScalef(SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE);	/* IGL: scale(); */
					SANDBOX_drawcube();
				glPopMatrix();					/* IGL: popmatrix(); */

				if (i == -4)
					sprintf(menu, "%1d", 0);
				else	sprintf(menu, "%1d", i + 3);

				SANDBOX_drawMenuText(menu, buttonx, buttony, letterZ);

				if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
#ifdef TOOGL_METHOD
					glCallList(SANDBOX_tyello); 		/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
					glEnable(GL_FRONT);
#else
					glColor4f(0.8, 0.8, 0.0, 0.5);		/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

					glPushMatrix();				/* IGL: pushmatrix(); */
						glTranslatef(buttonx, buttony, buttonz);	/* IGL: translate(); */
						glScalef(SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001);	/* IGL: scale(); */
						SANDBOX_drawcube();
					glPopMatrix();				/* IGL: popmatrix(); */
				}
			}
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_planeOptions(int doOrDraw, float x, float y, float z)
{
register int	i;
	float	buttonx, buttony, buttonz;
	float	menuLoc[3];
	float	vertex[3];
	int	oldWidth;

	menuLoc[0] = x + 0.6;
	menuLoc[1] = y;
	menuLoc[2] = z;

	buttonx = menuLoc[0];
	buttonz = menuLoc[2] + 0.02;

	if (doOrDraw == SANDBOX_DO) {
		for (i = -2; i <= 2; i++) {
			buttony = i * 0.15 + menuLoc[1];
			if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
				if ((SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp) {
					if (i == -2)
						SANDBOX_G.planeY -= 0.5;
					if (i == -1)
						SANDBOX_G.planeScale--;
					if (i == 0)
						SANDBOX_G.showGrid = !SANDBOX_G.showGrid;
					if (i == 1)
						SANDBOX_G.planeScale++;
					if (i == 2)
						SANDBOX_G.planeY += 0.5;

					SANDBOX_G.forceSelectUp = 1;
					SANDBOX_beepit();
				}
			}
		}
	} else {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0], menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.12, 0.5, 0.05);						/* IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0] - .12, menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.48, 0.5, 0.05);						/* IGL: scale(); */
			glRotatef(90, 0.0, 0.0, 1.0);						/* IGL: rot(90, 'z'); */
			SANDBOX_drawpyramid();
		glPopMatrix();							/* IGL: popmatrix(); */

		int	tmp;	/* for usin with glGetIntegerv() */
		oldWidth = (int)(glGetIntegerv(GL_LINE_WIDTH, &tmp), tmp);	/* IGL: oldWidth = (int)getlwidth(); */
		glLineWidth((GLfloat)(2));					/* IGL: linewidth(3); */

		for (i = -2; i <= 2; i++) {
			buttony = i * 0.15 + menuLoc[1];

#ifdef TOOGL_METHOD
			glCallList(SANDBOX_grey); 				/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
			glEnable(GL_FRONT);
#else
			glColor4f(0.4, 0.4, 0.42, 1.0);				/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

			glPushMatrix();						/* IGL: pushmatrix(); */
				glTranslatef(buttonx, buttony, buttonz);				/* IGL: translate(); */
				glScalef(SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE);	/* IGL: scale(); */
				SANDBOX_drawcube();
			glPopMatrix();						/* IGL: popmatrix(); */

			vertex[2] = buttonz + SANDBOX_BUTTONSIZE + 0.001;

			switch (i) {
			case -2:	/* lower v */
				SANDBOX_drawMenuText("v", buttonx, buttony, vertex[2]);
				break;
			case -1:	/* shrink >< */
				SANDBOX_drawMenuText("><", buttonx, buttony, vertex[2]);
				break;
			case 0:		/* grid # */
				SANDBOX_drawMenuText("#", buttonx, buttony, vertex[2]);
				break;
			case 1:		/* expand <> */
				SANDBOX_drawMenuText("<>", buttonx, buttony, vertex[2]);
				break;
			case 2:		/* raise ^ */
				SANDBOX_drawMenuText("^", buttonx, buttony, vertex[2]);
				break;
			}

			if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
#ifdef TOOGL_METHOD
				glCallList(SANDBOX_tyello); 			/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
				glEnable(GL_FRONT);
#else
				glColor4f(0.8, 0.8, 0.0, 0.5);			/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

				glPushMatrix();					/* IGL: pushmatrix(); */
					glTranslatef(buttonx, buttony, buttonz);							/* IGL: translate(); */
					glScalef(SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001);	/* IGL: scale(); */
					SANDBOX_drawcube();
				glPopMatrix();					/* IGL: popmatrix(); */
			}

			glLineWidth((GLfloat)(oldWidth));			/* IGL: linewidth(oldWidth); */
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawText(float angle, float angle2, char *thaText, float locX, float locY, float locZ)
{
	int	oldLineWidth;

	SANDBOX_DisableTransparency();

	htextsize(0.2, 0.2);

	hcentertext(1);

	int	tmp;	/* for use with glGetIntegerv() */
	oldLineWidth = (int)(glGetIntegerv(GL_LINE_WIDTH, &tmp), tmp);		/* IGL: oldLineWidth = (int)getlwidth(); */

	/****************/
	/* draw outline */
	/****************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black); 						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glColor4f(0.5, 0.5, 0.0, 1.0);	/* temp: */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(locX, locY, locZ);							/* IGL: translate(); */
		glRotatef(angle, 0.0, 1.0, 0.0);						/* IGL: rot(angle, 'y'); */
		glRotatef(angle2, 1.0, 0.0, 0.0);						/* IGL: rot(angle2, 'x'); */
		glBegin(GL_LINE_STRIP);
		glVertex3f(0, 0, 0);
		glEnd();									/* IGL: move(0, 0, 0);  -- TODO: consider GL_LINES and/or GL_POINTS types */
		glLineWidth((GLfloat)(6));							/* IGL: linewidth(6); */
		hcharstr(thaText);
	glPopMatrix();								/* IGL: popmatrix(); */

	/****************/
	/* draw message */
	/****************/
#ifdef TOOGL_METHOD
	glCallList(SANDBOX_gtyello); 						/* IGL: lmbind(MATERIAL, SANDBOX_gtyello); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.5, 0.5, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_gtyello); -- NOTE: also supposed to be emmisive 0.9, 0.9, 0.0 */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(locX, locY, locZ);							/* IGL: translate(); */
		glRotatef(angle, 0.0, 1.0, 0.0);						/* IGL: rot(angle, 'y'); */
		glRotatef(angle2, 1.0, 0.0, 0.0);						/* IGL: rot(angle2, 'x'); */
		glBegin(GL_LINE_STRIP);
		glVertex3f(0, 0, +0.005);
		glEnd();									/* IGL: move(0, 0, +0.005); -- TODO: consider GL_LINES and/or GL_POINTS types */
		glLineWidth((GLfloat)(2));							/* IGL: linewidth(2); */
		hcharstr(thaText);
	glPopMatrix();								/* IGL: popmatrix(); */

	glLineWidth((GLfloat)(oldLineWidth));					/* IGL: linewidth(oldLineWidth); */

	SANDBOX_EnableTransparency();
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_drawMenuText(char *thaText, float locX, float locY, float locZ)
{
	int	oldLineWidth;
	int	tmp;			/* for use with call to glGetIntegerv() */

	oldLineWidth = (int)(glGetIntegerv(GL_LINE_WIDTH, &tmp), tmp);		/* IGL: oldLineWidth = (int)getlwidth(); */

	hcentertext(1);
	htextsize(0.1, 0.1);

	SANDBOX_DisableTransparency();

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black); 						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);							/* IGL: lmbind(MATERIAL, SANDBOX_black); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glBegin(GL_LINE_STRIP);
		glVertex3f(locX, locY, locZ);
		glEnd();									/* IGL: move(locX,  locY, locZ); -- TODO: consider GL_LINES and/or GL_POINTS types */
		glLineWidth((GLfloat)(4));							/* IGL: linewidth(4); */
		hcharstr(thaText);
	glPopMatrix();								/* IGL: popmatrix(); */

	SANDBOX_EnableTransparency();

	glLineWidth((GLfloat)(oldLineWidth));					/* IGL: linewidth(oldLineWidth); */
}


/* misc notes

	eventual needs:
	turning, moving the plane
	more 3d information - wind at altitude, helicopters, etc.
	moving the menus
	distributed:
		have file for each user of system with some kind of mod bit
		every n time units check mod bit, if set re-do those instruments

	the actual data acquisition could be done by a seperate process for the
		full set. I still have to vizualize the current state of the
		tools though.
*/

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_holidaySnaps(int North, int East, float *location)
{
	float	cube_normals[3] = { 0.0, 0.0, 1.0 };
	float	cube[4][3] = {
			0.0, 1.0, 0.0,
			0.0, 0.0, 0.0,
			1.0, 0.0, 0.0,
			1.0, 1.0, 0.0
		};
	float	ang;
	float	splane[4] = { 1, 0, 0, 0 };
	float	tplane[4] = { 0, 1, 0, 0 };
	int	border = 0;


	if (SANDBOX_pics[North * 100 + East] != NULL) {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_bwhite); 					/* IGL: lmbind(MATERIAL, SANDBOX_bwhite); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.9, 0.9, 0.9, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_bwhite); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			SANDBOX_DisableTransparency();
			glColor4fv(SANDBOX_whiteColour);					/* IGL: c4f(SANDBOX_whiteColour); */

#ifdef TOOGL_METHOD
			glNewList(10 + North * 100 + East, GL_COMPILE);
#  ifdef PO2_TEXTURES
			glTexImage2D(GL_TEXTURE_2D, 0, 1, 256, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[North * 100 + East]);
#  else
			glTexImage2D(GL_TEXTURE_2D, 0, 1, 384, 256, border, GL_RGBA, GL_UNSIGNED_BYTE, (char *)SANDBOX_pics[North * 100 + East]);
#  endif
			glEndList();								/* IGL: texbind(TX_TEXTURE_0, 10+North*100+East); */
			glCallList(10 + North * 100 + East);					/* IGL: texbind(TX_TEXTURE_0, 10+North*100+East); */
			glCallList(1); 								/* IGL: tevbind(TV_ENV0, 1); */
			glEnable(GL_TEXTURE_2D);
#else
			switch (North * 100 + East) {
			case 1463:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_1463);
				break;
			case 2328:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_2328);
				break;
			case 3120:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_3120);
				break;
			case 4338:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_4338);
				break;
			case 6468:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_6468);
				break;
			case 6911:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_6911);
				break;
			case 8639:
				glBindTexture(GL_TEXTURE_2D, GL_TEXLIST_8639);
				break;
			default:
				break;
			}
			glEnable(GL_TEXTURE_2D);
#endif

#ifdef TOOGL_METHOD
#if 0 /* new for v3 -- 05/26/2006: this was wrong GL_EYE_LINEAR is not valid for the 2nd argument (so I'm trying GL_EYE_PLANE) */
			glTexGenfv(GL_S, GL_EYE_LINEAR, splane);				/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_LINEAR, tplane);				/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#else
			glTexGenfv(GL_S, GL_EYE_PLANE, splane);					/* IGL: texgen(TX_S,TG_LINEAR,splane); */
			glTexGenfv(GL_T, GL_EYE_PLANE, tplane);					/* IGL: texgen(TX_T,TG_LINEAR,tplane); */
#endif
#else
			/* TODO: this (or determine that it's not necessary) */
#endif

#if 0	/* Enabling the auto texture generation is causing problems */
#ifdef TOOGL_METHOD
			/* TODO: add glEnable(GL_TEXTURE_GEN_S); & glEnable(GL_TEXTURE_GEN_T); as per toogl instructions */
			glTexGenfv(GL_S, XXX_TG_ON, NULL);					/* IGL: texgen(TX_S, TG_ON, NULL); */
			glTexGenfv(GL_T, XXX_TG_ON, NULL);					/* IGL: texgen(TX_T, TG_ON, NULL); */
#else
			glEnable(GL_TEXTURE_GEN_S);
			glEnable(GL_TEXTURE_GEN_T);
#endif
#endif

			glTranslatef(location[0], location[1] + 0.3, location[2]);		/* IGL: translate(); */

			if ((SANDBOX_G.headX - location[0]) == 0)
				ang = 0.0;
			else	ang = 90 - (180 / SANDBOX_PI * atan2((SANDBOX_G.headZ - location[2]), (SANDBOX_G.headX - location[0])));

			glRotatef(ang, 0.0, 1.0, 0.0);						/* IGL: rot(ang, 'y'); */
			glScalef(1.5, 1.0, 0.01);						/* IGL: scale(); */
			glTranslatef(-0.5, 0.0, 0.0);						/* IGL: translate(); */

			/* NOTE: split into multi-line and added the texture coordinates to what toogl provided */
			glBegin(GL_POLYGON);							/* IGL: bgnpolygon(); -- TODO: consider GL_QUADS type */
			glNormal3fv(cube_normals);							/* IGL: n3f(cube_normals); */
			glTexCoord2f(0.0, 1.0);								/* IGL: no-op -- used auto texturing */
			glVertex3fv(cube[0]);								/* IGL: v3f(cube[0]); */
			glTexCoord2f(0.0, 0.0);								/* IGL: no-op -- used auto texturing */
			glVertex3fv(cube[1]);								/* IGL: v3f(cube[1]); */
			glTexCoord2f(1.0, 0.0);								/* IGL: no-op -- used auto texturing */
			glVertex3fv(cube[2]);								/* IGL: v3f(cube[2]); */
			glTexCoord2f(1.0, 1.0);								/* IGL: no-op -- used auto texturing */
			glVertex3fv(cube[3]);								/* IGL: v3f(cube[3]); */
			glEnd();								/* IGL: endpolygon(); */
			glDisable(GL_TEXTURE_2D);						/* IGL: texbind(TX_TEXTURE_0, 0); */
			SANDBOX_EnableTransparency();
		glPopMatrix();							/* IGL: popmatrix(); */
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_texts(int North, int East, int Height, float *location)
{
	char	textRow[10];

	htextsize(0.15, 0.15);
	hcentertext(0);
	hfixedwidth(0);
	glLineWidth((GLfloat)(1));						/* IGL: linewidth(1); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_white); 						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
	glEnable(GL_FRONT);
#else
	glColor4f(1.0, 1.0, 1.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_white); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(location[0], location[1] + 0.55, location[2]);			/* IGL: translate(); */
		glScalef(0.4, 0.3, 0.01);							/* IGL: scale(); */
		SANDBOX_drawcube();
	glPopMatrix();								/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
	glCallList(SANDBOX_black); 						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
	glEnable(GL_FRONT);
#else
	glColor4f(0.0, 0.0, 0.0, 1.0);						/* IGL: lmbind(MATERIAL, SANDBOX_black); */
#endif

	glPushMatrix();								/* IGL: pushmatrix(); */
		glTranslatef(location[0] - 0.35, location[1] + 0.65, location[2] + 0.02);	/* IGL: translate(); */
		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();									/* IGL: move(0, 0, 0); -- TODO: consider GL_LINES and/or GL_POINTS types */
		sprintf(textRow, "%s%02d", "East:  ", East);
		hcharstr(textRow);
		printf("toprint %s\n", textRow);
		glTranslatef(0, -0.2, 0);							/* IGL: translate(); */
		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();									/* IGL: move(0, 0, 0); -- TODO: consider GL_LINES and/or GL_POINTS types */
		sprintf(textRow, "%s%02d", "South:", North);
		hcharstr(textRow);
		glTranslatef(0, -0.2, 0);							/* IGL: translate(); */
		glBegin(GL_LINE_STRIP);
		glVertex3f(0.0, 0.0, 0.0);
		glEnd();									/* IGL: move(0, 0, 0); -- TODO: consider GL_LINES and/or GL_POINTS types */
		sprintf(textRow, "%s%3d", "Elev: ", Height);
		hcharstr(textRow);
	glPopMatrix();								/* IGL: popmatrix(); */
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_loadSitePic(int NN, int EE)
{
#ifdef PO2_TEXTURES
	int	width = 256, height = 256;
#else
	int	width = 384, height = 256;
#endif
	int	x, y;
	FILE	*imageFile;
	char	filename[50];

#ifdef PO2_TEXTURES
	sprintf(filename, "pics/site%4d_pot.bw", NN * 100 + EE);
#else
	sprintf(filename, "pics/site%4d.bw", NN * 100 + EE);
#endif
	imageFile = fopen(filename, "r");
	if (imageFile == NULL)
		SANDBOX_error("Cannot find image file", filename);

	SANDBOX_pics[NN * 100 + EE] = (char *)malloc(width * height);

	if (SANDBOX_pics[NN * 100 + EE] == NULL)
		SANDBOX_error("Cannot allocate memory for image", "SANDBOX_loadSitePic");

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
			SANDBOX_pics[NN * 100 + EE][(height - 1 - y) * width + (width - 1 - x)] = (char)fgetc(imageFile);

	fclose(imageFile);
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

void SANDBOX_toolOptions(int tool, int site, int doOrDraw, float x, float y, float z)
{
register int	i, j;
	float	buttonx, buttony, buttonz;
	float	menuLoc[3];
	float	vertex[3];
	int	oldWidth;
	char	maxamount[4], minamount[4];

	menuLoc[0] = x - 0.6;
	menuLoc[1] = y;
	menuLoc[2] = z;

	buttonx = menuLoc[0];
	buttonz = menuLoc[2] + 0.02;

	sprintf(maxamount, "%d", (gridTool[tool][site].max));
	sprintf(minamount, "%d", (gridTool[tool][site].min));

	if (doOrDraw == SANDBOX_DO) {
		for (j = 1; j <= 5; j++) {
			for (i = -1; i <= 2; i++) {
				buttony = i * 0.15 + menuLoc[1] - 0.05;
				buttonx = j * 0.3 + menuLoc[0] - 0.15;

				if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
					if ((SANDBOX_G.selectButton == ON) && !SANDBOX_G.forceSelectUp) {
						if (j == 1)
							switch (i) {
							case -1:
								gridTool[tool][site].min -= 5;
								break;
							case 1:
								gridTool[tool][site].min += 5;
								break;
							}
						if (j == 2)
							switch (i) {
							case -1:
								gridTool[tool][site].max -= 5;
								break;
							case 1:
								gridTool[tool][site].max += 5;
								break;
							}
						if (j == 4)
							switch (i) {
							case -1:
								gridTool[tool][site].displaySound =
									!gridTool[tool][site].
									displaySound;
								break;
							case 0:
								gridTool[tool][site].displayGraph =
									!gridTool[tool][site].
									displayGraph;
								break;
							case 1:
								gridTool[tool][site].displayQuant =
									!gridTool[tool][site].
									displayQuant;
								break;
							}

						SANDBOX_G.forceSelectUp = 1;
						SANDBOX_beepit();
					}
				}
			}
		}
	} else {
#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0] + 0.6, menuLoc[1], menuLoc[2]);			/* IGL: translate(); */
			glScalef(0.6, 0.35, 0.05);						/* IGL: scale(); */
			SANDBOX_drawcube();
		glPopMatrix();							/* IGL: popmatrix(); */

#ifdef TOOGL_METHOD
		glCallList(SANDBOX_blue); 					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
		glEnable(GL_FRONT);
#else
		glColor4f(0.15, 0.3, 0.7, 1.0);					/* IGL: lmbind(MATERIAL, SANDBOX_blue); */
#endif
		glPushMatrix();							/* IGL: pushmatrix(); */
			glTranslatef(menuLoc[0] + 0.6, menuLoc[1] - 0.35, menuLoc[2]);		/* IGL: translate(); */
			glScalef(0.6, 0.35, 0.05);						/* IGL: scale(); */
			glRotatef(180, 0.0, 0.0, 1.0);						/* IGL: rot(180, 'z'); */
			SANDBOX_drawpyramid();
		glPopMatrix();							/* IGL: popmatrix(); */

		int	tmp;	/* for use with glGetIntegerv() call */
		oldWidth = (int)(glGetIntegerv(GL_LINE_WIDTH, &tmp), tmp);	/* IGL: oldLineWidth = (int)getlwidth(); */
		glLineWidth((GLfloat)(3));					/* IGL: linewidth(3); */

		for (j = 1; j <= 5; j++) {
			for (i = -1; i <= 2; i++) {
				buttony = i * 0.15 + menuLoc[1] - 0.05;
				buttonx = j * 0.3 + menuLoc[0] - 0.15;
#ifdef TOOGL_METHOD
				glCallList(SANDBOX_grey); 			/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
				glEnable(GL_FRONT);
#else
				glColor4f(0.4, 0.4, 0.42, 1.0);			/* IGL: lmbind(MATERIAL, SANDBOX_grey); */
#endif

				if (((j <= 2) && ((i == -1) || (i == 1))) || ((j == 4) && (i < 2))) {
					glPushMatrix();				/* IGL: pushmatrix(); */
						glTranslatef(buttonx, buttony, buttonz);				/* IGL: translate(); */
						glScalef(SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE);	/* IGL: scale(); */
						SANDBOX_drawcube();
					glPopMatrix();				/* IGL: popmatrix(); */
				}

				vertex[2] = buttonz + SANDBOX_BUTTONSIZE + 0.001;

				if (j == 1)
					switch (i) {
					case 2:
						SANDBOX_drawMenuText("Min", buttonx, buttony, vertex[2]);
						break;
					case -1:
						SANDBOX_drawMenuText("-", buttonx, buttony, vertex[2]);
						break;
					case 0:
						SANDBOX_drawMenuText(minamount, buttonx, buttony, vertex[2]);
						break;
					case 1:
						SANDBOX_drawMenuText("+", buttonx, buttony, vertex[2]);
						break;
					}

				if (j == 2)
					switch (i) {
					case 2:
						SANDBOX_drawMenuText("Max", buttonx, buttony, vertex[2]);
						break;
					case -1:
						SANDBOX_drawMenuText("-", buttonx, buttony, vertex[2]);
						break;
					case 0:
						SANDBOX_drawMenuText(maxamount, buttonx, buttony, vertex[2]);
						break;
					case 1:
						SANDBOX_drawMenuText("+", buttonx, buttony, vertex[2]);
						break;
					}

				if (j == 3)
					switch (i) {
					case -1:
						SANDBOX_drawMenuText("Sound", buttonx, buttony, vertex[2]);
						break;
					case 0:
						SANDBOX_drawMenuText("Graph", buttonx, buttony, vertex[2]);
						break;
					case 1:
						SANDBOX_drawMenuText("Values", buttonx, buttony, vertex[2]);
						break;
					}
				if (j == 4)
					switch (i) {
					case -1:
						if (gridTool[tool][site].displaySound)
							SANDBOX_drawMenuText("on", buttonx, buttony, vertex[2]);
						else
							SANDBOX_drawMenuText("off", buttonx, buttony, vertex[2]);
						break;
					case 0:
						if (gridTool[tool][site].displayGraph)
							SANDBOX_drawMenuText("on", buttonx, buttony, vertex[2]);
						else
							SANDBOX_drawMenuText("off", buttonx, buttony, vertex[2]);
						break;
					case 1:
						if (gridTool[tool][site].displayQuant)
							SANDBOX_drawMenuText("on", buttonx, buttony, vertex[2]);
						else
							SANDBOX_drawMenuText("off", buttonx, buttony, vertex[2]);
						break;
					}

				if (((j <= 2) && ((i == -1) || (i == 1))) || ((j == 4) && (i < 2)))
					if (SANDBOX_inSpecificRange(buttonx, buttony, buttonz, SANDBOX_BUTTONSIZE, SANDBOX_BUTTONSIZE, 2 * SANDBOX_BUTTONSIZE)) {
#ifdef TOOGL_METHOD
						glCallList(SANDBOX_tyello); 	/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
						glEnable(GL_FRONT);
#else
						glColor4f(0.8, 0.8, 0.0, 0.5);	/* IGL: lmbind(MATERIAL, SANDBOX_tyello); */
#endif

						glPushMatrix();			/* IGL: pushmatrix(); */
							glTranslatef(buttonx, buttony, buttonz);		/* IGL: translate(); */
							glScalef(SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001, SANDBOX_BUTTONSIZE + 0.001);	/* IGL: scale(); */
							SANDBOX_drawcube();
						glPopMatrix();			/* IGL: popmatrix(); */
					}

				glLineWidth((GLfloat)(oldWidth));		/* IGL: linewidth(oldWidth); */
			}
		}
	}
}

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/

/********************************/
/* BgnGlobNotes()		*/
/********************************/
void BgnGlobNotes()
{
#ifdef SOUND

	/* Use this if you want the server to echo commands that you send
	 * actorMessage (A_SetPrintCommands, "F", 1.); 
	 */

	/* Create actor for sample played between apps (see main.c) */
	*buttonactor = actorMessage(A_Create, "F", (float) SampleActor);
	*thermoactor = actorMessage(A_Create, "F", (float) SampleActor);
	*windactor = actorMessage(A_Create, "F", (float) SampleActor);
	*beakeractor = actorMessage(A_Create, "F", (float) SampleActor);

	GetVssLibVersion();
#endif
}

/********************************/
/* EndGlobNotes()		*/
/********************************/
void EndGlobNotes()
{
#ifdef SOUND

	actorMessage(A_Delete, "F", *buttonactor);
	actorMessage(A_Delete, "F", *thermoactor);
	actorMessage(A_Delete, "F", *windactor);
	actorMessage(A_Delete, "F", *beakeractor);
#endif
}

/********************************/
/* SrvInit()			*/
/********************************/
int SrvInit()
{
	/* Start communication with the server */

#ifdef SOUND
	if (!FBgnSoundServer()) {
		printf("UDP connection to sound server failed\n");
		return 0;
	}

	/* Set up global notes and sequences */
	/* Do this here before CAVEInit too  */

	BgnGlobNotes();
#endif

	return 1;
}

/********************************/
/* SrvEnd()			*/
/********************************/
void SrvEnd()
{
#ifdef SOUND
	/* HEY, Don't Litter -- Clean up your Actors */
	EndGlobNotes();

	/* Break the connection to the server */
	EndSoundServer();
#endif
}

/* YOUR INCLUDES HERE */
void	demo_draw(), demo_init(), demo_update();
void	SANDBOX_mem_init(), SANDBOX_graph_init();

#if 1
int main(int argc, char *argv[])
#else
int main()
#endif
{
	int	ii;
	float	loop = 1.0;

#ifdef SOUND
	/* Set up shared memory */
	shared_arena = CAVEUserSharedMemory(2048);
	/* set up some actor handles in cave shared memory */
	buttonactor = (float *) amalloc(sizeof(float), shared_arena);
	thermoactor = (float *) amalloc(sizeof(float), shared_arena);
	windactor = (float *) amalloc(sizeof(float), shared_arena);
	beakeractor = (float *) amalloc(sizeof(float), shared_arena);

	sNote = (float *) amalloc(sizeof(float), shared_arena);
	tNote = (float *) amalloc(sizeof(float), shared_arena);
	wNote = (float *) amalloc(sizeof(float), shared_arena);
	bNote = (float *) amalloc(sizeof(float), shared_arena);
#endif

#ifdef BS_CHANGE
	/* moved the sound initialization to after CAVEInit(), */
	/*   and set the audio directory to a known full path.  */
#endif

	SANDBOX_mem_init();

#ifdef CAVE
	CAVEInit();
	CAVEInitApplication(SANDBOX_graph_init, 0);
#else /* FreeVR */
	vrStart();
	vrFunctionSetCallback(VRFUNC_DISPLAY_INIT, vrCallbackCreate(SANDBOX_graph_init, 0));
	vrFunctionSetCallback(VRFUNC_DISPLAY, vrCallbackCreate(demo_draw, 0));

#if (FREEVR_VERSION>=000507)	/* only do this with versions of 0.5g and later */
	vrSystemSetName("sandbox -- Sample data monitor/visualization application");
	vrSystemSetAuthors("Andy Johnson (FreeVR port: Bill Sherman)");
	vrInputSet2switchDescription(0, "Terminate the application");
	vrInputSet2switchDescription(2, "Select objects from pallete, calendar or menu");
	vrInputSet2switchDescription(3, "Summon/Dispell menu");
#endif
#endif

#ifdef SOUND
	/* Start the Sound server */
	ii = SrvInit();

	actorMessage(A_SetDirectory, "FS", *buttonactor, "/vroom/caveapps/sandbox/SANDBOX_sounds");
	actorMessage(A_SetDirectory, "FS", *thermoactor, "/vroom/caveapps/sandbox/SANDBOX_sounds");
	actorMessage(A_SetDirectory, "FS", *windactor, "/vroom/caveapps/sandbox/SANDBOX_sounds");
	actorMessage(A_SetDirectory, "FS", *beakeractor, "/vroom/caveapps/sandbox/SANDBOX_sounds");

	*tNote = actorMessage(A_BeginNote, "FS", *thermoactor, "cicada.aiff");
	actorMessage(A_SetLoop, "FFF", *thermoactor, *tNote, loop);

	*wNote = actorMessage(A_BeginNote, "FS", *windactor, "wind.aiff");
	actorMessage(A_SetLoop, "FFF", *windactor, *wNote, loop);
#endif


#ifdef CAVE
	while (!CAVEgetbutton(CAVE_ESCKEY)) {
#  if 0
		signal(SIGINT, CAVEExit);
#  endif
		CAVEDisplay(demo_draw, 0);
	}
#else
	while (!vrGet2switchValue(0)) {
		vrFrame();
#if 0
		demo_update();
#endif
	}
#endif

/****************************************************/
#ifdef SOUND
	SrvEnd();
#endif
/****************************************************/

#ifdef CAVE
	CAVEExit();
#else
	vrExit();
#endif

	return 0;
}

