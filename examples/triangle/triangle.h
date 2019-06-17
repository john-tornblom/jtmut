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


#ifndef _TRIANGLE_H_
#define _TRIANGLE_H_


#include <stdint.h> //uint64_t


/**
 * A triangle can be one of four types:
 * - SCA: Scalene, the triangle has no sides of equal lengths
 * - ISO: Isosceles, the triangle has two sides of equal length
 * - EQU: Equilateral, the triangle has three sides of equal length
 * - ERR: Error, not a triangle.
 **/
typedef enum triangle_type {
  SCA,
  ISO,
  EQU,
  ERR
} triangle_type_t;


/**
 * A data structure capturing the side lengths of a triangle.
 **/
typedef struct triangle {
  uint64_t s1;
  uint64_t s2;
  uint64_t s3;
} triangle_t;


/**
 * Create a new triangle.
 **/
triangle_t* triangle_new(uint64_t s1, uint64_t s2, uint64_t s3);


/**
 * Delete a triangle.
 **/
void triangle_del(triangle_t* t);


/**
 * Compute the area captured by a triangle.
 **/
double triangle_area(const triangle_t* t);


/**
* Get the type of a triangle.
**/
triangle_type_t triangle_type(const triangle_t* t);


#endif // _TRIANGLE_H_
