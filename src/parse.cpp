#include "parse.hpp"

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cerrno>
#include <climits>
#include <cstring>
#include <getopt.h>

namespace landsat
{

	static int parse_opts(int argc, char **argv, cli_arguments *args, int *next_arg);

	int parse_args(int argc, char **argv, cli_arguments *args)
	{
		int status = 0;
		args->mode = MODE_NORMAL;
		args->loudness = LOUDNESS_NORMAL;
		args->window.x = 0;
		args->window.y = 0;
		args->window.width = 0;
		args->window.height = 0;
		int nextind;
		status = parse_opts(argc, argv, args, &nextind);
		if (status == 0 && args->mode == MODE_NORMAL) {
			if (argc - nextind < 2) {
				status = ERR_MISSING_ARG;
			} else {
				args->red_filename = argv[nextind++];
				args->nir_filename = argv[nextind++];
				std::fstream red_file(args->red_filename);
				std::fstream nir_file(args->nir_filename);
				bool red_exists = red_file.good();
				bool nir_exists = nir_file.good();
				red_file.close();
				nir_file.close();
				if (!red_exists) {
					std::cerr << "Error: could not open '" << args->red_filename << "'" << std::endl;
					status = ERR_BAD_RED_FILE;
				} else if (!nir_exists) {
					std::cerr << "Error: could not open '" << args->nir_filename << "'" << std::endl;
					status = ERR_BAD_NIR_FILE;
				}
			}
		}
		return status;
	}

	int parse_opts(int argc, char **argv, cli_arguments *args, int *next_arg)
	{
		static option long_opts[] = {
			{"help",	no_argument,		NULL,		'h'	},
			{"version",	no_argument,		NULL,		'i'	},
			{"quiet",	no_argument,		NULL,		'q'	},
			{"silent",	no_argument,		NULL,		'q'	},
			{"verbose",	no_argument,		NULL,		'v'	},
			{"position",	required_argument,	NULL,		'p'	},
			{"size",	required_argument,	NULL,		's'	},
			{0,		0,			0,		0	}
		};
		int status = 0;
		while (true)
		{
			int c = getopt_long(argc, argv, ":hiqvp:s:", long_opts, NULL);
			if (c == -1 || args->mode != MODE_NORMAL || status != 0) {
				break;
			}
			switch (c) {
				case 'h':
					args->mode = MODE_HELP;
					break;

				case 'i':
					args->mode = MODE_VERSION;
					break;

				case 'q':
					args->loudness = LOUDNESS_QUIET;
					break;

				case 'v':
					args->loudness = LOUDNESS_VERBOSE;
					break;

				case 'p':
					if (!parse_split(optarg, &(args->window.x), &(args->window.y), ',')) {
						status = ERR_BAD_POSITION;
						std::cerr << "Error: '" << optarg << "' is not a valid position" << std::endl;
					}
					break;

				case 's':
					if (!parse_split(optarg, &(args->window.width), &(args->window.height), 'x')) {
						if (!parse_split(optarg, &(args->window.width), &(args->window.height), 'X')) {
							status = ERR_BAD_SIZE;
							std::cerr << "Error: '" << optarg << "' is not a valid size" << std::endl;
						}
					}
					break;

				case '?':
					status = ERR_UNKNOWN_OPT;
					// getopt will have already printed an error message
					break;

				case ':':
					status = ERR_MISSING_OPT_ARG;
					std::cerr << "Error: -" << ((char) optopt) << " option requires an argument" << std::endl;
					break;

				default:
					// should never happen
					std::cerr << "Warning: getopt returned character code 0x" << std::hex << c << std::dec << std::endl;
					break;
			}
		}
		*next_arg = optind;
		return status;
	}

	bool parse_int(const char *str, int *parsed)
	{
		long int conv;
		if (parse_long(str, &conv)) {
			if (conv > INT_MAX) {
				*parsed = INT_MAX;
			} else {
				*parsed = (int)conv;
			}
			return true;
		} else {
			return false;
		}
	}

	bool parse_int_u(const char *str, unsigned int *parsed)
	{
		unsigned long int conv;
		if (parse_long_u(str, &conv)) {
			if (conv > UINT_MAX) {
				*parsed = UINT_MAX;
			} else {
				*parsed = (unsigned int)conv;
			}
			return true;
		} else {
			return false;
		}
	}

	bool parse_long(const char *str, long int *parsed)
	{
		errno = 0;
		long int conv = strtol(str, NULL, 0);
		if (conv == 0 && errno != 0) {
			return false;
		} else {
			*parsed = conv;
			return true;
		}
	}

	bool parse_long_u(const char *str, unsigned long int *parsed)
	{
		errno = 0;
		unsigned long int conv = strtoul(str, NULL, 0);
		if (conv == 0 && errno != 0) {
			return false;
		} else {
			*parsed = conv;
			return true;
		}
	}

	bool parse_split(const char *str, int *parsed1, int *parsed2, int delimiter)
	{
		bool success = false;
		char delimiter_str[] = { (char)delimiter, '\0' };
		size_t loc = strcspn(str, delimiter_str);
		if (loc != strlen(str)) {
			char *half_1 = new char[loc + 1];
			char *half_2 = new char[strlen(str) - (loc + 1) + 1];
			strncpy(half_1, str, loc);
			half_1[loc] = '\0';
			strcpy(half_2, str + loc + 1);
			int conv1, conv2;
			if (parse_int(half_1, &conv1) && parse_int(half_2, &conv2)) {
				*parsed1 = conv1;
				*parsed2 = conv2;
				success = true;
			}
			delete[] half_1;
			delete[] half_2;
		}
		return success;
	}

	bool parse_split_u(const char *str, unsigned int *parsed1, unsigned int *parsed2, int delimiter)
	{
		bool success = false;
		char delimiter_str[] = { (char)delimiter, '\0' };
		size_t loc = strcspn(str, delimiter_str);
		if (loc != strlen(str)) {
			char *half_1 = new char[loc];
			char *half_2 = new char[strlen(str) - (loc + 1)];
			strncpy(half_1, str, loc);
			strcpy(half_2, str + loc + 1);
			unsigned int conv1, conv2;
			if (parse_int_u(half_1, &conv1) && parse_int_u(half_2, &conv2)) {
				*parsed1 = conv1;
				*parsed2 = conv2;
				success = true;
			}
			delete[] half_1;
			delete[] half_2;
		}
		return success;
	}

}
