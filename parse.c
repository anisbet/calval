/*
** App name:	calval
** File name:	parse.c
** Purpose:		This module takes extracts true 'c' strings from the header
**				character arrays and then analyses the strings for value
**				correctness.
** Author:		Andrew Nisbet
** Date:		January 25 2002
** Version:		1.5
*/


#include "calval.h"





/* private member function declared here */
void getData(char *, char *, int);
int getNumericData(char *, int);
/* x DPI y DPI and Rdensity */
void calculateDimensions(int x, int y, int d);
void integerToString(char *, int, unsigned int);
void populateHeadChar(char *, const char *, char *);




int parse(char *src, char *dest)
{
	/* keep track of the number of white characters we have encountered. */
	int whtCharCount = 0;	/* Count of white characters in a given array of chars */
	extern int VERBOSE;		/* Verbose mode as set from calval.c */
	extern FILE *fout;		/* I don't like global anythings but... */
	int FIX_FLAG = 0;		/* Flag is set if any error is encountered. */
	int alreadyWarned = 0;	/* Turns off warning of illegal char after first instance */
	int i;					/* counter var */

	/*
	** Traverse the string looking for non-valid white chars and non-7-bit
	** ASCII chars and replace them or ignore them depending.
	*/

	/* while the contents of the pointer are not null... */
	for (i = 0; i < 128; i++)
	{
		/*
		** If the *src is greater than D126 then it is illegal and ignore.
		** This should never happen.
		*/
		if (*src > 0x7E || *src <= 0x20)
		{
			if (*src != 0x20)
			{
				if (VERBOSE > 0 && alreadyWarned == FALSE)
				{
					fprintf(fout,"Warning: illegal characters found in header.\n");
					alreadyWarned = TRUE;
				}
				FIX_FLAG = 1;
			}

			whtCharCount++;
			/* reset the dest pointer so it doesn't advance. */
			if (whtCharCount >= 2)
			{
				dest--;
			}
			else
			{
				*dest = 0x20;
			}
		}
		else
		{
			*dest = *src;
			whtCharCount = 0;
		}

		src++;
		dest++;
	}

	/*
	** Regardless of the number of white chars we found only one is passed
	** to the dest string and so we only have to digress to the last position.
	*/
	if (whtCharCount >= 2)
	{
		dest--;
	}

	*dest = '\0';

	return FIX_FLAG;
}






