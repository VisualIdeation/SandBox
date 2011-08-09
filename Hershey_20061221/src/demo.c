/* NOTE: this is an IrisGL program */

#include <stdio.h>
#ifdef SGI
#include <gl.h>
#include <device.h>
#else
#include "vogl.h"
#include "vodevice.h"
#endif

/*
 *	displays every character in a hershey font at 64 characters
 * per screen. Note: this program reads the binary format as created
 * by h2v.
 */

float v[3] = {0,0,0};
Matrix id = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
main(ac, av)
	int	ac;
	char	**av;
{
	char	dev[50];
	int	i, nchars;
	float	x, y;
	short	val;

	if (ac != 2) {
		fprintf(stderr, "fdisp: usage fdisp fontname\n");
		exit(1);
	}

	winopen("fdisp");

	mmode(MVIEWING);

	perspective(450,1,0.1,1000);
	lookat(0,0,1,0,0,0,0);

	doublebuffer();
	gconfig();

	qdevice(KEYBD);
	color(BLACK);
	clear();

	color(GREEN);

	hfont(av[1]);

	nchars = hnumchars();

	htextsize(0.2, 0.2);


	x = 0;
	y = 0;
	move2(x,y);


	loadmatrix(id);
	while(!getbutton(ESCKEY)){
		pushmatrix();
		translate(0,0,-x);

		color(BLACK); clear(); color(GREEN);
		v[2] = x;

		bgnline();
		v[0] = -0.1;
		v3f(v);
		v[0] = 0.1;
		v3f(v);
		endline();

		move(0,0,0);
#if 0
		rot(x*1000,'x');
#endif
		hcharstr("Hello");
		popmatrix();
		swapbuffers();
		x+=0.001;
	}



	gexit();
}

