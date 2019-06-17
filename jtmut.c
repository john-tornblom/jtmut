/* Copyright (C) 2019 John Törnblom

   This file is part of jtmut, Johns tiny schemata mutation testing tool.

jtmut is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free
Software Foundation, either version 3 of the License, or (at your option) any
later version.

jtmut is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
for more details.

You should have received a copy of the GNU Lesser General Public
License along with jtmut; see the files COPYING and COPYING.LESSER. If not,
see <http://www.gnu.org/licenses/>.  */

#include <stdlib.h>    //exit
#include <sys/types.h> //pid_t
#include <unistd.h>    //fork
#include <sys/wait.h>  //waitpid

#include "jtmut.h"


/**
 * Linked listed capturing all registered mutant identifiers.
 **/
typedef struct id_sequence {
  jtmut_id            value;
  struct id_sequence* next;
} id_sequence_t;


/**
 * Linked list capturing all registered test functions.
 **/
typedef struct test_sequence {
  jtmut_test_cb*        cb;
  void*                 ctx;
  struct test_sequence* next;
} test_sequence_t;


/**
 * Global state for the metaprogram.
 **/
static test_sequence_t* head_test = NULL;
static id_sequence_t*   head_id   = NULL;
static jtmut_id         curr_id   = 0;


void
jtmut_register_mutant(jtmut_id id) {
  if(0 == id) {
    exit(EXIT_FAILURE);
  }

  for(id_sequence_t *it=head_id; it!=NULL; it=it->next) {
    if(it->value == id) {
      exit(EXIT_FAILURE);
    }
  }

  id_sequence_t *item = malloc(sizeof(id_sequence_t));
  if(!item) {
    exit(EXIT_FAILURE);
  }

  item->value = id;
  item->next  = head_id;
  head_id     = item;
}


void
jtmut_register_test(jtmut_test_cb* test_cb, void* ctx) {
  test_sequence_t *item = malloc(sizeof(test_sequence_t));
  if(!item) {
    exit(EXIT_FAILURE);
  }

  item->cb   = test_cb;
  item->ctx  = ctx;
  item->next = head_test;
  head_test  = item;
}


jtmut_id
jtmut_get_current_id(void) {
  return curr_id;
}


static int
jtmut_execute_mutant(jtmut_id mid) {
  pid_t pid = fork();
  if(pid < 0) {
    exit(EXIT_FAILURE);
  }

  if(pid == 0) {
    curr_id = mid;
    for(test_sequence_t *it=head_test; it!=NULL; it=it->next) {
      it->cb(it->ctx);
    }
    exit(EXIT_SUCCESS);
  }

  int status;
  waitpid(pid, &status, 0);

  return status;
}


void
jtmut_launch(jtmut_outcome_cb* outcome_cb, void* ctx) {
  int status = jtmut_execute_mutant(0);
  outcome_cb(ctx, 0, status);
  if(status != EXIT_SUCCESS) {
    return;
  }

  for(id_sequence_t *it=head_id; it!=NULL; it=it->next) {
    status = jtmut_execute_mutant(it->value);
    outcome_cb(ctx, it->value, status);
  }
}

