/*
** App name:	calval
** File name:	readdir.c
** Purpose:		Read the current dir and make a list of all fax and cal files.
** Author:		Andrew Nisbet
** Date:		January 25 2002
*/


#include <dirent.h>
#include <ctype.h>
#include "calval.h"


int endsWith(char * , const char *);



void readDirectory(void)
{

	DIR *dirPtr;
	struct dirent *entry; // d_dirname in the header dirent.h
	const char extcal[] = ".cal";
	const char extfax[] = ".fax";

	#ifdef DEBUG
	printf("Entering readdirectory()\n");
	#endif

	if ((dirPtr = opendir(".")) == NULL)
		fprintf(stderr,"Error: unable to open directory for listStacking.");
	else
	{
		while (entry = readdir(dirPtr))
		{
			/* printf("%s\n",entry); */
			/*
			** This is pretty sucky and not fool proof but it is supposed
			** to catch the majority of fax extensions because the comparison
			** is case sensitive.
			*/
			if (endsWith((char*)entry, extcal) == 1)
			{
				push((char *)entry);
			}
			else if (endsWith((char*)entry, extfax) == 1)
			{
				push((char *)entry);
			}
		} /* end while */

		closedir(dirPtr);

	}

	#ifdef DEBUG
	printf("Leaving readDirectory()\n");
	#endif

	return;
}

/*
** endsWith() takes two args - a name of a file and an expected extension
** If the extension does not start with a '.' the extension is invalid and
** the function returns -1. else the
*/
int endsWith(char *name, const char *extension)
{

	int i, j;						/* generic counters */
	char end[MAX_NAME];				/* name of the extension from passed name */
	char tolow[MAX_NAME];			/* where we store the tolower(name) */
	int lenend = strlen(extension);	/* lengthStack of the file's extension */
	int lenname = strlen(name);		/* lengthStack of file name + extension */
	int found;						/* return value */


	#ifdef DEBUG
	printf("Entering endsWith()\n");
	#endif



	/* if someone passes an extension longer than MAX_NAME chars fail gracefully */
	if (lenend >= MAX_NAME)
	{
		fprintf(stderr,"Error: extension too big.\n");
		return -1;
	}

	/* if the file name is less than the extension return false - no match (hmmm)*/
	if ((lenname -lenend) <= 0)
		return 0;

	/*
	** Now we do the case insensitive matching by casting all chars to lowercase.
	*/
	i = 0;
	while(i < MAX_NAME)
	{
		tolow[i] = (char)tolower(name[i]);
		if (tolow[i] == '\0')
			break;
		i++;
	}


	for (j = 0, i = (lenname -lenend); i < lenname; j++,i++)
		end[j] = tolow[i];


	end[lenend] = '\0';


	if ((strcmp(end,extension) > 0) || (strcmp(end,extension) < 0))
		found = 0;
	else
		found = 1;

	#ifdef DEBUG
	printf("leaving endsWith()\n");
	#endif

	return found;
}