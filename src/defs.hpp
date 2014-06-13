#pragma once

#include <iostream>

#ifdef HAVE_CONFIG_H
#include <config.h>
#else
#define VERSION "[replace with version number in defs.hpp]"
#endif

#include "types.hpp"

#define ERR_MISSING_ARG 1
#define ERR_MISSING_OPT_ARG 2
#define ERR_UNKNOWN_OPT 3
#define ERR_BAD_RED_FILE 4
#define ERR_BAD_NIR_FILE 5
#define ERR_BAD_POSITION 6
#define ERR_BAD_SIZE 7

#define MODE_NORMAL 0
#define MODE_CELLS 1
#define MODE_HELP 2
#define MODE_VERSION 3

#define LOUDNESS_QUIET 0
#define LOUDNESS_NORMAL 1
#define LOUDNESS_VERBOSE 2

#define IS_LOUDNESS(x) landsat::loudness_level >= x
#define IS_QUIET IS_LOUDNESS(LOUDNESS_QUIET)
#define IS_NORMAL IS_LOUDNESS(LOUDNESS_NORMAL)
#define IS_VERBOSE IS_LOUDNESS(LOUDNESS_VERBOSE)

#define IF_LOUDNESS(x, y) do { if(landsat::loudness_level >= x) {y;} } while(0)
#define IF_QUIET(x) IF_LOUDNESS(LOUDNESS_QUIET, x)
#define IF_NORMAL(x) IF_LOUDNESS(LOUDNESS_NORMAL, x) 
#define IF_VERBOSE(x) IF_LOUDNESS(LOUDNESS_VERBOSE, x)