/*
** This next part goes through the string and extracts the actual values
** from the strings. It returns a repair flag if required.
*/
int analyse(char *string, char *tmp, CALSHEAD *header)
{
	extern FILE *fout;	/* where to output our information */
	extern FILE *hout;	/* HTML file pointer */
	extern int STRICT;	/* strict flag reports subtle problems */
	extern int REPAIR;	/* repair if possible - over-writes original cals */
	extern int VERBOSE;	/* verbosity <0 = brief, = 0 basic info, >0 = dump all */
	int FIX_FLAG = 0;	/* Flag is set if any error is encountered. */

	int pelPath, lineProgress;	/* for collection of data from strtoi() */
	static int ppl = 0, nol = 0;/* pels per line, number of lines. */
	int rden;					/* raster density (DPI) */

	/* Legal vals of the header strings */
	const char srcdid[] = "srcdocid: ";
	const char dstdid[] = "dstdocid: ";
	const char txtfid[] = "txtfilid: ";
	const char _figid[]	= "figid: ";
	const char srcgph[] = "srcgph: ";
	const char doccls[] = "doccls: ";
	const char _rtype[] = "rtype: ";
	const char rorint[] = "rorient: ";
	const char rpelct[] = "rpelcnt: ";
	const char rdenst[] = "rdensty: ";	/* this is correct spelling of this field */
	const char xdenst[] = "rdensity: "; /* this is not (sigh,I know) */
	const char _notes[] = "notes: ";
	/*
	** This value is added because Corel CALS don't add anything to the
	** notes field and this application removes as much duplicate white
	** space as possible. The result is the match value with the space
	** at the end fails. This is an error because empty fields are supposed
	** to contain the word 'NONE'. Make a note to fix if user wants.
	*/
	const char xnotes[] = "notes:";




	/* source document ID */
	if (strncmp(srcdid,string,strlen(srcdid)) == 0)
	{
		getData(string,tmp,strlen(srcdid));
		if (VERBOSE > 0)
			fprintf(fout,"source document ID: %s\n",tmp);

		/* so if the repair flag is set and header isn't NULL then do this */
		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->sdi, srcdid, tmp);
		}
	}
	/* Destination document ID */
	else if (strncmp(dstdid,string,strlen(dstdid)) == 0)
	{
		getData(string,tmp,strlen(dstdid));
		if (VERBOSE > 0)
			fprintf(fout,"destination document ID: %s\n",tmp);

		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->ddi, dstdid, tmp);
		}
	}
	/* Location of graphic in document (page no) */
	else if (strncmp(txtfid,string,strlen(txtfid)) == 0)
	{
		getData(string,tmp,strlen(txtfid));
		if (VERBOSE > 0)
			fprintf(fout,"location of graphic in document: %s\n",tmp);

		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->tfi, txtfid, tmp);
		}
	}
	/* figure ID of this graphic */
	else if (strncmp(_figid,string,strlen(_figid)) == 0)
	{
		getData(string,tmp,strlen(_figid));
		if (VERBOSE > 0)
			fprintf(fout,"figure: %s\n",tmp);

		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->fgi, _figid, tmp);
		}
	}
	/* Name of the originating graphic */
	else if (strncmp(srcgph,string,strlen(srcgph)) == 0)
	{
		getData(string,tmp,strlen(srcgph));
		if (VERBOSE > 0)
			fprintf(fout,"graphic's original name: %s\n",tmp);

		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->sgr, srcgph, tmp);
		}
	}
	/* document's security classification */
	else if (strncmp(doccls,string,strlen(doccls)) == 0)
	{
		getData(string,tmp,strlen(doccls));
		if (VERBOSE >= 0)
			fprintf(fout,"document security classification: %s\n",tmp);

		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->dcl, doccls, tmp);
		}
	}
	/* CALS type */
	else if (strncmp(_rtype,string,strlen(_rtype)) == 0)
	{
		getData(string,tmp,strlen(_rtype));
		if (VERBOSE >= 0)
		{
			fprintf(fout,"CALS raster type: %s \n",tmp);
			if (strcmp(tmp,"1") != 0)
				fprintf(fout,"(may contain multipule images)\n");
		}
		if (REPAIR == TRUE && header != NULL)
		{
			populateHeadChar(header->rtp, _rtype, tmp);
		}
	}
	/* orientation of scan lines compared to original document. */
	else if (strncmp(rorint,string,strlen(rorint)) == 0)
	{
		getData(string,tmp,strlen(rorint));
		pelPath = getNumericData(tmp,0);
		lineProgress = getNumericData(tmp,1);

		if (VERBOSE > 0)
		{
			fprintf(fout,"graphic's scan geometry:\n");
			fprintf(fout,"  pel path = %d\n",pelPath);
			fprintf(fout,"  scan line progression = %d \n",lineProgress);
		}

		if (pelPath != 0 && pelPath != 90 &&
			pelPath != 180 && pelPath != 270)
		{
			FIX_FLAG = 1;
			fprintf(fout,"***Error: invalid pel path; file not repairable.***\n");
		}

		if (lineProgress != 90 && lineProgress != 270)
		{
			FIX_FLAG = 1;
			fprintf(fout,"***Error: invalid line progress; file not repairable.***\n");
		}
		/******************* repair *******************/
		if (REPAIR == TRUE && header != NULL)
		{
			/* fill the target with 0x20 */
			padArray(header->ori, HEAD_WIDTH);
			/* now we copy in the const value */
			strcpy(header->ori, rorint);
			/* concatinate the collected value to header string */
			/* we reuse the tmp var which is 128 bytes wide */
			integerToString(tmp, pelPath, 3);
			strcat(header->ori, tmp);
			strcat(header->ori, ",");
			integerToString(tmp, lineProgress, 3);
			strcat(header->ori, tmp);
			/* replace the terminating '\0' with a space */
			header->ori[strlen(header->ori)] = PAD_CHAR;
		}
	}
	/* Pels per line and number of lines in file */
	else if (strncmp(rpelct,string,strlen(rpelct)) == 0)
	{
		getData(string,tmp,strlen(rpelct));
		ppl = getNumericData(tmp,0);
		nol = getNumericData(tmp,1);

		if (VERBOSE > 0)
		{
			fprintf(fout,"pels per line: %d.\n",ppl);
			fprintf(fout,"number of lines: %d.\n",nol);
		}


		/******************* repair *******************/
		if (REPAIR == TRUE && header != NULL)
		{
			/* fill the target with 0x20 */
			padArray(header->pel, HEAD_WIDTH);
			/* now we copy in the const value */
			strcpy(header->pel, rpelct);
			/* concatinate the collected value to header string */
			integerToString(tmp, ppl, 6);
			strcat(header->pel, tmp);
			strcat(header->pel, ",");
			integerToString(tmp, nol, 6);
			strcat(header->pel, tmp);
			/* replace the terminating '\0' with a space */
			header->pel[strlen(header->pel)] = PAD_CHAR;
		}
		/*
		** The rest of this calculation will take place once we know the
		** the resolution of the document. Because it is static this will
		*/
	}
	/* Resolution */
	else if (strncmp(xdenst,string,strlen(xdenst)) == 0)
	{
		getData(string,tmp,strlen(xdenst));
		rden = getNumericData(tmp,0);
		if (VERBOSE >= 0)
		{
			fprintf(fout,"resolution: %d\n",rden);
		}

		/*
		**	Here is where we output the resolution into <TD> tags.
		**	if HTML is required send it out now
		*/
		if (hout != NULL)
			fprintf(hout,"\n\t<TD align=\"center\">%d</TD>",rden);


		if (STRICT == TRUE)
		{
			FIX_FLAG = 1;
			fprintf(fout,"Warning: rdensity field identifier contains a spelling error.\n");
		}

		if (STRICT == TRUE)
		{
			FIX_FLAG = 1;
			/*
			** Complain if strict set that a density that does not match these
			** is incorrect to specifications.
			*/
			if (rden != 200 && rden != 240 && rden != 300 && rden != 400 &&
				rden != 600 && rden != 1200)
			{
				fprintf(fout,"Warning: file has an invalid resolution; valid resolutions are:\n");
				fprintf(fout,"200, 240, 300, 400, 600, or 1200 pels per inch.\n");
				fprintf(fout,"Calval cannot repair this type of non-compliance.\n");
			}
		}
		if (VERBOSE >= 0)
		{
			calculateDimensions(ppl, nol, rden);
		}
		/*
		** Reset these so we know the static values are for this graphic
		** the next time analyse() is called.
		*/
		ppl = 0;
		nol = 0;

		/******************* repair *******************/
		if (REPAIR == TRUE && header != NULL)
		{
			/* fill the target with 0x20 */
			padArray(header->den, HEAD_WIDTH);
			/* now we copy in the const value */
			strcpy(header->den, rdenst);
			/* concatinate the collected value to header string */
			integerToString(tmp, rden, 4);
			strcat(header->den, tmp);
			/* replace the terminating '\0' with a space */
			header->den[strlen(header->den)] = PAD_CHAR;
		}
	}
	/* Resolution if they spelled density correctly (sigh) */
	else if (strncmp(rdenst,string,strlen(rdenst)) == 0)
	{
		getData(string,tmp,strlen(rdenst));
		rden = getNumericData(tmp,0);
		if (VERBOSE >= 0)
		{
			fprintf(fout,"resolution: %d\n",rden);
		}

		/*
		**	Here is where we output the resolution into <TD> tags.
		**	if HTML is required send it out now
		*/
		if (hout != NULL)
			fprintf(hout,"\n\t<TD align=\"center\">%d</TD>",rden);


		if (STRICT == TRUE)
		{
			/*
			** Complain if strict set that a density that does not match these
			** is incorrect to specifications.
			*/
			if (rden != 200 && rden != 240 && rden != 300 && rden != 400 &&
				rden != 600 && rden != 1200)
			{
				fprintf(fout,"Warning: invalid resolution; valid resolutions are:\n");
				fprintf(fout,"200, 240, 300, 400, 600, or 1200 pels per inch.\n");
				fprintf(fout,"Calval cannot repair this type of non-compliance.\n");
				FIX_FLAG = 1;
			}
		}
		if (VERBOSE >= 0)
		{
			calculateDimensions(ppl, nol, rden);
		}
		/*
		** Reset these so we know the static values are for the next graphic.
		*/
		ppl = 0;
		nol = 0;

		/******************* repair *******************/
		if (REPAIR == TRUE && header != NULL)
		{
			/* fill the target with 0x20 */
			padArray(header->den, HEAD_WIDTH);
			/* now we copy in the const value */
			strcpy(header->den, rdenst);
			/* concatinate the collected value to header string */
			integerToString(tmp, rden, 4);
			strcat(header->den, tmp);
			/* replace the terminating '\0' with a space */
			header->den[strlen(header->den)] = PAD_CHAR;
		}
	}
	/* Notes */
	else if (strncmp(_notes,string,strlen(_notes)) == 0)
	{
		getData(string,tmp,strlen(_notes));
		if (VERBOSE >= 0)
		{
			fprintf(fout,"notes: %s\n",tmp);
		}

		if (REPAIR == TRUE && header != NULL)
		{
			if (strcmp(tmp,"") == 0)
				tmp = "NONE";
			populateHeadChar(header->not, _notes, tmp);
		}
	}
	else if (strncmp(xnotes,string,strlen(xnotes)) == 0)
	{
		FIX_FLAG = 1;
		getData(string,tmp,strlen(xnotes));
		if (VERBOSE >= 0 && strlen(tmp) > 1)
		{
			fprintf(fout,"notes: %s\n",tmp);
		}
		else if (STRICT == TRUE)
		{
			fprintf(fout,"The notes field is empty but should contain 'NONE'\n");
		}

		if (REPAIR == TRUE && header != NULL)
		{
			tmp = "NONE";
			populateHeadChar(header->not, _notes, tmp);
		}
	}
	else
	/* catch everything else that may come down the pipe */
	{
		fprintf(fout,"***Warning: unexpected value '%s' found but not expected\n",string);
		fprintf(fout,"***not part of MIL-R-28002B. If this file is repaired\n");
		fprintf(fout,"***these additions will not be included.\n");
		FIX_FLAG = 1;
	}


	return FIX_FLAG;
}




