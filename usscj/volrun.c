
/* 

"volrun.c"

C program for computing various statistics from USSC decisions.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm volrun.c -o volrun".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

    /* Check for special "-" option. */
    if (!strcmp(argv[1], "-"))
      strcpy(argv[1], "*");

    /* Loop through the volumes. */
    for (i=1; i<=561; i++) {
	printf("\n%s, volume = %d:\n", argv[1], i);
	fflush(stdout);
        /* Check if the corresponding directory exists. */
        if ( (!strcmp(argv[1],"*")) )
          sprintf(commandstring, "test -d volume%d", i);
        else
          sprintf(commandstring, "test -d volume%d/%s", i, argv[1]);
        if ( (!system(commandstring)) ) {
          sprintf(commandstring, "./textvar volume%d/%s/thecase* | tail -5 | head -2", i, argv[1]);
	  system(commandstring);
        } else {
	  printf("(No files to analyse.)\n");
	}
    }

}


