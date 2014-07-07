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
#define ERR_BAD_N 8
#define ERR_BAD_BASE 9
#define ERR_BAD_WINDOW_START 10

#define DEFAULT_SIZE_BASE 2
#define DEFAULT_ANALYSIS_START_POW 1
#define DEFAULT_HYBRID_START_POW 3

#define MODE_NORMAL 0
#define MODE_CELLS 1
#define MODE_HYBRID 2
#define MODE_HELP 3
#define MODE_VERSION 4

#define LOUDNESS_QUIET 0
#define LOUDNESS_NORMAL 1
#define LOUDNESS_VERBOSE 2
