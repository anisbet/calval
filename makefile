#[File: makefile.bcc]
#
# Makefile for Borland C++ compiler
#	Project: Calval.exe
#	Version:	1.5002
#
CC=bcc
#
#Flags
#	-N -- Check for stack overflow
#	-v -- Enable debugging
#	-w -- Turn on all warnings
#	-ml -- Large model
#	-6 -- Pentium optimization, default (no flag) 386 optimization.
CFLAGS=-n -v -w -6

calval.exe: 
	$(CC) $(CFLAGS) calval.c validatecal.c readdir.c parse.c stack.c
	
	
clean:
	erase *.obj
	erase *.tds

proper:
	erase calval.exe
	erase *.obj
	erase *.tds