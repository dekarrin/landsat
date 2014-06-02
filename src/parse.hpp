#pragma once

#include "defs.hpp"

namespace landsat
{
	struct cli_arguments
	{
		int mode;
		int loudness;
		char *red_filename;
		char *nir_filename;
		rect<int> window;
	};

	int parse_args(int argc, char **argv, cli_arguments *args);
	bool parse_int(const char *str, int *parsed);
	bool parse_int_u(const char *str, unsigned int *parsed);
	bool parse_long(const char *str, long int *parsed);
	bool parse_long_u(const char *str, unsigned long int *parsed);
	bool parse_split(const char *str, int *parsed1, int *parsed2, int delimiter);
	bool parse_split_u(const char *str, unsigned int *parsed1, unsigned int *parsed2, int delimiter);

}
