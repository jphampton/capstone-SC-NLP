
/* 

grabfromlist.c

C program for grabbing a whole list of USSC decision texts from findlaw

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm grabfromlist.c -o grabfromlist".

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
    char thecommand[MAXLINELENGTH];
    char thefilename[MAXLINELENGTH];
    int volume, invol, i, j, t, len;

    volume = atoi(argv[1]);
    sprintf(thefilename, "volume%d/caselist", volume);
    fp = fopen(thefilename, "r");

    while (fgets(theline, MAXLINELENGTH, fp) != NULL) {
      /* invol = atoi(theline);
      if (invol > 0) { */
      if ( (len=strlen(theline)) > 1) {
	if (theline[len-1]=='\n')
	  theline[len-1] = '\0';
        sprintf(thecommand, "./grabcase %d %s", volume, theline);
        printf("%s\n", thecommand);
        system(thecommand);
      }
    }

    fclose(fp);
    return(0);

}


