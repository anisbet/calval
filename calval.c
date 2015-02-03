/**********************************************************************
**
** App name:	calval.exe
**
** Purpose:		To validate, fix and display meaningful info about
**				CALS and FAX files.
**
** Author:		Andrew Nisbet
**
** Date:		January 19 2002
**
** Version:		1.50 - June 19, 2002	Added reporting in HTML.
**				and cleaned up compiler time warnings in readdir.c
**				and parse.c.
**				1.50_01 - June 25, 2002 Fixed minor error in CALSHEAD
**				reserved field size.
**				1.50_02 - February 4, 2003 Minor text change to help
**				display, added '-6' flag to the makefile to optimize for
**				pentiums. See compiler documentation for details.
**
***********************************************************************
*/

#include <time.h>	/* for html time reporting */
#include "calval.h"


const char APPLICATION[] = "calval";	/* used in usage message */
const float VERSION = 1.5002;			/* used in usage message */
const char HTML_FILE[] = "calval.html";	/* html page name */


int STRICT = FALSE; /* strict flag reports subtle problems */
int REPAIR = FALSE; /* repair if possible - over-writes original cals */
int VERBOSE = 0;	/* verbosity <0 = brief, = 0 basic info, >0 = dump all */
FILE *fout;			/* standard and log file pointer. */
FILE *hout;			/* html file output pointer */


void usage(void);
void createHtml(void);
void closeHtml(void);

int main (int argc, char *argv[])
{
	int i,j;
	char filename[MAX_NAME];
	int len;				/* used for reporting total files etc. */
	extern int GOOD;		/* used for reporting set in validatecal.c */

	if (argc < 2)
	{
		usage();
		exit(1);
	}

	/* set the default stream for output and let user change it with '-l' */
	fout = stdout;

	for (i = 1; i < argc; i++)
	{
		strcpy(filename, argv[i]);

		if (strcmp(filename,"-s") == 0)
		{
			/*
			** Strict switch
			*/
			STRICT = TRUE;
			continue;
		}

		if (strcmp(filename,"-l") == 0)
		{
			/*
			** log file request switch
			*/
			if (++i <= argc)
			{
				/*
				** Here we check to see if the user has set the log
				** name to the web page name and if so redirect log
				** back to stdout.
				*/
				if (strcmp(HTML_FILE,argv[i]) == 0)
				{
					fprintf(stderr,"Warning: log file name is a reserved and cannot be used, pick another name.\n");
					fprintf(stderr,"\tDefaulting output to stdout.\n");
					fout = stdout;
				}

				else if ((fout = fopen(argv[i],"w")) == NULL)
				{
					fprintf(stderr,"Warning: log file could not be opened.\n");
					fprintf(stderr,"Output will be printed to stdout.\n");
					fout = stdout;
				}

			}
			else
			{
				fprintf(stderr,"Warning: no log file specified, results will\n");
				fprintf(stderr,"be printed to stdout.\n");
				fout = stdout;
			}


			continue;
		}


		else if (strcmp(filename,"-html") == 0)
		{
			/*
			** HTML reporting request switch
			**
			** If the user selects this it writes the results to an
			** html file in the current directory.
			*/

			if ((hout = fopen(HTML_FILE,"w")) == NULL)
			{
				fprintf(stderr,"Warning: could not create HTML file.\n");
			}

			continue;
		}
		/* here is where we branch to read the directory for cals files. */
		else if (strcmp(filename,"-all") == 0)
		{

			readDirectory();
			continue;
		}

		else if (strcmp(filename,"-r") == 0)
		{
			/*
			** repair the file if possible
			*/
			REPAIR = TRUE;
			continue;
		}

		/*
		** set verbosity
		*/
		else if (strcmp(filename,"-v") == 0)
		{
			/*
			** repair the file if possible
			*/
			VERBOSE = 1;
			continue;
		}
		else if (strcmp(filename,"-b") == 0)
		{
			/*
			** repair the file if possible
			*/
			VERBOSE = -1;
		}
		else
		{
			/*
			** To get here the user hasn't selected all so this arg
			** is a potential file so put it on the stack.
			*/
			push(filename);
		}

	} /* end of for loop of file names from the command line. */


	printf("\nCalval (C) 2002 by Andrew Nisbet.\n");
	printf("Validates CALS files for compliancy with specification:\n");
	printf("MIL-PRF-28002B Amendment 1, 30 September 1993.\n\n");

	/*
	**	Start the HTML writer
	*/
	createHtml();


	/*
	** Call the main file processing routine
	*/
	len = lengthStack();

	for (j = 0; j < len; j++)
	{
		pop(filename);
		validateCal(filename,j);
	}

	printf("---------------------------------\n");
	printf("%d file(s) examined; %d compliant.\n",len,GOOD);

	/*
	**	Start the HTML writer
	*/	/* don't do anything if the html stream is not open. */
	if (hout != NULL)
	{
		closeHtml();
		fprintf(stdout,"(Close the browser to exit %s.)\n",APPLICATION);
		/*
		**	Now open the page for viewing....
		*/
		system(HTML_FILE);
	}


	return 0;
}	/* end main() */




