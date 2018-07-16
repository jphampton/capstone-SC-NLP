
/* 

"yearlyrun.c"

C program for computing various statistics from USSC decisions.

by Jeffrey Rosenthal (probability.ca), 2009

Compile with "cc -lm yearlyrun.c -o yearlyrun".

*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Define yearly volume starts from 1870 on ... */
int ystart[] = { 
	74, 76, 77, 80, 84, 87, 91, 93, 95, 97, 100, 102, 104, 106, 109,
	112, 116, 119, 124, 129, 132, 137, 142, 147, 151, 155, 160, 164,
	168, 172, 175, 179, 183, 187, 192, 196, 200, 204, 207, 211, 215,
	219, 222, 226, 231, 235, 239, 242, 245, 248, 251, 254, 257, 260,
	263, 266, 269, 272, 275, 278, 280, 281, 284, 287, 290, 293, 296,
	299, 302, 305, 308, 311, 314, 317, 320, 323, 326, 329, 332, 335,
	338, 340, 342, 344, 346, 348, 350, 352, 355, 358, 361, 364, 368,
	371, 375, 379, 382, 385, 389, 393, 396, 400, 403, 409, 414, 419,
	423, 429, 434, 439, 444, 449, 454, 459, 464, 469, 474, 479, 484,
	488, 493, 498, 502, 506, 510, 513, 516, 519, 522, 525, 528, 531,
	534, 537, 540, 543, 546, 549, 552, 555, 562
};

char tmplistname[] = "mytmplist";

main(int argc, char *argv[])
{

    char commandstring[999], newnamestring[9];
    FILE *fp;
    int i;

    /* Confirm number of arguments. */
    if (argc != 2) {
        fprintf(stderr, "textvarvols: Wrong number of arguments.\n");
        exit(1);
    }

    /* Loop through the years. */
    for (i=0; i<140; i++) {
	printf("\n%s, year = %d (volumes %d - %d):\n", argv[1],
		1870+i, ystart[i], ystart[i+1]-1);
	fflush(stdout);
        sprintf(commandstring, "./createvollist %d %d %s ; ./textvarvols %s %s | tail -5 | head -2", ystart[i], ystart[i+1]-1, tmplistname, tmplistname, argv[1]);
	system(commandstring);
    }

}


