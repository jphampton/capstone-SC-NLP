
/* 

"textvar.c"

C program for computing various statistics from a collection of text files.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm textvar.c -o textvar".

*/

/* #define FRACEXTRA true */

#define NONREFTOO true

double reportthreshold = 99.9;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Specify "function words", as in e.g. Table 2.5-2 on
	page 38 of Mosteller & Wallace, "Applied Bayesian and
	Classical Inference: The Case of The Federalist Papers", 1984 */
int numrefwords = 63;
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
int numfiles;

#define MAXREFWORDS 80
char curword[MAXREFWORDS];
int charsinword, dashing;

double newrfsdsum = 0.0;
double newnormsum = 0.0;

#define BSIZE 100
#define BLENGTH 1000

/* Define various static global arrays of size MAXNUMFILES ... */
int frefwordcount[MAXREFWORDS][MAXNUMFILES], totwordcount[MAXREFWORDS],
			    fnonrefwordcount[MAXNUMFILES];
double fefrac[MAXNUMFILES], fvfrac[MAXNUMFILES],
	    fsecfrac[MAXNUMFILES], fcommafrac[MAXNUMFILES],
	    fsemifrac[MAXNUMFILES],  
	    freffrac[MAXREFWORDS][MAXNUMFILES], fsntlength[MAXNUMFILES];
int ftotwords[MAXNUMFILES], a[MAXNUMFILES];