void usage(void)
{

	fprintf(stderr,"\nTHIS IS A CONSOLE PROGRAM,\n");
	fprintf(stderr,"in case you clicked this EXE from Windows explorer.\n");
	fprintf(stderr,"\n%s version %0.4f\n", APPLICATION, VERSION);
	fprintf(stderr,"\nUsage: %s [-all -brsv -l log -html] foo.[cal|fax] [foo\\bar.[cal|fax]...]\n", APPLICATION);
	fprintf(stderr,"\n%s validates and repairs CALS files (both .cal and .fax).\n", APPLICATION);
	fprintf(stderr,"\t-all	= process all CAL and FAX files in current directory only.\n");
	fprintf(stderr,"\t-b	= brief mode; report summary only.\n");
	fprintf(stderr,"\t-html	= send output to HTML file.\n");
	fprintf(stderr,"\t-l	= send output to user specified file.\n");
	fprintf(stderr,"\t-r	= repair any damage if possible.\n");
	fprintf(stderr,"\t-s	= strict mode; report on subtle errors.\n");
	fprintf(stderr,"\t-v	= verbose mode; report everything.\n");

	/*
	** Put this in because some users just click on exe files in windows
	** and they will miss this message.
	*/

	printf("\n(hit <Enter> to continue...)");
	getc(stdin);

	return;
}


/* initiates the html form if one has been specified. */
void createHtml(void)
{
	/* date and time run structure Sec. 18.4 C, A Reference Manual pg. 401 */
	time_t now;
	const char *htmlhead = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 3.2 Final//EN\">\n<HTML>\n<HEAD>\n";
	const char *preamble = "<BR>Calval Results dated: ";
	const char *title = "Calval Results At-A-Glance";
	const char *table = "<TABLE ALIGN=\"center\" BORDER=0 CELLSPACING=3 CELLPADDING=1 WIDTH=\"70%\">\n";
	const char *tableHead = "<TR ALIGN=\"middle\" VALIGN=\"middle\">\n\t<B><TH>File No.</TH>\n\t<TH>File Name</TH>\n\t<TH>Resolution</TH>\n\t<TH>Dimensions</TH>\n\t<TH>Compliant</TH></B>\n";


	now = time(NULL);


	/* don't do anything if the html stream is not open. */
	if (hout == NULL)
		return;

	/* other wise print out this stuff... */
	fprintf(hout,"%s",htmlhead);
	fprintf(hout,"<TITLE>%s</TITLE>",title);
	fprintf(hout,"<META NAME=\"Generator\" CONTENT=\"%s %.2f\">\n</HEAD>\n<BODY>\n", APPLICATION, VERSION);
	fprintf(hout,"<FONT face=\"Arial\"><H3 align=\"center\">%s%s</H3>\n",preamble,ctime(&now));
	fprintf(hout,"%s",table);
	fprintf(hout,"%s",tableHead);
}

/* close everything HTML */
void closeHtml(void)
{

	fprintf(hout,"</TABLE>\n<BR><BR><CENTER><FONT size=-2 color=\"red\">%s written by Andrew Nisbet</FONT></CENTER>\n",APPLICATION);
	fprintf(hout,"</FONT>\n</BODY>\n</HTML>");

	fclose(hout);
}