/********************* private members *****************************/


/*
**  This little baby converts an integer to a string - a suprisingly common
**	(for me) need but there doesn't seem to be a C lib to do this.
**	Am I missing something?
**
**	For validatecal.c we are going to modify it so that it will also pad
**	extra zeros in front of MSD to round out the numbers to look like this
**	"rdensity: 400" will become "rdensity: 0400"
**
**	This function also handles negative numbers as well so: -49 pad 3 is -049
*/
void integerToString(char *dest, int integer, unsigned int pad)
{
	int j;				/* locator within dest where c[last_element] is */
	unsigned int i;		/* same thing but stops warning messages from compiler */
	unsigned int len;	/* length of the number string */
	int NEG = 0;		/* flag to indicate integer is negative */
	char c[MAX_INT];	/* a temporary array to contain characters */
						/*
						** MAX_INT is the max number of character it
						** takes to express the smallest integer
						** -4294967296 plus a '\0'
						*/
	char *pc, *pd;/* pointer to c, pointer to dest */

	/* first determine if integer is positive or negative */
	if (integer < 0)
	{
		integer = abs(integer);
		NEG = 1;
	}

	pc = c;

	while (integer > 0)
	{
		*pc = (char)(integer % 10 + 48);
		integer /= 10;
		pc++;
	}

	/* terminate the string so we can test it's length - have to do it anyway */
	*pc = '\0';
	len = strlen(c);

	/* now do we have to pad the string with leading 0's */
	if (len < pad)
	{
		for (i = 0; i < (pad - len); i++)
		{
			*pc = '0';
			pc++;
		}
	}

	/* now do we have to add a negative sign? */
	if (NEG == 1)
	{
		*pc = '-';
		pc++;
	}

	*pc = '\0';

	/* printf("c contains: %s\n",c); */
	/* reset the pointer to the beginning of string */
	pc = c;
	/* point pd to the jth address of dest */
	j = strlen(c);
	pd = &dest[j];


	/* set contents of that address to null */
	*pd = '\0';
	/* retract the pointer */
	pd--;

	/* now while pc points to non-null contents of c[] keep going */
	while (*pc)
		*pd-- = *pc++;


	return;
}





