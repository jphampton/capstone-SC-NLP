
/* 

"justrange.c"

C program for computing various statistics from a collection of text files.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm justrange.c -o justrange".

*/

/* #define OUTPUT true */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

main(int argc, char *argv[])
{

    char commandstring[99999], namestring[99], tmpfilename[99];
    FILE *fp;
    int i, found, first, last;
    int vol65a, vol65b, volf5, voll5;

    /* Confirm number of arguments. */
    if (argc != 3) {
        fprintf(stderr, "Wrong number of arguments.\n");
        exit(1);
    }

    /* Open file of volume numbers. */
    if ((fp = fopen("voldata","r")) == NULL) {
	fprintf(stderr, "Unable to read file %s.\n", "voldata");
	exit(1);
    }

    /* Loop through file looking for match. */
    found = 0;
    while ( fscanf(fp, "%s", namestring) != EOF ) {
      fscanf(fp, "%d", &vol65a);
      fscanf(fp, "%d", &vol65b);
      fscanf(fp, "%d", &volf5);
      fscanf(fp, "%d", &voll5);
      if (!strcmp(namestring,argv[1])) {
#ifdef OUTPUT
        printf("matched:  ");
        printf("%s, %d, %d, %d, %d\n",
      				namestring, vol65a, vol65b, volf5, voll5);
#endif
        found = 1;
	break;
      }
    }
    if (!found) {
      fprintf(stderr, "Error: justice name %s not found.\n", argv[1]);
      exit(1);
    }

    /* Prepare to do the analysis. */
#ifdef OUTPUT
    printf("Considering the ");
#endif
    if (argv[2][0]=='f') {
      first = 1;
      last = volf5;
#ifdef OUTPUT
      printf("first 5 years");
#endif
    } else if (argv[2][0]=='l') {
      first = voll5;
      last = 560;
#ifdef OUTPUT
      printf("last 5 years");
#endif
    } else if (argv[2][0]=='y') {
      first = 1;
      last = vol65a;
#ifdef OUTPUT
      printf("pre-age-65 years");
#endif
    } else if (argv[2][0]=='o') {
      first = vol65b;
      last = 560;
#ifdef OUTPUT
      printf("post-age-65 years");
#endif
    } else {
      printf("...\nError: second argument must be 'f', 'l', 'y', or 'o'.\n");
      exit(1);
    }
#ifdef OUTPUT
    printf(" of justice %s: vols %d-%d.\n\n", argv[1], first, last);
#endif

    /* Define the new command. */
    sprintf(tmpfilename, "%stmplist", argv[1]);
    sprintf(commandstring, "rm -f %s; ./createvollist %d %d %s; ./textvarvols %s %s", tmpfilename, first, last, tmpfilename, tmpfilename, argv[1]);
#ifdef OUTPUT
    printf("%s\n", commandstring);
    fflush(stdin);
#endif
    system(commandstring);

}


