
/* 

"lindisc.c"

C program for linear discrimination

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm lindisc.c -o lindisc".

Reference used for beta formula:
statmaster.sdu.dk/courses/st111/module03/index.html#SECTION00031000000000000000

*/

/* #define VERBOSE true */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
#define TWICEMAXNUMFILES 199998

/* Define some global (static) large arrays ... */
double xfull[TWICEMAXNUMFILES][numrefwords+1], yfull[TWICEMAXNUMFILES];
double x[TWICEMAXNUMFILES][numrefwords+1], y[TWICEMAXNUMFILES];
double H[numrefwords+1][TWICEMAXNUMFILES];

main(int argc, char *argv[])
{

    char filename0[200], filename1[200], tmpstring[200];
    FILE *fp0, *fp1;
    int numfiles0, numfiles1;
    int fnum, tstnum, i, j, k, h, w, numcor0, numcor1;
    double matinvrt();
    void matrix_inverse();

    double M[numrefwords+1][numrefwords+1];
    double Minv[numrefwords+1][numrefwords+1];
    double beta[numrefwords+1];
    double linfit;

    /* Confirm number of files to analyse. */
    if (argc != 3) {
        fprintf(stderr, "Error: need two filename arguments.\n");
        exit(1);
    }

    /* Read through first file. */
    if ((fp0=fopen(argv[1],"r"))==NULL) {
        fprintf(stderr, "Unable to read file %s.\n", argv[1]);
	exit(1);
    }
    fgets(tmpstring, 80, fp0);
    numfiles0 = atoi(tmpstring);
    if (numfiles0 > MAXNUMFILES) {
      printf("Oops, number of files (%d) exceeds maximum allowed (%d).\n",
                                numfiles0, MAXNUMFILES);
      exit(1);
    }

    for (fnum=0; fnum<numfiles0; fnum++) {
      yfull[fnum] = -1.0;
      xfull[fnum][0] = 1.0;
      for (i=1; i<=numrefwords; i++) {
        fgets(tmpstring, 80, fp0);
        xfull[fnum][i] = atof(tmpstring);
      }
    }
    fclose(fp0);

    /* Read through second file. */
    if ((fp1=fopen(argv[2],"r"))==NULL) {
        fprintf(stderr, "Unable to read file %s.\n", argv[2]);
	exit(1);
    }
    fgets(tmpstring, 80, fp1);
    numfiles1 = atoi(tmpstring);
    if (numfiles1 > MAXNUMFILES) {
      printf("Oops, number of files (%d) exceeds maximum allowed (%d).\n",
                                numfiles1, MAXNUMFILES);
      exit(1);
    }
    for (fnum=numfiles0; fnum<numfiles0+numfiles1; fnum++) {
      yfull[fnum] = +1.0;
      xfull[fnum][0] = 1.0;
      for (i=1; i<=numrefwords; i++) {
        fgets(tmpstring, 80, fp1);
        xfull[fnum][i] = atof(tmpstring);
      }
    }
    fclose(fp1);

    /* Classify the files. */
    numcor0 = numcor1 = 0;
for (tstnum=0; tstnum<numfiles0+numfiles1; tstnum++) {

    /* Make "reduced" x and y matrices, with tstnum row omitted. */
    for (fnum=0; fnum<numfiles0+numfiles1-1; fnum++) {
      y[fnum] = yfull[fnum+(fnum>=tstnum)];
      x[fnum][0] = 1.0;
      for (i=1; i<=numrefwords; i++) {
        x[fnum][i] = xfull[fnum+(fnum>=tstnum)][i];
      }
    }

    /* WILL APPLY FORMULA: beta = (x^T x)^{-1} x^T y */

    /* Here x is (numfiles0+numfiles1-1) x (numrefwords+1) = h x w. */
    /* And y is h x 1, so beta is w x 1. */
    h = numfiles0+numfiles1-1;
    w = numrefwords+1;

/*
printf("\n\nx matrix is:\n");
for (i=0; i<h; i++) {
for (j=0; j<w; j++) {
printf("%f ", x[i][j]);
}
printf("\n");
}
printf("\n");
*/

    /* First compute M = x^T x, which is w x w. */
    for (i=0; i<w; i++) {
	for (j=0; j<w; j++) {
	    M[i][j] = 0.0;
	    for (k=0; k<h; k++)
	      M[i][j] = M[i][j] + x[k][i] * x[k][j];
	}
    }

/*
printf("\n\nM matrix is:\n");
for (i=0; i<w; i++) {
for (j=0; j<w; j++) {
printf("%f ", M[i][j]);
}
printf("\n");
}
printf("\n");
*/

    /* Then compute Minv = M^{-1}, which is also w x w. */
    /* matinvrt(M, Minv, w); */
    matrix_inverse(M, Minv, w);

    /* Then compute H = Minv x^T, which is w x h. */
    for (i=0; i<w; i++) {
	for (j=0; j<h; j++) {
	    H[i][j] = 0.0;
	    for (k=0; k<w; k++)
	      H[i][j] = H[i][j] + Minv[i][k] * x[j][k];
	}
    }

    /* Then compute beta = H y, which is w x 1. */
    for (i=0; i<w; i++) {
	beta[i] = 0.0;
	for (k=0; k<h; k++)
	  beta[i] = beta[i] + H[i][k] * y[k];
#ifdef VERBOSE
        printf("beta[%d] = %f; ", i, beta[i]);
#endif
    }
    /* printf("\n"); */

    /* See how well it predicts the "tstnum" file's value. */
    linfit = 0.0;
    for (i=0; i<w; i++)
      linfit = linfit + beta[i] * xfull[tstnum][i];
    if (linfit * yfull[tstnum] > 0.0) {
      if (tstnum<numfiles0)
	numcor0++;
      else
	numcor1++;
    }

    /* Output progress report. */
    printf("tstnum=%d,  yfull[%d]=%.1f,  linfit=%f\n",
    			tstnum, tstnum, yfull[tstnum], linfit);

} /* End of "tstnum" for loop. */

    /* Output final results, and terminate. */
    printf("\nSuccess rates:  #0 = %d/%d = %f,  #1 = %d/%d = %f\n\n",
    		numcor0, numfiles0, 1.0*numcor0/numfiles0,
    		numcor1, numfiles1, 1.0*numcor1/numfiles1);
    return(0);

}


