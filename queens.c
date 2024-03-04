/* queens.c -- program to generate the solutions to the NxN chees queen problem.
 * Author: Luis Colorado <luiscoloradourcola@gmail.com>
 * Date: Mon Mar  4 21:05:17 EET 2024
 * Copyright: (c) 2024 Luis Colorado.  All rights reserved.
 * License: BSDd
 */
#include <stdio.h>

/* as only one queen is needed by row, we can assume a solution in which 
 * board[i] == j means that the queen at row i is positioned at column j
 * (where row and column go from 0 to N-1 where N is the board size)
 * In order to be capable of printing more than one solution per row (to
 * save paper) the possibility of holding upto C solutions (depending of
 * output width) will be handled.
 *
 * Solutions are printed like this:
 * 1)                  2)
 * +---+---+---+---+   +---+---+---+---+
 * |   |\|/|   |   |   |   |   |\|/|   |
 * +---+---+---+---+   +---+---+---+---+
 * |   |   |   |\|/|   |\|/|   |   |   |
 * +---+---+---+---+   +---+---+---+---+
 * |\|/|   |   |   |   |   |   |   |\|/|  
 * +---+---+---+---+   +---+---+---+---+
 * |   |   |\|/|   |   |   |\|/|   |   |  
 * +---+---+---+---+   +---+---+---+---+
 */

void
print_hor_line(unsigned n, unsigned dim)
{
	printf("+");
	char *sep = "";
	for (int board = 0; board < n; board++) {
		printf("%s+", sep);
		sep = "   ";
		for (int col = 0; col < dim; col++) {
			printf("---+");
		}
	}
}

void
print_boardset(
		unsigned n,
		unsigned dim,
		unsigned char a[][dim],
		unsigned first)
{
	/* print the header line */
	/* full board width is dim*4 + 1 */
	unsigned board_w = dim*4 + 1;
	unsigned line_w = n*(bw + 3) - 3;

	/* header line */
	int blank_width = 0;
	for (int col = 0; col < n; col++) {
		int n = printf("%*s%u", blank_width, "", first++);
		blank_width = board_w + 3 - n;
	}
	/* top line */
	print_hor_line(n, dim);
	
