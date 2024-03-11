/* knight.c -- program to generate the solutions to the NxN chees knight
 *             ride  problem.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Fri Mar  8 13:57:08 EET 2024
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

typedef short CELL_T;

/* board set boards[sol][row][col] is a CELL_T */
CELL_T     ***boards;

int           N        = DEFAULT_WIDTH,
              DIM      = DEFAULT_DIM,
              SIZ      = 0,
              cell_sz,
              board_w,
              line_w;
unsigned long SOLUTION = 0UL;

FILE         *deb_f    = NULL;

/* prints an horizontal line, like this:
 * +---+---+---+---+   +---+---+---+---+ */
void
print_hor_line(void)
{
    char *sep = "";
    for (int board = 0; board < SIZ; board++) {
        printf("%s" CORNER, sep);
        sep = GAP;
        for (int col = 0; col < DIM; col++) {
            printf("%.*s" CORNER,
                cell_sz,
                "-------------");
        }
    }
    printf("\n");
} /* print_hor_line */

/* Solutions are printed like this:
 * 1)                 2)
 * +--+--+--+--+--+   +--+--+--+--+--+
 * | 1|14|9 |20| 3|   | 7|18|13|24| 1|
 * +--+--+--+--+--+   +--+--+--+--+--+
 * |24|19| 2|15|10|   |12|25| 8|19|14|
 * +--+--+--+--+--+   +--+--+--+--+--+
 * |13| 8|23| 4|21|   |17| 6|23| 2| 9|
 * +--+--+--+--+--+   +--+--+--+--+--+
 * |18|25| 6|11|16|   |22|11| 4|15|20|
 * +--+--+--+--+--+   +--+--+--+--+--+
 * | 7|12|17|22| 5|   | 5|16|21|10| 3|
 * +--+--+--+--+--+   +--+--+--+--+--+
 * print_boardset() is in charge of that.
 */
unsigned long
print_boardset(void)
{
    /* if nothing to do, just return */
    if (!SIZ)
        return SOLUTION;

    /* header line */
    int blank_width = 0;
    for (int sol = 0; sol < SIZ; sol++) {
        printf("%*s", blank_width, "");
        int n = printf("%lu:", ++SOLUTION);
        blank_width = board_w + GAP_SZ - n;
    }
    printf("\n");

    /* top line */
    print_hor_line();

    for (int row = 0; row < DIM; row++) {
        char *sep = VERT_LINE;
        for (int sol = 0; sol < SIZ; sol++) {
            printf("%s", sep);
            sep = GAP VERT_LINE;

            /* the cells */
            for (int col = 0; col < DIM; col++) {
                printf("%*u" VERT_LINE,
                        cell_sz,
                        boards[sol][row][col]);
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
        CELL_T **B)
{
    DEB("got a solution\n");
    if (!boards) {
        boards = calloc(N, sizeof *boards);
        for (int b = 0; b < N; b++) {
            boards[b] = calloc(DIM, sizeof *boards[b]);
            for (int r = 0; r < DIM; r++) {
                boards[b][r] = calloc(DIM,
                        sizeof *boards[b][r]);
            }
        }
    }
    /* copy the solution */
    for (int row = 0; row < DIM; row++) {
        for (int col = 0; col < DIM; col++) {
            boards[SIZ][row][col] = B[row][col];
        }
    }
    SIZ++;
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
check_boards(int row, int col, int mov) /* pos */
{
    /* alloc this statically for all instances
     * of this function */

    static CELL_T  **B   = NULL;
    static unsigned  DIM2;

    /* movements to be done */
    const static struct {
        int dr, dc;
    } movements[] = {
        { .dr = +1, .dc = +2 },
        { .dr = +2, .dc = +1 },
        { .dr = +2, .dc = -1 },
        { .dr = +1, .dc = -2 },
        { .dr = -1, .dc = -2 },
        { .dr = -2, .dc = -1 },
        { .dr = -2, .dc = +1 },
        { .dr = -1, .dc = +2 },
    };

#define MOVEMENTS (sizeof movements / sizeof movements[0])

    if (!B) { /* init */
        B   = calloc(DIM, sizeof *B);
        for (int r = 0; r < DIM; r++) {
            B[r] = calloc(DIM, sizeof *B[r]);
        }
        DIM2 = DIM * DIM;
    }

    B[row][col] = mov;
    if (mov == DIM2) {
        add_solution(B);
    } else {
        for (int dir = 0; dir < MOVEMENTS; dir++) {
            int pos_nxt_r = row + movements[dir].dr;
            int pos_nxt_c = col + movements[dir].dc;
            if (       pos_nxt_r < 0 || pos_nxt_r >= DIM
                    || pos_nxt_c < 0 || pos_nxt_c >= DIM)
            {
                DEB("%*s%d: <%d|%d> --pos_next[%d]--> "
                    "fuera del tablero (<%d|%d>)\n",
                    mov, "",
                    mov,
                    row, col,
                    dir,
                    pos_nxt_r, pos_nxt_c);
                continue;
            }
            if (B[pos_nxt_r][pos_nxt_c]) {
                DEB("%*s%d: <%d|%d> --pos_next[%d]--> "
                    "<%d|%d> already visited (==%d)\n",
                    mov, "",
                    mov,
                    row, col,
                    dir,
                    pos_nxt_r, pos_nxt_c,
                    B[pos_nxt_r][pos_nxt_c]);
                continue;
            }
            DEB("%*s%d: <%d|%d> --[%d]--> <%d|%d>\n",
                mov, "",
                mov, row, col,
                dir,
                pos_nxt_r, pos_nxt_c);
            check_boards(pos_nxt_r, pos_nxt_c, mov+1);
            DEB("%*s%d: <%d|%d> <--[%d]-- <%d|%d>\n",
                mov, "",
                mov, row, col,
                dir ^ 4,
                pos_nxt_r, pos_nxt_c);
        }
    }
    B[row][col] = 0;
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

    cell_sz = snprintf(NULL, 0, "%d", DIM*DIM);

    N = (width + GAP_SZ) / ((cell_sz + BORDER_SZ) * DIM + BORDER_SZ + GAP_SZ);
    if (!N) {
        N++;
        WRN("Solution will be wider(%d) than width(%u)\n",
                    (CELL_SZ + BORDER_SZ) * DIM + BORDER_SZ,
                    width);
    }

    /* full board width is */
    board_w = DIM * (cell_sz + BORDER_SZ) + BORDER_SZ;

    /* so full line width is */
    line_w  = N * (board_w + GAP_SZ) - GAP_SZ;

#define P(_arg) DEB("%-10s: %u\n", #_arg, (_arg))
    P(DIM);
    P(width);
    P(N);
    P(cell_sz);
    P(board_w);
    P(line_w);
#undef P

    int dim1_2 = (DIM + 1) / 2;
    for (int r = 0; r < dim1_2; r++)
        for (int c = r; c < dim1_2; c++)
            check_boards(r, c, 1);

    unsigned long sol = print_boardset();

    printf("%lu solutions\n", sol);
} /* main */
