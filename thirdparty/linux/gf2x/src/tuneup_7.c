/* This file is part of the gf2x library.

   Copyright 2007, 2008, 2009, 2010, 2013, 2015
   Richard Brent, Pierrick Gaudry, Emmanuel Thome', Paul Zimmermann

   This program is free software; you can redistribute it and/or modify it
   under the terms of either:
    - If the archive contains a file named toom-gpl.c (not a trivial
    placeholder), the GNU General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.
    - If the archive contains a file named toom-gpl.c which is a trivial
    placeholder, the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.
   
   This program is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
   FITNESS FOR A PARTICULAR PURPOSE.  See the license text for more details.
   
   You should have received a copy of the GNU General Public License as
   well as the GNU Lesser General Public License along with this program;
   see the files COPYING and COPYING.LIB.  If not, write to the Free
   Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
   02110-1301, USA.
*/

/* Tuning program for GF(2)[x] low-level multiplication. */
/* The environment variable GF2X_TUNE_LOWLEVEL_TIME gives the time, in
   microseconds, to spend benching each function. The default is 1000000,
   thus one second.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>

/* gf2x is used for checking correctness -- not otherwise */
#include "gf2x.h"

#include "timing.h"

#include "tuning_undefs_7.h"

// make it somewhat realistic.
#define N       (1 << 16)
#define NMASK   ((N)-1)

#if 7 == 1
extern void gf2x_mul7(unsigned long *c, unsigned long a, unsigned long b);
#else
extern void gf2x_mul7(unsigned long *c, const unsigned long *a, const unsigned long *b);
#endif

int main(int argc, char *argv[])
{
    unsigned long i, *c0, *c, *a, *b;
    uint64_t st;
    char *benchtime;
    uint64_t btime = 1000000;

    char * progname = "me";
    if (argc >= 1) {
        progname = argv[0];
    }
    benchtime = getenv("GF2X_TUNE_LOWLEVEL_TIME");
    if (benchtime != NULL) {
        btime = strtol(benchtime, (char **)NULL, 10);
        if (btime < 1000 || btime > 100000000) {
            fprintf(stderr, "Warning: unrealistic GF2X_TUNE_LOWLEVEL_TIME,  ");
            fprintf(stderr, "set back to default\n");
            btime = 1000000;
        }
    }

    a = (unsigned long *) malloc((N + 7) * sizeof(unsigned long));
    b = (unsigned long *) malloc((N + 7) * sizeof(unsigned long));
    c = (unsigned long *) malloc(2 * 7 * sizeof(unsigned long));
    c0 = (unsigned long *) malloc(2 * 7 * sizeof(unsigned long));

    for (i = 0; i < N + 7; i++) {
        a[i] = (unsigned long) rand();
        b[i] = (unsigned long) rand();
    }

    st = 0;
    for (i = 0; i < 10 && i < N; i++) {
        /* Use this one as a reference implementation */
        gf2x_mul(c0, a + i, 7, b + i, 7);
        st -= microseconds();
#if 7 == 1
        gf2x_mul7 (c, a[i & NMASK], b[i & NMASK]);
#else
        gf2x_mul7 (c, a + (i & NMASK), b + (i & NMASK));
#endif
        st += microseconds();
        if (memcmp(c, c0, 2 * 7 * sizeof(unsigned long)) != 0)
          {
            unsigned long i;

            fprintf (stderr, "Error, computed test values do not match\n");
            fprintf (stderr, "a=");
            for (i = 0; i < 7; i++)
              fprintf (stderr, "%lu ", a[i]);
            fprintf (stderr, "\n");
            fprintf (stderr, "b=");
            for (i = 0; i < 7; i++)
              fprintf (stderr, "%lu ", b[i]);
            fprintf (stderr, "\n");
            fprintf (stderr, "gf2x_mul gives:\n");
            for (i = 0; i < 2 * 7; i++)
              fprintf (stderr, "%lu ", c0[i]);
            fprintf (stderr, "\n");
            fprintf (stderr, "gf2x_mul7 gives:\n");
            for (i = 0; i < 2 * 7; i++)
              fprintf (stderr, "%lu ", c[i]);
            fprintf (stderr, "\n");
            exit (255);
          }
    }

    uint64_t m = i;

    /* arrange for the measure to take at least .1 seconds */
    for( ; ; ) {
        st = -microseconds();
        for (i = 0; i < m; i++) {
#if 7 == 1
            gf2x_mul7 (c, a[i & NMASK], b[i & NMASK]);
#else
            gf2x_mul7 (c, a + (i & NMASK), b + (i & NMASK));
#endif
        }
        st += microseconds();
        if (st >= btime)
            break;
        m *= 3;
    }

    printf("%s : %1.1f ns\n", progname, (double) st / (double) m * 1.0e3);


    free(c0);
    free(c);
    free(b);
    free(a);
    return 0;
}
