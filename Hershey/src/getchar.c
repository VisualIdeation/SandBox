#include <stdio.h>


#if 0				/* These were included in the IGL_VERSION, but are totally unnecessary */
#include <gl.h>
#include <device.h>
#endif


/*
 * getcharacter
 *
 *	read in a character line from a hershey font file
 */
int
getcharacter(fp, no, pairs, buf)
	FILE	*fp;
	int	*no, *pairs;
	char	*buf;
{
	int	i;
	char	*p, tmp[10];

	/*
	 * read in the numbers
	 */
	for (i = 0; i < 5; i++) {
		if ((tmp[i] = fgetc(fp)) == '\n')	/* take care of the odd blank line */
			tmp[i] = fgetc(fp);
		if (feof(fp))
			return(0);
	}
	tmp[5] = 0;
	sscanf(tmp, "%d", no);
		
	for (i = 0; i < 3; i++) {
		tmp[i] = fgetc(fp);
		if (feof(fp))
			return(0);
	}
	tmp[3] = 0;
	sscanf(tmp, "%d", pairs);
		

	/*
	 * read in the pairs
	 */
	for (p = buf, i = 0; i < 2 * *pairs; i++, p++)
		if ((*p = fgetc(fp)) == '\n')
			*p = fgetc(fp);

	*p = 0;

	fgetc(fp);			/* skip newline at end */

	return(1);
}