/*
** This is to calculate some basic dimensions of the CALS file
** and as it is called from a couple of places it is a candidate for
** a function.
*/
void calculateDimensions(int x, int y, int d)
{
	extern FILE *fout;
	extern FILE *hout;	/* HTML file pointer */

	if (d > 0)
	{
		fprintf(fout,"graphic dimensions: %0.2f\" x %0.2f\"\n",((float)x/(float)d),((float)y/(float)d));

		/*
		**	Here is where we output the dimensions into <TD> tags.
		*/
		if (hout != NULL)
			fprintf(hout,"\n\t<TD align=\"center\">%0.2f\" x %0.2f\"</TD>",((float)x/(float)d),((float)y/(float)d));
	}

	fprintf(fout,"raster data occupies: %d bytes uncompressed.\n",((x * y) / 8));

	return;
}


/*
** This will extract the data from a ',' delimited string (if there is one)
** and populate return either the number on the left or the number on the
** right. Any number smaller than 0 means the left of the ',' anything
** bigger than 0 is the right.
*/
int getNumericData(char *src, int which)
{
	char tmpa[HEAD_WIDTH];
	char tmpb[HEAD_WIDTH];
	char *t;
	int I;

	t = tmpa;

	while(*src)
	{
		if (*src == ',')
		{
			*t = '\0';
			t = tmpb;
			src ++;
			continue;
		}
		*t = *src;
		src++;
		t++;
	}
	/* terminate tmpb */
	*t = '\0';

	if (which <= 0)
		I = atoi(tmpa);
	else
		I = atoi(tmpb);

	return I;
}