main(int argc, char *argv[])
{

    char basename[200], strippedname[200], infoname[210];
    char capsit();
    FILE *fptxt, *fpinfo, *fpboot, *fpfrac, *fpnames;
    int totchars, totwords, totsentences, wordsinsentence, vcount,
    		ecount, seccount, commacount, semicount;
    int charcount[256], refwordcount[MAXREFWORDS], sumofrefcounts, nonrefwords;

    double charfrac[256], refwordfrac[MAXREFWORDS], vfrac,
    		efrac, secfrac, commafrac, semifrac, avrsentlength;
    int df, ob, totnonrefwords, matchedref;
    long grandtotwords;
    double ex, chisq;
    int fnum, i, j, bootcount, bnum;
    int t = 5;
    double sdsum, displayvar(), sq();

    /* Determine number of files to analyse. */
    if (argc == 1) {
        /* fprintf(stderr, "Missing filename argument.\n"); */
	printf("(Oops, no files to analyse.)\n");
        exit(1);
    }
    numfiles = argc-1;
    printf("numfiles=%d\n", numfiles);
    if (numfiles > MAXNUMFILES) {
      printf("Oops, number of files (%d) exceeds maximum allowed (%d).\n",
      				numfiles, MAXNUMFILES);
      exit(1);
    }

    /* Create "info" directory if necessary, redirecting stderr. */
    system("mkdir info 2> /dev/null");

/* Loop through the files. */
for (fnum=1; fnum<=numfiles; fnum++) {

    strcpy(basename, argv[fnum]);
    printf("Analysing file #%d:  %s\n", fnum, basename);

    /* Specify info file name. */
    strcpy(infoname, "info/");
    stripstring(strippedname, basename);
    strcat(infoname, strippedname);
    strcat(infoname, ".info");

    /* Open files. */
    if ((fptxt = fopen(basename,"r")) == NULL) {
	fprintf(stderr, "Unable to read file %s.\n", basename);
	exit(1);
    }
    if ((fpinfo = fopen(infoname,"w")) == NULL) {
	fprintf(stderr, "Unable to write to file %s.\n", infoname);
	exit(1);
    }

    /* Set counts to zero. */
    totchars = totwords = nonrefwords = totsentences
    		= charsinword = wordsinsentence = vcount = dashing = 0;
    for (i=0; i<=255; i++)
	charcount[i] = 0;
    for (i=0; i<numrefwords; i++)
	refwordcount[i] = 0;

    /* Loop through input file, counting lots of stuff. */
    while ((t=getc(fptxt))!=EOF) {

	/* Count characters. */
	charcount[t]++;
	totchars++;

	/* Special check for "v." count. */
        if ( (t=='.') && (charsinword==1) && (curword[0]='V') )
	  vcount++;

	/* Check for word stuff. */
	if (isletter(t)) {

	  /* Add to current word. */
	  curword[charsinword] = capsit(t);
	  charsinword++;

	} else if ((charsinword>0) && iswordend(t)) {

	    /* End of word; increment counts. */
	    totwords++;
	    wordsinsentence++;
	    curword[charsinword] = '\0';
	    matchedref = 0;
	    for (i=0; i<numrefwords; i++) {
	      /* if (!strncmp(refword[i], curword, charsinword)) */
	      if ( (charsinword==strlen(refword[i])) &&
			(!strncmp(refword[i], curword, charsinword)) ) {
		if (matchedref) {
		  fprintf(stderr, "Oops, matched two different reference words.\n");
		  exit(1);
		}
	        refwordcount[i] = refwordcount[i] + 1;
		matchedref = 1;
	      }
	    }
	    if (!matchedref)
	      nonrefwords++;

	    /* Check for end of sentence. */
	    if (issentenceend(t)) {
	      totsentences++;
	      wordsinsentence = 0;
	    }

	    charsinword = 0;

	}

        /* Special "dash" fix. */
	if (t=='-')
	  dashing = 1;
	else
	  dashing = 0;

    }  /* End of "while" loop. */

/*
sumofrefcounts = 0;
for (i=0; i<numrefwords; i++) {
  sumofrefcounts = sumofrefcounts + refwordcount[i];
}
printf("%s: TOTWORDS = %d, SUMOFREFCOUNTS=%d\n", argv[fnum], totwords,
			sumofrefcounts);
*/

    /* Special fix for null-length judgements (e.g. "decrees"). */
    if (totchars==0) {
      printf("Warning: null-length decision in file %s.\n", basename);
      totchars = totwords = totsentences = 1;
    } else if (totwords==0) {
      printf("Warning: zero-word decision in file %s.\n", basename);
      totwords = totsentences = 1;
    } else if (totsentences==0) {
      printf("Warning: zero-sentence decision in file %s.\n", basename);
      totsentences = 1;
    }

    /* Compute char fractions. */
    for (i=0; i<=255; i++)
	charfrac[i] = 1.0*charcount[i]/totchars;
    ecount = charcount[69] + charcount[101];
    efrac = charfrac[69] + charfrac[101];

    /* Compute word fractions. */
    for (i=0; i<numrefwords; i++)
	refwordfrac[i] = 1.0*refwordcount[i]/totwords;
    vfrac = 1.0*vcount/totwords;
    seccount = charcount[167];
    secfrac = (1.0*seccount)/totwords;
    commacount = charcount[44];
    commafrac = (1.0*commacount)/totwords;
    semicount = charcount[59];
    semifrac = (1.0*semicount)/totwords;
    avrsentlength = (1.0*totwords)/totsentences;

    /* Output info. */
    fprintf(fpinfo, "\nFILENAME:  %s\n\n", basename);

    for (i=0; i<numrefwords; i++)
	fprintf(fpinfo, "%s:  refwordcount = %d,  refwordfrac = %f\n",
				refword[i], refwordcount[i], refwordfrac[i]);
    fprintf(fpinfo, "\n");
    fprintf(fpinfo, "V.:  vcount = %d,  vfrac = %f\n", vcount, vfrac);
    fprintf(fpinfo, "§:  seccount = %d,  secfrac = %f\n", seccount, secfrac);
    fprintf(fpinfo, "\ntotwords = %d, nonrefwords = %d\n",
				totwords, nonrefwords);

    fprintf(fpinfo, "\ntotsentences = %d;  average sentence length = %f\n",
    	totsentences, avrsentlength);

    fprintf(fpinfo, "\ntotchars = %d\n\n", totchars);
    fprintf(fpinfo, "E or e:  ecount = %d,  efrac = %f\n\n", ecount, efrac);
    fprintf(fpinfo, "comma:  commacount = %d,  commafrac = %f\n\n",
					commacount, commafrac);
    fprintf(fpinfo, "semicolon:  semicount = %d,  semifrac = %f\n\n",
					semicount, semifrac);
    for (i=0; i<=255; i++)
	fprintf(fpinfo, "%d (%c):  charcount = %d,  charfrac = %f\n",
					i, i, charcount[i], charfrac[i]);
    fprintf(fpinfo, "\n\n");

    /* Save to arrays. */

    fefrac[fnum] = efrac;
    fvfrac[fnum] = vfrac;
    fsecfrac[fnum] = secfrac;
    fcommafrac[fnum] = commafrac;
    fsemifrac[fnum] = semifrac;
    fsntlength[fnum] = avrsentlength;
    ftotwords[fnum] = totwords;
    /* fnonrefwordcount[fnum] = totwords; */
    fnonrefwordcount[fnum] = nonrefwords;
    for (i=0; i<numrefwords; i++) {
	freffrac[i][fnum] = refwordfrac[i];
	frefwordcount[i][fnum] = refwordcount[i];
	/* fnonrefwordcount[fnum] = fnonrefwordcount[fnum] - refwordcount[i]; */
    }

    /* Finish up with the file. */
    fclose(fptxt);
    fclose(fpinfo);

} /* End of fnum for loop. */

    /* Display results. */
    printf("\nRESULTS:\n\n");
    sdsum = 0.0;
    for (i=0; i<numrefwords; i++)
      sdsum = sdsum + displayvar(refword[i], freffrac[i], ftotwords);
    printf("\n");
    printf("numfiles=%d\n", numfiles);
    printf("old sum of refword sd's (V1):  %f\n", sdsum);
    printf("new sum of refword sd's (V2):  %f ...  normed (V3): %f\n",
				newrfsdsum, newnormsum);
    displayvar("efrac", fefrac, ftotwords);
    displayvar("vfrac", fvfrac, ftotwords);
    displayvar("secfrac", fsecfrac, ftotwords);
    displayvar("commafrac", fcommafrac, ftotwords);
    displayvar("semifrac", fsemifrac, ftotwords);
    displayvar("sntlength", fsntlength, ftotwords);
    printf("\n");

    /* Do special chi-squared calcuation. */
    grandtotwords = 0;
    for (i=0; i<numrefwords; i++) {
      totwordcount[i] = 0;
      for (fnum=1; fnum<=numfiles; fnum++)
        totwordcount[i] = totwordcount[i] + frefwordcount[i][fnum];
    }
    totnonrefwords = 0;
    for (fnum=1; fnum<=numfiles; fnum++)
      totnonrefwords = totnonrefwords + fnonrefwordcount[fnum];
    for (i=0; i<numrefwords; i++)
      grandtotwords = grandtotwords + totwordcount[i];
    grandtotwords = grandtotwords + totnonrefwords;
    printf("#files=%d, grandtotwords=%ld; average #words/file = %.2f\n",
		numfiles, grandtotwords, ((double)grandtotwords)/numfiles);
    df = (numrefwords)*(numfiles-1);
    chisq = 0.0;
    for (fnum=1; fnum<=numfiles; fnum++) {
      for (i=0; i<numrefwords; i++) {
	ex = totwordcount[i] * (1.0*ftotwords[fnum] / grandtotwords);
/* printf("chisqtmp: i=%d, fnum=%d, totwordcount[i]=%d, ftotwords[fnum]=%d, grandtotwords=%d\n", i, fnum, totwordcount[i], ftotwords[fnum], grandtotwords); */
	ob = frefwordcount[i][fnum];
        if (ex>0) {
          chisq = chisq + sq(ob-ex)/ex;
	  /* Output large contributions to chisq/df ... */
	  if (sq(ob-ex)/ex/df > reportthreshold)
	      printf("(%s, %s, ob=%d, ex=%.2f, contr=%.2f)\n",
			basename, refword[i], ob, ex, sq(ob-ex)/ex/df);
        }
      }
#ifdef NONREFTOO
      /* Add in the non-reference words too. */
      ex = totnonrefwords * (1.0*ftotwords[fnum] / grandtotwords);
      ob = fnonrefwordcount[fnum];
      if (ex>0) {
        chisq = chisq + sq(ob-ex)/ex;
	/* Output large contributions to chisq/df ... */
	if (sq(ob-ex)/ex/df > reportthreshold)
	      printf("(%s, %s, ob=%d, ex=%.2f, contr=%.2f)\n",
			basename, refword[i], ob, ex, sq(ob-ex)/ex/df);
      }
#endif
    }
    printf("df=%d,  chisq=%f,   chisq/df (V4) = %f \n", df, chisq, chisq/df);

    /* Output function word fracs & filenames for later analysis. */
    fpfrac = fopen("thefracs","w");
    fpnames = fopen("thefilenames","w");
    printf("Outputing function word fracs & filenames ...");
    /* fprintf(fpfrac, "reffracs = matrix( c( ", freffrac[i][fnum]); */
    fprintf(fpfrac, "%d\n", numfiles);
    fprintf(fpnames, "%d\n", numfiles);
    for (fnum=1; fnum<=numfiles; fnum++) {
      fprintf(fpnames, "%s\n", argv[fnum]);
      for (i=0; i<numrefwords; i++) {
        fprintf(fpfrac, "%f\n", freffrac[i][fnum]);
	/* if ( (fnum<numfiles) || (i<numrefwords-1) )
          fprintf(fpfrac, ", "); */
      }
#ifdef FRACEXTRA
      fprintf(fpfrac, "%f\n", fsntlength[fnum]);
      fprintf(fpfrac, "%f\n", fcommafrac[fnum]);
      fprintf(fpfrac, "%f\n", fsemifrac[fnum]);
#endif
    }
    /* fprintf(fpfrac, " ), nrow=%d)", numfiles); */
    fclose(fpfrac);
    fclose(fpnames);
    printf(" done.\n");

    /* Do bootstrap chi-squared calcuations. */
    fpboot = fopen("bvals","w");
    printf("Computing bootstrap values ...");
    df = (numrefwords)*(BSIZE-1);
    seedrand();
for (bootcount=0; bootcount<BLENGTH; bootcount++) {
    for (j=0; j<BSIZE; j++)
      a[j] = 1 + ifloor( numfiles * drand48() );
    grandtotwords = 0;
    for (i=0; i<numrefwords; i++) {
      totwordcount[i] = 0;
      for (bnum=0; bnum<BSIZE; bnum++)
        totwordcount[i] = totwordcount[i] + frefwordcount[i][a[bnum]];
    }
    totnonrefwords = 0;
    for (bnum=0; bnum<BSIZE; bnum++)
      totnonrefwords = totnonrefwords + fnonrefwordcount[a[bnum]];
    for (i=0; i<numrefwords; i++)
      grandtotwords = grandtotwords + totwordcount[i];
    grandtotwords = grandtotwords + totnonrefwords;
    chisq = 0.0;
    for (bnum=0; bnum<BSIZE; bnum++) {
      for (i=0; i<numrefwords; i++) {
	ex = totwordcount[i] * (1.0*ftotwords[a[bnum]] / grandtotwords);
	ob = frefwordcount[i][a[bnum]];
        if (ex>0) {
          chisq = chisq + sq(ob-ex)/ex;
	  /* Output large contributions to chisq/df ... */
	  if (sq(ob-ex)/ex/df > reportthreshold)
	      printf("[%s, %s, ob=%d, ex=%.2f, contr=%.2f]\n",
			basename, refword[i], ob, ex, sq(ob-ex)/ex/df);
        }
      }
#ifdef NONREFTOO
      /* Add in the non-reference words too. */
      ex = totnonrefwords * (1.0*ftotwords[a[bnum]] / grandtotwords);
      ob = fnonrefwordcount[a[bnum]];
      if (ex>0) {
        chisq = chisq + sq(ob-ex)/ex;
	/* Output large contributions to chisq/df ... */
	if (sq(ob-ex)/ex/df > reportthreshold)
	      printf("[%s, NON, ob=%d, ex=%.2f, contr=%.2f]\n",
			basename, ob, ex, sq(ob-ex)/ex/df);
      }
#endif
    }
    fprintf(fpboot, "%f\n", chisq/df);
}
    fclose(fpboot);
    printf(" done.\n\n");

    return(0);

}


