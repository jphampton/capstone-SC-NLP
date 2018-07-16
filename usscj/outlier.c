
/* 

"outlier.c"

C program for writing style outlier detection

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm outlier.c -o outlier".

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define PI 3.1415926

#define numrefwords 63

char *refword[] = {
  "A",
  "ALL",
  "ALSO",
  "AN",
  "AND",
  "ANY",
  "ARE",
  "AS",
  "AT",
  "BE",
  "BEEN",
  "BUT",
  "BY",
  "CAN",
  "DO",
  "DOWN",
  "EVEN",
/*
  "EVERY",
*/
  "FOR",
  "FROM",
  "HAD",
  "HAS",
  "HAVE",
  "HER",
  "HIS",
  "IF",
  "IN",
  "INTO",
  "IS",
  "IT",
  "ITS",
  "MAY",
  "MORE",
  "MUST",
/*
  "MY",
*/
  "NO",
  "NOT",
  "NOW",
  "OF",
  "ON",
  "ONE",
  "ONLY",
  "OR",
  "OUR",
/*
  "SHALL",
  "SHOULD",
*/
  "SO",
  "SOME",
  "SUCH",
  "THAN",
  "THAT",
  "THE",
  "THEIR",
  "THEN",
  "THERE",
  "THINGS",
  "THIS",
  "TO",
  "UP",
/*
  "UPON",
*/
  "WAS",
  "WERE",
  "WHAT",
  "WHEN",
  "WHICH",
  "WHO",
/*
  "WILL",
*/
  "WITH",
  "WOULD",
/*
  "YOUR",
  "WHILE",
  "WHILST",
*/
};

#define MAXNUMFILES 99999
double fracs[MAXNUMFILES][numrefwords];
char fname[MAXNUMFILES][999];

main(int argc, char *argv[])
{

    char tmpstring[200];
    FILE *fp, *fpout, *fpnames;
    int numfiles;
    int fnum, i, j, k, len;
    double tmpsum, mean[numrefwords], var[numrefwords];
    double loglike;
    double sq();

    /* Confirm number of files to analyse. */
    if (argc < 2) {
        fprintf(stderr, "Error: missing filename argument.\n");
        exit(1);
    }

    /* Read through frac input file. */
    if ((fp=fopen(argv[1],"r"))==NULL) {
        fprintf(stderr, "Unable to read file %s.\n", argv[1]);
	exit(1);
    }
    fgets(tmpstring, 80, fp);
    numfiles = atoi(tmpstring);
    if (numfiles > MAXNUMFILES) {
      fprintf(stderr, "Oops, number of files (%d) exceeds maximum allowed (%d).\n", numfiles, MAXNUMFILES);
      exit(1);
    }

    printf("Reading through the input files ... ");
    for (fnum=0; fnum<numfiles; fnum++) {
      for (i=0; i<numrefwords; i++) {
        fgets(tmpstring, 80, fp);
        fracs[fnum][i] = atof(tmpstring);
      }
    }
    fclose(fp);
    printf("done.\n");

    /* Read through filenames input file. */
    if (argc > 2) {
      if ((fpnames=fopen(argv[2],"r"))==NULL) {
        fprintf(stderr, "Unable to read file %s.\n", argv[2]);
	exit(1);
      }
      fgets(tmpstring, 80, fp);
      if (numfiles != atoi(tmpstring)) {
        fprintf(stderr, "Oops, number of files in filename file (%d) does not equal number in fracs file (%d).\n", atoi(tmpstring), numfiles);
        exit(1);
      }
      printf("Reading through names input file ...");
      for (fnum=0; fnum<numfiles; fnum++) {
        fgets(tmpstring, 80, fp);
        strcpy(fname[fnum], tmpstring);
	len = strlen(fname[fnum]);
	if (fname[fnum][len-1] == '\n')
	  fname[fnum][len-1] = '\0';
      }
      fclose(fpnames);
      printf("done.\n");
    } else {
      for (fnum=0; fnum<numfiles; fnum++) {
        strcpy(fname[fnum], "");
      }
    }

    /* Prepare output file. */
    if ((fpout=fopen("outliervals","w"))==NULL) {
        fprintf(stderr, "Unable to write to file %s.\n", "outliervals");
	exit(1);
    }

    printf("Computing the log-likelihood outlier scores of each file ...\n");
  for (k=0; k<numfiles; k++) {
    /* Consider predicting case k of justice j. */

    /* Compute means and vars. */
    for (i=0; i<numrefwords; i++) {
      tmpsum = 0.0;
      for (fnum=0; fnum<numfiles; fnum++) {
        if (fnum!=k)
          tmpsum = tmpsum + fracs[fnum][i];
      }
      mean[i] = tmpsum / (numfiles-1);
      tmpsum = 0.0;
      for (fnum=0; fnum<numfiles; fnum++) {
        if (fnum!=k)
          tmpsum = tmpsum + sq( fracs[fnum][i] - mean[i] );
      }
      var[i] = tmpsum / (numfiles-2);
      /* printf("%s: mean=%f, var=%f\n", refword[i], mean[i], var[i]); */
    }

    /* Compute the log-likelihood. */
    loglike = 0.0;
    for (i=0; i<numrefwords; i++) {
        loglike = loglike - 0.5*log(2*PI*var[i])
      			- sq( fracs[k][i] - mean[i] ) / 2 / var[i];
    }
    printf("file #%d (%s):  loglike=%f\n", k+1, fname[k], loglike);
    fprintf(fpout, "%8d:  %10.4f   (%s)\n", k+1, loglike, fname[k]);

  } /* End of "k" for statement. */

    fclose(fpout);
    printf("\n\nTop outlier candidates:\n");
    fflush(stdout);
    /* system("sort -n --key=2 outliervals | head -20 | nl -n ln"); */
    system("sort -n --key=2 outliervals | nl -n ln");
    /* printf("\nDone.\n\n"); */
    fflush(stdout);
    return(0);

}


double sq(double xx)
{
  return(xx*xx);
}


