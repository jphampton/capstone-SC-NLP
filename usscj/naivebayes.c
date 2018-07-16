
/* 

"naivebayes.c"

C program for naive Bayes classification

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm naivebayes.c -o naivebayes".

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
double fracs[2][MAXNUMFILES][numrefwords];

main(int argc, char *argv[])
{

    char tmpstring[200];
    FILE *fp[2];
    int numfiles[2];
    int fnum, i, j, k, m, numcor[2];
    double tmpsum, mean[2][numrefwords], var[2][numrefwords];
    double loglike[2];
    double sq();

    /* Confirm number of files to analyse. */
    if (argc != 3) {
        fprintf(stderr, "Error: need two filename arguments.\n");
        exit(1);
    }

    /* Read through files. */
for (j=0; j<=1; j++) {
    if ((fp[j]=fopen(argv[j+1],"r"))==NULL) {
        fprintf(stderr, "Unable to read file %s.\n", argv[j+1]);
	exit(1);
    }
    fgets(tmpstring, 80, fp[j]);
    numfiles[j] = atoi(tmpstring);
    if (numfiles[j] > MAXNUMFILES) {
      printf("Oops, %d'th number of files (%d) exceeds maximum allowed (%d).\n",
                                j, numfiles[j], MAXNUMFILES);
      exit(1);
    }

    for (fnum=0; fnum<numfiles[j]; fnum++) {
      for (i=0; i<numrefwords; i++) {
        fgets(tmpstring, 80, fp[j]);
        fracs[j][fnum][i] = atof(tmpstring);
      }
    }
    fclose(fp[j]);
}

for (j=0; j<=1; j++) {
  numcor[j] = 0;
  for (k=0; k<numfiles[j]; k++) {
    /* Consider predicting case k of justice j. */

    /* Compute means and vars. */
    for (i=0; i<numrefwords; i++) {
     for (m=0; m<=1; m++) {
      tmpsum = 0.0;
      for (fnum=0; fnum<numfiles[m]; fnum++) {
        if ((m!=j) || (fnum!=k))
          tmpsum = tmpsum + fracs[m][fnum][i];
      }
      mean[m][i] = tmpsum / (numfiles[m]-(m==j));
      tmpsum = 0.0;
      for (fnum=0; fnum<numfiles[m]; fnum++) {
        if ((m!=j) || (fnum!=k))
          tmpsum = tmpsum + sq( fracs[m][fnum][i] - mean[m][i] );
      }
      var[m][i] = tmpsum / (numfiles[m]-1-(m==j));
     }
     /*
     printf("%s: mean0=%f, var0=%f, mean1=%f, var1=%f\n",
		    refword[i], mean[0][i], var[0][i], mean[1][i], var[1][i]);
     */
    }

    /* Compute the log-likelihoods. */
    for (m=0; m<=1; m++) {
      loglike[m] = 0.0;
      for (i=0; i<numrefwords; i++) {
        loglike[m] = loglike[m] - 0.5*log(2*PI*var[m][i])
      			- sq( fracs[j][k][i] - mean[m][i] ) / 2 / var[m][i];
      }
    }
    printf("file %d-%d:  loglike0=%f, loglike1=%f;  classification: ",
					    j, k, loglike[0], loglike[1]);
    if (loglike[j] > loglike[1-j]) {
      printf("%d", j);
      numcor[j]++;
    } else {
      printf("%d", 1-j);
    }
    printf("\n");

  } /* End of "k" for statement. */
} /* End of "j" for statement. */

    for (j=0; j<=1; j++)
      printf("Justice #%d number correct = %d/%d = %f\n",
			j, numcor[j], numfiles[j], 1.0*numcor[j]/numfiles[j]);

    return(0);

}


double sq(double xx)
{
  return(xx*xx);
}


