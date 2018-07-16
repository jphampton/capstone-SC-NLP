
/* 

sortbyauthor.c

C program for sorting USSC decision texts by author

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm sortbyauthor.c -o sortbyauthor".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAXLINELENGTH 240

/* BEGIN MAIN PROGRAM. */

int main(int argc, char **argv)
{

    /* Initial declarations. */
    FILE *fp;
    char theline[MAXLINELENGTH];
    char theword[MAXLINELENGTH];
    char thecommand[MAXLINELENGTH];
    char thefilename[MAXLINELENGTH];
    int t, wordlength, goodsofar, foundjustice, foundchief;

    /* Open the file. */
    if ( (fp = fopen(argv[1], "r")) == NULL ) {
      fprintf(stderr, "Unable to open file %s.\n", argv[1]);
      exit(1);
    }

    /* Scan tokens from this file until one follows "Justice". */
    wordlength = foundjustice = 0;
    goodsofar = 1;
    while (1) {

      /* Read the next character from the file. */
      t = fgetc(fp);
      // printf("%c  %d  %d\n", t, wordlength, goodsofar);

      if ( ((t >= 'A') && (t <= 'Z')) ) {

	/* New letter of possible Justice name. */
        theword[wordlength] = t;
	wordlength++;

      } else if ( ((t >= 'a') && (t <= 'z')) ) {

	/* New letter of possible Justice name. */
        theword[wordlength] = t + 'A' - 'a';
	wordlength++;

      } else if ( (t==' ') && (wordlength==3)
      		&& ( (!strncmp(theword,"VAN",3))
      					|| (!strncmp(theword,"van",3)))
		) {

        theword[wordlength] = '_';
	wordlength++;

      } else if (t=='\'') {

        /* Ignore apostrophes. */

      } else if ( (!strncmp(theword,"Justice",7))
			|| (!strncmp(theword,"JUSTICE",7)) ) {

        /* We found the word Justice or JUSTICE. */
	wordlength = 0;
	goodsofar = foundjustice = 1;

      } else if ( (!strncmp(theword,"Chief",5))
			|| (!strncmp(theword,"CHIEF",5)) ) {

        /* We found the word Chief or CHIEF. */
	wordlength = 0;
	goodsofar = foundchief = 1;

      } else if ( goodsofar && foundjustice && (wordlength>2) ) {

	/* We found an acceptable Justice name. */
        theword[wordlength] = '\0';
	/* Check for "Chief Justice delivered. */
	if (foundchief && (!strncmp(theword,"DELIVERED",9)))
	  strcpy(theword,"CHIEF");
	/* Execute the move command. */
        sprintf(thecommand, "mkdir %s 2> /dev/null", theword);
        /* printf("%s\n", thecommand); */
        system(thecommand);
        sprintf(thecommand, "mv -f %s %s", argv[1], theword);
        printf("%s\n", thecommand);
        system(thecommand);
	break;

      } else if (t=='\n') {

        /* We reached the end of the first line without success. */
	if (foundchief) {
          sprintf(thecommand, "mkdir %s 2> /dev/null", "CHIEF");
          system(thecommand);
          sprintf(thecommand, "mv -f %s %s", argv[1], "CHIEF");
          printf("%s\n", thecommand);
          system(thecommand);
	  break;
	} else {
          printf("WARNING: no author found for %s.\n", argv[1]);
	  break;
	}

      } else {

        /* We're at the end of a word, without success. */
	wordlength = 0;
	goodsofar = 1;

      }

    }

    fclose(fp);
    return(0);

}