double displayvar(char thename[], double thearray[], int numwords[])
{
    double thesum, themean, thevar, thesd, theCoV;
    double munum, mu, newsum, newvar, newsd, sq();
    int ii, mudenom;

    printf("%s:", thename);

    thesum = munum = 0.0;
    mudenom = 0;
    for (ii=1; ii<=numfiles; ii++) {
      thesum = thesum + thearray[ii];
      munum = munum + numwords[ii]*thearray[ii];
      mudenom = mudenom + numwords[ii];
    }
    themean = thesum / numfiles;
    mu = munum / mudenom;
    /* printf("  mean=%f/%d=%f", thesum, numfiles, themean); */
    printf("  mean=%f", themean);
    printf(", mu=%f", mu);

    thesum = newsum = 0.0;
    for (ii=1; ii<=numfiles; ii++) {
      thesum = thesum + sq(thearray[ii]-themean);
      newsum = newsum + numwords[ii]*sq( (thearray[ii]-mu) );
    }
    thevar = thesum / (numfiles-1);
    newvar = newsum / (numfiles-1);
    printf(",  var=%f", thevar);
    printf(",  newvar=%f", newvar);
    thesd = sqrt(thevar);
    newsd = sqrt(newvar);
    printf(",  sd=%f", thesd);
    printf(",  newsd=%f", newsd);
    newrfsdsum = newrfsdsum + newsd;
    if (mu>0.0)
      newnormsum = newnormsum + newsd/mu/(1-mu);
    /* newnormsum = newnormsum + newsd/mu/(1-mu)/numfiles; */
    theCoV = thesd/themean;
    printf(",  CoV=%f", theCoV);
    printf("\n");
    return(thesd);
}


