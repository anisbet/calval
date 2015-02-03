/*
** App name:	calval
** File name:	validatecal.c
** Purpose:		This module opens the target file and controls the processing
**				of the file one at a time in an orderly fashion.
** Author:		Andrew Nisbet
** Date:		January 25 2002
*/



#include "calval.h"

int GOOD = 0;					/* used for reporting total good files to calval.c */

void writeTableRecord(char *,int);	/* writes the opening and closing of an html table record */
void closeTableRecord(int);		/* does the clean up for the next record */
extern FILE *hout;				/* global stream for HTML file. */



int validateCal(char *file, int mynumber)
{

	FILE *fcals;				/* cals file pointer */
	FILE *ftmp;					/* new out file for repaired data. */
	extern FILE *fout;			/* where we want the output */
	CALSSTRING head[HREC];		/* cals header structure */
	int FIX_FLAG = 0;			/* return val from parse-do we need to fix file?*/
	int i;						/* counter var */
	char *filename;				/* pointer to file(name) argument */
	char tmpfilename[MAX_NAME]; /* name returned by tmpnam() */
	char parseResult[HEAD_WIDTH];	/* string to hold parsed header data */
	char analyseResult[HEAD_WIDTH];	/* holds results of parse() and analyse() */
	char BUFFER[BUFF_SIZE];		/* Read write buffer size */
	int bytesRead;				/* bytes read to and from cals file */

	CALSHEAD *header;			/* make a structure for header if repair is option */

	extern int REPAIR;			/* is the repair flag set */




	/* now reserve some space for that - just for safty sake */
	if((header = malloc(sizeof(CALSHEAD))) == NULL)
	{
		fprintf(stderr,"Error: malloc() failed; will continue validation only.");
		/* This will stop you having to check for header's existance */
		REPAIR = FALSE;
	}




	/* open cal file */
	filename = file;

	/* log file name */
	if (fout != stdout)
		fprintf(fout,"examining: %s\n",filename);

	printf("examining: %s\n",filename);

	/* open the cals file if possible and return if you can't */
	if ((fcals = fopen(filename,"rb+")) == NULL)
	{
		fprintf(stderr,"Error: file is in use or does not exist.\n");
		if (header != NULL)
			free(header);
		return 1;
	}





	/*
	** This processes the header one entry at a time and stores it in the
	** header structure, but first let's record the event in HTML if requested.
	*/
	if (hout != NULL)
		writeTableRecord(filename,mynumber);

	for (i = 0; i < HREC; i++)
	{
		if ((fread(head[i].val,sizeof(char),HEAD_WIDTH,fcals)) != HEAD_WIDTH)
		{
			fprintf(stderr,"Error: failed to read entire CALS header.\n");
			fclose(fcals);
			if (header != NULL)
				free(header);
			return 1;
		}

		/*
		** Parse out useful and valid information
		** parse()'s return value is a flag to fix the string.
		*/
		FIX_FLAG += parse(head[i].val, parseResult);
		FIX_FLAG += analyse(parseResult, analyseResult, header);
		//printf("analyseResult now contains: %s\n", analyseResult);

	}

	/* close this record for writing in preparation for the next */
	if (hout != NULL){
		closeTableRecord(FIX_FLAG);
	}






	/* pad the reserve field, write the header to file, write the grahics data  */
	if (REPAIR == TRUE)
	{
		/* get a temp file name to write out the good data to */
		if ((tmpnam(tmpfilename) == NULL) ||
			(ftmp = fopen(tmpfilename,"wb")) == NULL)
		{
			fprintf(stderr,"Error: unable to create temp file.\n");
			free(header);
			return 1;
		}
		/* fill the reserve area with spaces */
		padArray(header->pad, RESERVE_WIDTH);
		/* write the header to the tmp file */
		fwrite(header,sizeof(char),2048,ftmp);
		/* free the header space */
		free(header);

		/* now write out the graphic data */
		rewind(fcals);
		fseek(fcals, 2048, SEEK_SET);

		while ((bytesRead = fread(BUFFER, sizeof(char), BUFF_SIZE, fcals)) == BUFF_SIZE)
			fwrite(BUFFER, sizeof(char), BUFF_SIZE, ftmp);

		fwrite(BUFFER, sizeof(char), bytesRead, ftmp);

		fclose(ftmp);

	}






	/* close the cals file regardless of if repair is flagged or not */
	fclose(fcals);


	/*
	** If everything went well we get to here and now the cals file is
	** closed we can over-write the bad data with the good.
	*/
	if (REPAIR == TRUE)
	{
		if (remove(filename) == 0)
		{
			rename(tmpfilename, filename);
		}
		else
		{
			fprintf(stderr,"Error: can't repair cals file because it may be in use.");
			/* clean up after ourselves */
			remove(tmpfilename);
		}
	}



	fprintf(fout,"--- Summary: ");


	if (FIX_FLAG > 0 && REPAIR == TRUE)
	{
		fprintf(fout,"repaired discrepancies\n");
		fprintf(fout,"The source of any other problems with this file may lie in\n");
		fprintf(fout,"the  compression scheme (i.e. CCITT group III instead of IV).");
	}
	else if (FIX_FLAG > 0)
	{
		fprintf(fout,"non-compliant");
	}
	else
	{
		GOOD += 1;
		fprintf(fout,"compliant");
	}

	fprintf(fout," ---\n\n");

	return 0;
}

/*
** Set the preliminaries of writing data to the html table.
** Here we write the table rows and the first 2 and the last records into
** table data tags then out to the web page.
** So File No., File Name and Compliant are written from here.
** see parce.c for the Dimension and Resolution data writing
*/
void writeTableRecord(char *file, int mynumber)
{
	if ((mynumber % 2) == 0)
		fprintf(hout,"<TR bgcolor=\"#99ccff\">\n\t<TD align=\"center\">%d</TD>\n\t<TD align=\"right\"><A HREF=\"%s\">%s</A></TD>",(mynumber +1),file,file);
	else
		fprintf(hout,"<TR>\n\t<TD align=\"center\">%d</TD>\n\t<TD align=\"right\"><A HREF=\"%s\">%s</A></TD>",(mynumber +1),file,file);
}

/* close the record */
void closeTableRecord(int problems)
{
	if (problems < 1)
		fprintf(hout,"\n\t<TD align=\"center\"><B><FONT color=\"green\">Yes</FONT></B></TD>\n");
	else
		fprintf(hout,"\n\t<TD align=\"center\"><B><FONT color=\"red\">No</FONT></B></TD>\n");

	fprintf(hout,"</TR>\n");
}