void matrix_inverse(double *Min, double *Mout, int actualsize) {

    /* Loop variables */
    int i, j, k;
    /* Sum variables */
    double sum,x;
    
    /*  Copy the input matrix to output matrix */
    for(i=0; i<actualsize*actualsize; i++) {
      Mout[i]=Min[i];
    }
    
    /* Add small value to diagonal if diagonal is zero */
    for(i=0; i<actualsize; i++)
    { 
        j=i*actualsize+i;
        if ( (Mout[j]<1e-12) && (Mout[j]>-1e-12) ) {
	  Mout[j]=1e-12;
	}
    }
    
    /* Matrix size must be larger than one */
    if (actualsize <= 1) return;
    
    for (i=1; i < actualsize; i++) {
        Mout[i] /= Mout[0]; /* normalize row 0 */
    }
    
    for (i=1; i < actualsize; i++)  {
        for (j=i; j < actualsize; j++)  { /* do a column of L */
            sum = 0.0;
            for (k = 0; k < i; k++) {
                sum += Mout[j*actualsize+k] * Mout[k*actualsize+i];
            }
            Mout[j*actualsize+i] -= sum;
        }
        if (i == actualsize-1) continue;
        for (j=i+1; j < actualsize; j++)  {  /* do a row of U */
            sum = 0.0;
            for (k = 0; k < i; k++) {
                sum += Mout[i*actualsize+k]*Mout[k*actualsize+j];
            }
            Mout[i*actualsize+j] = (Mout[i*actualsize+j]-sum)
	    					/ Mout[i*actualsize+i];
        }
    }
    for ( i = 0; i < actualsize; i++ )  /* invert L */ {
        for ( j = i; j < actualsize; j++ )  {
            x = 1.0;
            if ( i != j ) {
                x = 0.0;
                for ( k = i; k < j; k++ ) {
                    x -= Mout[j*actualsize+k]*Mout[k*actualsize+i];
                }
            }
            Mout[j*actualsize+i] = x / Mout[j*actualsize+j];
        }
    }
    for ( i = 0; i < actualsize; i++ ) /* invert U */ {
        for ( j = i; j < actualsize; j++ )  {
            if ( i == j ) continue;
            sum = 0.0;
            for ( k = i; k < j; k++ ) {
                sum += Mout[k*actualsize+j]*( (i==k) ?
						1.0 : Mout[i*actualsize+k] );
            }
            Mout[i*actualsize+j] = -sum;
        }
    }
    for ( i = 0; i < actualsize; i++ ) /* final inversion */ {
        for ( j = 0; j < actualsize; j++ )  {
            sum = 0.0;
            for ( k = ((i>j)?i:j); k < actualsize; k++ ) {
                sum += ( (j==k) ?  1.0 : Mout[j*actualsize+k] )
						* Mout[k*actualsize+i];
            }
            Mout[j*actualsize+i] = sum;
        }
    }
}


