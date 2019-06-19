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

#ifndef _JTMUT_H_
#define _JTMUT_H_


/**
 * Type declaration for mutant identifiers. The value zero (0) is reserved for
 *  the original non-mutated program instance.
 **/
typedef unsigned __int128 jtmut_id;


/**
 * Convenient macro to express 128bit literals.
 **/
#define JTMUT_ID(hi, lo) (((jtmut_id)hi << 64) | lo)


/**
 * GNU C extension that causes a function to be called automatically before 
 * execution enters main().
 **/
#define JTMUT_CONSTRUCTOR __attribute__((constructor)) static void


/**
 * Function prototype used to report the exit *status* of a mutant
 * identified by *id*.
 **/
typedef void (jtmut_outcome_cb)(void* ctx, jtmut_id id, int status);


/**
 * Function prototype for tests subject to mutant analysis.
 **/
typedef void (jtmut_test_cb)(void* ctx);


/**
 * Register a test function to be included in the mutant analysis.
 **/
void jtmut_register_test(jtmut_test_cb* test_cb, void* ctx);


/**
 * Register a mutant to be included in the mutant analysis.
 *
 * Note: if the provided *id* is allready occopied by a registered mutant, the
 * parent process terminates with the EXIT_FAILURE status.
 **/
void jtmut_register_mutant(jtmut_id id);


/**
 * Get the identifier of the current mutant.
 **/
jtmut_id jtmut_get_current_id(void);


/**
 * Launch child processes that executes all registered test functions for the
 * original program and all registered mutants. Results from each child process
 * are communicated via *outcome_cb*. The *ctx* parameter provides the outcome
 * callback function access to an arbritary context.
 *
 * Note: If the original program terminates with an error, mutant analysis
 * is aborted and the parent process terminates with the status EXIT_FAILURE.
 **/
void jtmut_launch(jtmut_outcome_cb* outcome_cb, void* ctx);


#endif // _JTMUT_H_
