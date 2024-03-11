/* queens.c -- program to generate the solutions to the NxN chees queen problem.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Mon Mar  4 21:05:17 EET 2024
 * Copyright: (c) 2024 Luis Colorado.  All rights reserved.
 * License: BSDd
 */

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_WIDTH  (80)
#define DEFAULT_DIM    (8)

/* CELL_SZ == strlen(HOR_LINE) && CELL_SZ == strlen(QUEEN)
 *     && CELL_SZ == strlen(EMPTY) */
#define CELL_SZ        (3)
#define HOR_LINE       "---"
#define QUEEN          "(@)"
#define EMPTY          "   "

/* BORDER_SZ must be equal to strlen(VERT_LINE) & strlen(CORNER) */
#define BORDER_SZ      (1)
#define VERT_LINE      "|"
#define CORNER         "+"
#define GAP_SZ         (3)
#define GAP            "   "

#define F(_fmt) "%s:%d:%s: "_fmt, __FILE__, __LINE__, __func__

#define DEB_TAIL(_dfmt, ...) do { \
        if (deb_f) {              \
            fprintf(deb_f,        \
                _dfmt,            \
                ##__VA_ARGS__);   \
        }                         \
    } while (0)

#define DEB(_dfmt, ...) DEB_TAIL(F(_dfmt), ##__VA_ARGS__)

