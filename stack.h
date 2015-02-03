/*
** stack.h
** The header file for a Borland implementation of a linked listStack
** Written by: Andrew Nisbet
** January 14 2002
**
** This implementation of a stack contains only one string which
** was originally designed to store file names in a directory.
** It can be easily adjusted to include more data but you must also
** alter stack.c to account for the extra you add.
*/

#include <stdio.h>
#include <stdlib.h>	/* for malloc() */

#ifndef LINK_LIST_H
#define LINK_LIST_H
#endif
#define MAX_SIZE 256

typedef struct LinkList
{
	char 	item[MAX_SIZE];
	struct 	LinkList *next_node;
} NODE;


void push(char *);
void pop(char *);
void listStack(void);
int  lengthStack(void);