char capsit(int cc)
{
  if ( ('a'<=cc) && (cc<='z') )
    return (cc + 'A' - 'a');
  return(cc);
}


isletter(int cc)
{
  if ( ( ('a'<=cc) && (cc<='z') ) || ( ('A'<=cc) && (cc<='Z') ) )
    return(1);
  return(0);
}


iswordend(int cc)
{
  return( (cc=='!') || (cc=='?') || (cc==' ') || (cc=='.') || (cc=='\n')
	  || (dashing && (cc=='-')) );
}


issentenceend(int cc)
{
  if ( (cc=='!') || (cc=='?') )
    return(1);
  if (cc=='.') {
    /* Check for non-sentence-ending abbreviations. */
    if (
           ( (charsinword==2) && (!strncmp("DR", curword, charsinword)) )
        || ( (charsinword==2) && (!strncmp("RD", curword, charsinword)) )
        || ( (charsinword==2) && (!strncmp("ST", curword, charsinword)) )
        || ( (charsinword==2) && (!strncmp("MR", curword, charsinword)) )
        || ( (charsinword==2) && (!strncmp("MS", curword, charsinword)) )
        || ( (charsinword==3) && (!strncmp("MRS", curword, charsinword)) )
        || ( (charsinword==4) && (!strncmp("PROF", curword, charsinword)) )
      )
      return(0);
    else
      return(1);
  }
  return(0);
}


stripstring(char outstring[], char instring[])
{
    int ii;

    for (ii=0; ii <= strlen(instring); ii++) {
      if (instring[ii]=='/')
        outstring[ii] = '-';
      else
        outstring[ii] = instring[ii];
    }
    return(0);
}


double sq(double xxx)
{
  return(xxx*xxx);
}


/* IFLOOR */
ifloor(double x)  /* returns floor(x) as an integer */
{
    return((int)floor(x));
}


/* SEEDRAND: SEED RANDOM NUMBER GENERATOR. */
seedrand()
{
    int i, seed;
    struct timeval tmptv;
    gettimeofday (&tmptv, (struct timezone *)NULL);
    /* seed = (int) (tmptv.tv_usec - 1000000 *
                (int) ( ((double)tmptv.tv_usec) / 1000000.0 ) ); */
    seed = (int) tmptv.tv_usec;
    srand48(seed);
    (void)drand48();  /* Spin it once. */
    return(0);
}