#define WRN_TAIL(_wfmt, ...) \
    fprintf(stderr, _wfmt, ##__VA_ARGS__)

#define WRN(_wfmt, ...) WRN_TAIL(F(_wfmt), ##__VA_ARGS__)

/* as only one queen is needed by row, we can assume a solution in which
 * board[i] == j means that the queen at row i is positioned at column j
 * (where row and column go from 0 to N-1 where N is the board size)
 * In order to be capable of printing more than one solution per row (to
 * save paper) the possibility of holding upto C solutions (depending of
 * output width) will be handled.
 */

typedef unsigned char CELL_T;

CELL_T      **boards;
unsigned      N        = DEFAULT_WIDTH,
              DIM      = DEFAULT_DIM,
              SIZ      = 0;
unsigned long SOLUTION = 1;

FILE         *deb_f    = NULL;

/* prints an horizontal line, like this:
 * +---+---+---+---+   +---+---+---+---+
 */
void
print_hor_line(void)
{
    char *sep = "";
    for (int board = 0; board < SIZ; board++) {
        printf("%s" CORNER, sep);
        sep = GAP;
        for (int col = 0; col < DIM; col++) {
            printf(HOR_LINE CORNER);
        }
    }
    printf("\n");
} /* print_hor_line */

/* Solutions are printed like this:
 * 1)                  2)
 * +---+---+---+---+   +---+---+---+---+
 * |   |\X/|   |   |   |   |   |\X/|   |
 * +---+---+---+---+   +---+---+---+---+
 * |   |   |   |\X/|   |\X/|   |   |   |
 * +---+---+---+---+   +---+---+---+---+
 * |\X/|   |   |   |   |   |   |   |\X/|
 * +---+---+---+---+   +---+---+---+---+
 * |   |   |\X/|   |   |   |\X/|   |   |
 * +---+---+---+---+   +---+---+---+---+
 * print_boardset() is in charge of that.
 */
unsigned long
print_boardset(void)
{
    /* if nothing to do, just return */
    if (!SIZ)
        return SOLUTION;

    /* full board width is */
    static unsigned board_w = DIM * (CELL_SZ + BORDER_SZ)
                     + BORDER_SZ;

    /* so full line width is */
    static unsigned line_w  = N * (board_w + GAP_SZ)
                     - GAP_SZ;

    /* header line */
    int blank_width = 0;
    for (int sol = 0; sol < SIZ; sol++) {
        printf("%*s", blank_width, "");
        int n = printf("%lu:", SOLUTION++);
        blank_width = board_w + GAP_SZ - n;
    }
    printf("\n");

    /* top line */
    print_hor_line();

    for (int row = 0; row < DIM; row++) {
        char *sep = VERT_LINE;
        for (int sol = 0; sol < SIZ; sol++) {
            printf("%s", sep); sep = GAP VERT_LINE;

            /* the cells */
            for (int col = 0; col < DIM; col++) {
                printf("%s" VERT_LINE,
                    (boards[sol][row] == col
                        ? QUEEN
                        : EMPTY));
            }
        }
        printf("\n");
        print_hor_line();
    }
    printf("\n");
    SIZ = 0;

    return SOLUTION;
} /* print_boardset */

/* Adds a new solution to the boardset above.  The
 * solution is specified in parameter B.  If the
 * boardset is full, then a call to print_boardset()
 * if made to print the set of solutions found. */
void
add_solution(
        CELL_T *B)
{
    DEB("Found sol #%lu: {", SOLUTION+SIZ);
    char *sep = "";
    for (int row = 0; row < DIM; row++) {
        DEB_TAIL("%s<%u|%u>", sep, row, B[row]);
        sep = ", ";
    }
    DEB_TAIL("};\n");

    /* copy the solution */
    memcpy(boards[SIZ++], B, DIM * sizeof *B);

    if (SIZ == N) {
        print_boardset();
    }
} /* add_solution */

/* The next function checks recursively for a cell
 * array B, assuming that rows < R have already been
 * set, tries to find a position for a QUEEN at row
 * R.  If it finds a valid position, the routine is
 * called recursively at row R + 1.  Finally, if R
 * equals DIM, the solution is added to the solution
 * buffer through a call to add_solution(B).
 */
void
check_boards(unsigned R) /* row */
{
    /* alloc this statically for all instances
     * of this function */
    static CELL_T   *B   = NULL;
    static int      *maj = NULL;
    static unsigned *min = NULL;
    if (!B) {
        B   = calloc(DIM, sizeof *B);
        maj = calloc(DIM, sizeof *maj);
        min = calloc(DIM, sizeof *min);
    }

    for (int col = 0; col < DIM; col++) {
        /* set filled diagonals and cell */
        maj[R] = R - col; /* major */
        min[R] = R + col; /* minor */
        B[R]   =     col; /* column */

        DEB("Trying <%d|%d>\n", R, col);
        int row;
        for (row = 0; row < R; row++) {
            DEB("  checking against "
                "<%u|%u>\n",
                row, B[row]);
            if (B[row] == B[R]) {
                /* same column */
                DEB("  same column "
                    "than <%u|%u>\n",
                    row, B[row]);
                break;  /* exit loop */
            }
            if (maj[row] == maj[R]) {
                /* same major diagonal */
                DEB("  same maj. diag. "
                    "than <%u|%u>\n",
                    row, B[row]);
                break;
            }
            if (min[row] == min[R]) {
                /* same minor diagonal */
                DEB("  same min. diag. "
                    "than <%u|%u>\n",
                    row, B[row]);
                break;
            }
        } /* for */
        if (row < R) {
            /* if we exited loop prematurely, some other
             * queen attacks this position */
            continue;
        }
        if (R + 1 == DIM) {
            add_solution(B);
        } else {
            DEB("Accepted, trying next row (%u)\n", R + 1);
            check_boards(R + 1);
        }
    } /* for */
} /* check_boards */

int main(int argc, char **argv)
{
    unsigned width = DEFAULT_WIDTH;

    char *w_str = getenv("COLUMNS");
    if (w_str) {
        width = strtoul(w_str, NULL, 10);
    }

    int opt;
    while ((opt = getopt(argc, argv, "dn:w:")) != EOF) {
        switch (opt) {
        case 'd': deb_f = stderr;                    break;
        case 'n': DIM   = strtoul(optarg, NULL, 10); break;
        case 'w': width = strtoul(optarg, NULL, 10); break;
        }
    } /* while */

    N = (width + GAP_SZ) / ((CELL_SZ + BORDER_SZ) * DIM + BORDER_SZ + GAP_SZ);
    if (!N) {
        N++;
        WRN("Solution will be wider(%d) than width(%u)\n",
            (CELL_SZ + BORDER_SZ) * DIM + BORDER_SZ,
            width);
    }

    /* get the memory for the full set */

    boards = calloc(N, sizeof boards[0]);

    /* boards initialization */
    for (int i = 0; i < N; i++) {
        boards[i] = calloc(DIM, sizeof *boards[i]);
    }


#define P(_arg) DEB("%-10s: %u\n", #_arg, (_arg))
    P(DIM);
    P(width);
    P(N);
#undef P

    check_boards(0);

    unsigned long sol = print_boardset();

    printf("%lu solutions\n", sol-1);
} /* main */
