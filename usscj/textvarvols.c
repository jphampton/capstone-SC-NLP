
/* 

"textvar.c"

C program for computing various statistics from a collection of text files.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm textvarvols.c -o textvarvols".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

main(int argc, char *argv[])
{

    char commandstring[99999], newargstring[99], newvolstring[9];
    char dirteststring[999];
    FILE *fp;
    int i;
    int t = 5;

    /* Confirm number of arguments. */
    if (argc != 3) {
        fprintf(stderr, "textvarvols: Wrong number of arguments.\n");
        exit(1);
    }

    /* Open file of volume numbers. */
    if ((fp = fopen(argv[1],"r")) == NULL) {
	fprintf(stderr, "textvarvols: Unable to read file %s.\n", argv[1]);
	exit(1);
    }

    /* Check for special "-" option. */
    if (!strcmp(argv[2], "-"))
      strcpy(argv[2], "*");

    /* Begin to define the new command. */
    strcpy(commandstring, "./textvar");

    /* Loop through the strings in the volumes file. */
    while ( fscanf(fp, "%s", newvolstring) != EOF ) {
	/* Check if the corresponding directory exists. */
	if ( (!strcmp(argv[2],"*")) )
          sprintf(dirteststring, "test -d volume%s", newvolstring);
	else
          sprintf(dirteststring, "test -d volume%s/%s", newvolstring, argv[2]);
	if ( (!system(dirteststring)) ) {
          sprintf(newargstring, "    volume%s/%s/thecase* \\\n",
				newvolstring, argv[2]);
          strcat(commandstring, newargstring);
	}
    }

    /* Output and run the new command. */
    printf("%s\n", commandstring);
    fflush(stdout);
    system(commandstring);

}


