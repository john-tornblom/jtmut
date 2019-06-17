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


#include <assert.h> //assert
#include <stddef.h> //NULL

#include "triangle.h"


#define assert_triangle_type(s1, s2, s3, expected) {			\
  triangle_t* t = triangle_new(s1, s2, s3);				\
  triangle_type_t actual = triangle_type(t);				\
  triangle_del(t);							\
  assert(expected == actual);						\
}


#define assert_triangle_area(s1, s2, s3, expected) {			\
  triangle_t* t = triangle_new(s1, s2, s3);				\
  double actual = triangle_area(t);					\
  triangle_del(t);							\
  assert(expected == actual);						\
}


static void
test_triangle_type() {
  assert_triangle_type(1, 1, 1, EQU);

  assert_triangle_type(2, 2, 1, ISO);
  assert_triangle_type(2, 1, 2, ISO);
  assert_triangle_type(1, 2, 2, ISO);

  assert_triangle_type(4, 3, 2, SCA);

  // expect integer overflow
  assert_triangle_type(4201476, 4201476, 2145527840, ERR);
  assert_triangle_type(681740491, 1534703449, 681740491, ERR);

  assert_triangle_type(2, 1, 1, ERR);
  assert_triangle_type(1, 1, 2, ERR);

  assert_triangle_type(1, 1, 0, ERR);
  assert_triangle_type(0, 1, 1, ERR);
  assert_triangle_type(1, 0, 1, ERR);

  assert_triangle_type(2, 1, 3, ERR);
  assert_triangle_type(2, 3, 1, ERR);
  assert_triangle_type(3, 2, 1, ERR);

  assert_triangle_type(1, 2, 4, ERR);
  assert_triangle_type(4, 2, 1, ERR);
  assert_triangle_type(2, 4, 1, ERR);
}

static void
test_triangle_area() {
  assert_triangle_area(0, 1, 1, 0);
  assert_triangle_area(1, 0, 1, 0);
  assert_triangle_area(1, 1, 0, 0);
  assert_triangle_area(0, 0, 0, 0);

  assert_triangle_area(12, 16, 20, 96);
}


#ifdef HAVE_JTMUT_H
#include "jtmut.h"

__attribute__((constructor)) static void
register_tests() {
  jtmut_register_test(test_triangle_area, NULL);
  jtmut_register_test(test_triangle_type, NULL);
}

#endif