/*
** Extract just the data from the string; we do this so we can apply
** meaningful titles to the displayed values. Collect everything after
** the ': '.
*/
void getData(char *src, char *dest, int len)
{
	int i = 0;

	while(*src)
	{
		if (i >= len)
		{
			*dest = *src;
			dest++;
		}

		i++;
		src++;
	}

	*dest = '\0';

	return;
}

/*
**	This function mearly takes a char array and a size of the array
**	and fills it with space characters. The spaces at the front of
**	the array will be over - written later with valid data.
*/
void padArray(char *array, int size)
{
	int i;
	for ( i = 0; i < size; i++)
		array[i] = PAD_CHAR;

	return;
}

/*
** This function takes three strings as targets the first is the destination
** string-normally the header string, a myTag value which const char[] name
** of that field and finally the actual data associated with that tag.
*/
void populateHeadChar(char *target, const char *myTag, char *dataStr)
{
	/* fill the target with 0x20 */
	padArray(target, HEAD_WIDTH);
	/* now we copy in the const value */
	strcpy(target, myTag);
	/* concatinate the collected value to header string */
	strcat(target, dataStr);
	/* replace the terminating '\0' with a space */
	target[strlen(target)] = PAD_CHAR;
	/* printf(">%s<",target); */

	return;
}