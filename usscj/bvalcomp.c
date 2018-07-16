
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BLENGTH 1000

main(int argc, char *argv[])
{

    int i, j, numless;
    double vals1[BLENGTH], vals2[BLENGTH], tmpval;
    double sum1, sum2;
    char tmpstring[80];
    FILE *fp1, *fp2, *fpdiff, *fpsort;

    if (argc!=3) {
      fprintf(stderr, "Error, number of arguments must be 2, not %d.\n",
      								argc-1);
      exit(1);
    }

    /* Open files. */
    if ( ((fp1=fopen(argv[1],"r"))==NULL) ||
    			((fp2=fopen(argv[2],"r"))==NULL) ) {
        fprintf(stderr, "error: Unable to read files %s and %s.\n",
						argv[1], argv[2]);
	exit(1);
    }

    /* Read and sum values. */
    sum1 = sum2 = 0.0;
    for (i=0; i<BLENGTH; i++) {
      fgets(tmpstring, 80, fp1);
      vals1[i] = atof(tmpstring);
      sum1 = sum1 + vals1[i];
    }
    for (i=0; i<BLENGTH; i++) {
      fgets(tmpstring, 80, fp2);
      vals2[i] = atof(tmpstring);
      sum2 = sum2 + vals2[i];
    }
    fclose(fp1);
    fclose(fp2);

    /* printf("%f  %f\n", vals1[3], vals2[3]); */

    /* Do the comparison, and write differences to fpdiff. */
    if ((fpdiff=fopen("bvaldiffs","w"))==NULL) {
        fprintf(stderr, "error: Unable to write to bvaldiffs file.\n");
	exit(1);
    }
    numless = 0;
    for (i=0; i<BLENGTH; i++) {
      for (j=0; j<BLENGTH; j++) {
        if (vals1[i]<vals2[j])
	  numless++;
        fprintf(fpdiff, "%f\n", vals2[j]-vals1[i]);
      }
    }
    fclose(fpdiff);

    /* Output the results. */
    printf("mean for %s = %f\n", argv[1], sum1/BLENGTH);
    printf("mean for %s = %f\n", argv[2], sum2/BLENGTH);
    printf("Number of times that %s is less than %s:\n", argv[1], argv[2]);
    printf("  numless=%d     frac = %f\n", numless,
    				1.0 * numless / BLENGTH / BLENGTH);

    /* Sort the differences. */
    printf("Sorting the '%s - %s' differences ...\n", argv[2], argv[1]);
    system("rm -f bvalsorted ; sort -n bvaldiffs > bvalsorted");
    fpsort = fopen("bvalsorted","r");
    for (i=0; i<BLENGTH*BLENGTH*0.025; i++)
      fgets(tmpstring, 80, fpsort);
    printf("lower 95%% confidence interval bound = %f\n", atof(tmpstring));
    for (i=0; i<BLENGTH*BLENGTH*0.95; i++)
      fgets(tmpstring, 80, fpsort);
    printf("upper 95%% confidence interval bound = %f\n", atof(tmpstring));
    fclose(fpsort);

    return(0);

}

