#pragma once

#include "defs.hpp"

#ifdef LOUDNESS_VAR
#define IS_LOUDNESS(x) LOUDNESS_VAR >= x
#else
#define IS_LOUDNESS(x) landsat::loudness_level >= x
#endif

#define IS_QUIET IS_LOUDNESS(LOUDNESS_QUIET)
#define IS_NORMAL IS_LOUDNESS(LOUDNESS_NORMAL)
#define IS_VERBOSE IS_LOUDNESS(LOUDNESS_VERBOSE)

#define IF_LOUDNESS(x, y) do { if(IS_LOUDNESS(x)) {y;} } while(0)
#define IF_QUIET(x) IF_LOUDNESS(LOUDNESS_QUIET, x)
#define IF_NORMAL(x) IF_LOUDNESS(LOUDNESS_NORMAL, x) 
#define IF_VERBOSE(x) IF_LOUDNESS(LOUDNESS_VERBOSE, x)

namespace landsat
{
	int get_loudness_level();
}
