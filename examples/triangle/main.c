/* Copyright (C) 2019 John Törnblom

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING. If not, see
<http://www.gnu.org/licenses/>.  */


#include <stdio.h>

#include "jtmut.h"


/**
 * Write the outcome of a tested mutant to a file handle.
 **/
void write_outcome_as_csv(void* ctx, jtmut_id id, int status) {
  FILE *fp = (FILE*)ctx;

  fprintf(fp,
	  "0x%lx%lx, %d\n",
	  (long unsigned int) (id >> 64),
	  (long unsigned int) id,
	  status);
  fflush(stdout);
}


int main() {
  fprintf(stdout, "#id, status\n");
  fflush(stdout);

  jtmut_launch(write_outcome_as_csv, stdout);

  return 0;
}
