/*
** stack.h
** The header file for a Borland implementation of a linked listStack
** Written by: Andrew Nisbet
** January 14 2002
**
** This implementation of a stack contains only one string which
** was originally designed to store file names in a directory.
** It can be easily adjusted to include more data but you must also
** alter stack.h to account for the extra you add.
*/

#include "stack.h"
#include <string.h>

static NODE *headPtr = NULL;


void pop(char *str)
{
	NODE *ptr, *ptrDel;


	if (headPtr == NULL)
	{
		;
	}
	else if (headPtr->next_node == NULL)
	{
		strcpy(str,headPtr->item);
		headPtr = NULL;
	}
	else
	{
		/* traverse the listStack to the second last one */
		for (ptr = headPtr;
			ptr->next_node->next_node != NULL;
			ptr = ptr->next_node);

		ptrDel = ptr->next_node;
		strcpy(str,ptrDel->item);
		ptr->next_node = NULL;
		free(ptrDel);

	}


	return;
}


void listStack(void)
{
	/* print the entire contents of the listStack implementation specific */
	NODE *ptr;


	if (headPtr == NULL)
	{
		fprintf(stdout,"(stack empty)\n");
		return;
	}
	else
	{
		ptr = headPtr;

		while((ptr = ptr->next_node) != NULL)
		{
			printf("%s\n",ptr->item);
		}
	}

}



int lengthStack(void)
{
	int len = 0;
	NODE *ptr;


	if (headPtr == NULL)
	{
		;
	}
	else
	{
		len++;
		for (ptr = headPtr; ptr->next_node != NULL; ptr = ptr->next_node)
			len++;
	}

	return len;
}



void push(char *word)
{
	/* create the node */
	NODE *newNode, *Node;

	if((newNode = malloc(sizeof(NODE))) == NULL)
	{
		fprintf(stderr,"Error: listStack node creation failed due to lack of space.");
		exit(1);
	}


	strcpy(newNode->item, word);
	newNode->next_node = NULL;


	/* add it to the listStack */
	if (headPtr == NULL)
	{
		/* head node now becomes the new node */
		headPtr = newNode;
	}
	else
	{
		/* traverse the listStack to the end and put it there */
		for (Node = headPtr; Node->next_node != NULL; Node = Node->next_node);

		Node->next_node = newNode;
	}
}
