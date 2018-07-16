
/* 

"decaderun.c"

C program for computing various statistics from USSC decisions.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm decaderun.c -o decaderun".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

int dstart[] = { 
	2, 4, 10, 18, 27, 39, 49, 65, 74, 100, 132, 175, 215, 251,
	280, 308, 338, 361, 396, 444, 493, 528, 562
};

char tmplistname[] = "mytmplist";

main(int argc, char *argv[])
{

    char commandstring[999], newnamestring[9];
    FILE *fp;
    int i;

    /* Confirm number of arguments. */
    if (argc != 2) {
        fprintf(stderr, "textvarvols: Wrong number of arguments.\n");
        exit(1);
    }

    /* Loop through the decades. */
    for (i=0; i<22; i++) {
	printf("\n%s, decade = %ds (volumes %d - %d):\n", argv[1],
		1790+10*i, dstart[i], dstart[i+1]-1);
	fflush(stdout);
        sprintf(commandstring, "./createvollist %d %d %s ; ./textvarvols %s %s | tail -5 | head -2", dstart[i], dstart[i+1]-1, tmplistname, tmplistname, argv[1]);
	system(commandstring);
    }

}


