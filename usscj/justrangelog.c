
/* 

"justrangelog.c"

C program for computing various statistics from USSC decisions.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm justrangelog.c -o justrangelog".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

char optchar[] = { 'f', 'l', 'y', 'o' };

main(int argc, char *argv[])
{

    char commandstring[999], newnamestring[9];
    FILE *fp;
    int i;

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
      /* Analyse *all* the justice's decisions. */
      printf("Running 'textvarit %s' ...\n", newnamestring);
      sprintf(commandstring, "(echo ""; echo %s ALL: ; (./textvarit %s | tail -5 | head -2) ) >> %s", newnamestring, newnamestring, argv[2]);
      system(commandstring);
      for (i=0; i<4; i++) {
        /* Analyse the justice's decisions corresponding to optchar[i]. */
        printf("Running 'justrange %s %c' ...\n", newnamestring, optchar[i]);
        sprintf(commandstring, "( (echo ""; echo %s %c: ; (./justrange %s %c | tail -5 | head -2) ) >> %s); mv -f bvals bvals%s%c", newnamestring, optchar[i], newnamestring, optchar[i], argv[2], newnamestring, optchar[i]);
	system(commandstring);
      }
      /* Do bootstrap comparisons. */
      printf("Running %s bootstrap comparisons ...\n", newnamestring);
      sprintf(commandstring, "(echo ""; echo '%s BOOTSTRAP %c versus %c:'; (./bvalcomp bvals%s%c bvals%s%c | tail -5) ) >> %s", newnamestring, optchar[0], optchar[1], newnamestring, optchar[0], newnamestring, optchar[1], argv[2]);
      system(commandstring);
      sprintf(commandstring, "(echo ""; echo '%s BOOTSTRAP %c versus %c:'; (./bvalcomp bvals%s%c bvals%s%c | tail -5) ) >> %s", newnamestring, optchar[2], optchar[3], newnamestring, optchar[2], newnamestring, optchar[3], argv[2]);
      system(commandstring);
    }

    /* Wrap up. */
    printf("Done.\n");
    return(0);

}


