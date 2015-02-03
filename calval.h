/*
** App name:	calval
** File name:	calval.h
** Author:		Andrew Nisbet
** Date:		January 25 2002
** Version:		1.50_01	June 25, 2002 - Fixed erroneous pad field width
**				in struct CALSHEAD
*/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "stack.h"



#define MAX_NAME 260		/* Borlands max chars per file name under NT */
#define MAX_INT 12			/* number of digits in the smallest int with '\0' */
#define HREC 11				/* number of fields in header */
#define HEAD_WIDTH 128 		/* width of the header fields */
#define RESERVE_WIDTH 640	/* the odd sized reserved area in the head */
#define PAD_CHAR 0x20		/* space char to pad out the head strings with */
#define BUFF_SIZE 65535		/* size of read write buffer */
#define FALSE 0
#define TRUE 1



/* #define DEBUG */
typedef struct HS {
	char val[HEAD_WIDTH];
} CALSSTRING;



/* structure for writing new cals header*/
typedef struct H {
	char sdi[HEAD_WIDTH];		/* source doc id */
	char ddi[HEAD_WIDTH];		/* destination doc id */
	char tfi[HEAD_WIDTH];		/* text page figure belongs to */
	char fgi[HEAD_WIDTH];		/* figure id */
	char sgr[HEAD_WIDTH];		/* source graphic id */
	char dcl[HEAD_WIDTH];		/* document classification */
	char rtp[HEAD_WIDTH];		/* cals type I or II */
	char ori[HEAD_WIDTH];		/* orientation */
	char pel[HEAD_WIDTH];		/* pel per line and scan lines per page */
	char den[HEAD_WIDTH];		/* density */
	char not[HEAD_WIDTH];		/* note field */
	char pad[RESERVE_WIDTH];	/* reserved field */
} CALSHEAD;



/* function definitions */
int parse(char *, char *);
int analyse(char *, char *, CALSHEAD *);
void readDirectory(void);
int validateCal(char * ,int );
void padArray(char *, int );

/* EOF */