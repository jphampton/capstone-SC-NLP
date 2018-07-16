
/* 

"textvarlog.c"

C program for computing various statistics from USSC decisions.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm textvarlog.c -o textvarlog".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

main(int argc, char *argv[])
{

    char commandstring[999], newnamestring[9];
    FILE *fp;

    /* Confirm number of arguments. */
    if (argc != 3) {
        fprintf(stderr, "textvarvols: Wrong number of arguments.\n");
        exit(1);
    }

    /* Open file of volume justice names. */
    if ((fp = fopen(argv[1],"r")) == NULL) {
	fprintf(stderr, "textvarvols: Unable to read file %s.\n", argv[1]);
	exit(1);
    }

    /* Remove any previous data. */
    sprintf(commandstring, "rm -f %s", argv[2]);
    system(commandstring);

    /* Loop through the strings in the names file. */
    while ( fscanf(fp, "%s", newnamestring) != EOF ) {

        printf("Running 'textvarit %s' ...\n", newnamestring);
        sprintf(commandstring, "(echo %s: ; (./textvarit %s | tail -5 | head -2) ) >> %s", newnamestring, newnamestring, argv[2]);
	system(commandstring);
    }

}


