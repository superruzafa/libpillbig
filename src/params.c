/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Parameters parsing. Implementation.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <assert.h>
#include "params.h"

static int
str_to_number(const char *string);

static PillBigCMDInfo
parse_info(char *arg, int *error);

static PillBigCMDFormat
parse_audio_format(char *arg, int *error);

static PillBigCMDFormat
parse_bitmap_format(char *arg, int *error);

static PillBigReplaceMode
parse_replace_mode(char *arg, int *error);



PillBigCMDParams *
pillbig_cmd_params_decode(int argc, char **argv)
{
	PillBigCMDParams *params;
	static struct option options[] =
	{
		{"help",     no_argument,       0, 'h'},
		{"version",  no_argument,       0, 'v'},
		{"info",     optional_argument, 0, 'i'},
		{"extract",  no_argument,       0, 'x'},
		{"replace",  optional_argument, 0, 'r'},
		{"hash",     no_argument,       0, 's'},

		{"pillbig",  required_argument, 0, 'p'},
		{"database", optional_argument, 0, 'd'},
		{"convert",  required_argument, 0, 'c'},
		{"pattern",  required_argument, 0, 't'},

		{0,          0,                 0, 0}
	};

	params = (PillBigCMDParams *)malloc(sizeof(PillBigCMDParams));
	if (params == NULL) return NULL;
	memset(params, 0, sizeof(PillBigCMDParams));

	params->command = argv[0];
	params->mode = -1;

	int c;
	int index = 0;
	while (1)
	{
		c = getopt_long(argc, argv, "hvi::xr::sp:d::c:t:", options, &index);
		if (c == -1) break;

		switch (c)
		{
			case 'h': // --help
				if (params->mode != -1) params->error = 1;
				params->mode = PillBigCMDMode_Help;
				break;
			case 'v': // --version
				if (params->mode != -1) params->error = 1;
				params->mode = PillBigCMDMode_Version;
				break;
			case 'i': // --info
				if (params->mode != -1 && params->mode != PillBigCMDMode_Info) params->error = 1;
				params->mode = PillBigCMDMode_Info;
				params->show_info |= parse_info(optarg, &params->error);
				break;
			case 'x': // --extract
				if (params->mode != -1) params->error = 1;
				params->mode = PillBigCMDMode_Extract;
				break;
			case 'r': // --replace
				if (params->mode != -1) params->error = 1;
				params->mode = PillBigCMDMode_Replace;
				params->replace_mode = parse_replace_mode(optarg, &params->error);
				break;
			case 's': // --hash
				if (params->mode != -1) params->error = 1;
				params->mode = PillBigCMDMode_Hash;
				break;

			case 'p': // --pillbig
				params->pillbig = optarg;
				break;
			case 'd': // --database
				params->use_database = 1;
				params->database = optarg;
				break;
			case 'c': // --convert
				params->audio_format = parse_audio_format(optarg, &params->error);
				params->bitmap_format = parse_bitmap_format(optarg, &params->error);
				break;
			case 't': // --pattern
				if (strcmp(optarg, "") != 0)
				{
					params->filename_pattern = optarg;
				}
				break;
		}

	}

	int argc_count = argc - optind;
	int i = 0;

	switch (params->mode)
	{
		case PillBigCMDMode_Info:
		case PillBigCMDMode_Extract:
		case PillBigCMDMode_Replace:
			params->indices = (int *)calloc(argc_count, sizeof(int));
			// TODO: Check memory allocation

			while (optind < argc && !params->error)
			{
				int value = str_to_number(argv[optind++]);
				if (value != -1)
				{
					params->files_count++;
					params->indices[i++] = value;
				}
				else
				{
					params->error = 1;
				}
			}
			break;
		case PillBigCMDMode_Hash:
			params->filenames = (char **)calloc(argc_count, sizeof(char **));
			// TODO: Check memory allocation

			while (optind < argc)
			{
				params->filenames_count++;
				params->filenames[i++] = argv[optind++];
			}
			break;
	}

	return params;
}

void
pillbig_cmd_params_free(PillBigCMDParams *params)
{
	if (params != NULL)
	{
		if (params->indices != NULL)
		{
			free(params->indices);
		}
		if (params->filenames != NULL)
		{
			free(params->filenames);
		}
		free(params);
	}
}



static int
str_to_number(const char *string)
{
	int value = 0;
	const char *ptr = string;

	while (value != -1 && ptr != NULL && *ptr != '\0')
	{
		value *= 10;
		switch (*ptr++)
		{
			case '0': value += 0; break;
			case '1': value += 1; break;
			case '2': value += 2; break;
			case '3': value += 3; break;
			case '4': value += 4; break;
			case '5': value += 5; break;
			case '6': value += 6; break;
			case '7': value += 7; break;
			case '8': value += 8; break;
			case '9': value += 9; break;
			default:
				value = -1;
				break;
		}
	}

	return value;
}

static PillBigCMDInfo
parse_info(char *arg, int *error)
{
	assert(error != NULL);
	PillBigCMDInfo info = PillBigCMDInfo_All;

	     if (arg == NULL)                info = PillBigCMDInfo_All;
	else if (strcmp(arg, "index") == 0)  info = PillBigCMDInfo_Index;
	else if (strcmp(arg, "i") == 0)      info = PillBigCMDInfo_Index;
	else if (strcmp(arg, "hash") == 0)   info = PillBigCMDInfo_Hash;
	else if (strcmp(arg, "h") == 0)      info = PillBigCMDInfo_Hash;
	else if (strcmp(arg, "offset") == 0) info = PillBigCMDInfo_Offset;
	else if (strcmp(arg, "o") == 0)      info = PillBigCMDInfo_Offset;
	else if (strcmp(arg, "size") == 0)   info = PillBigCMDInfo_Size;
	else if (strcmp(arg, "s") == 0)      info = PillBigCMDInfo_Size;
	else if (strcmp(arg, "name") == 0)   info = PillBigCMDInfo_Name;
	else if (strcmp(arg, "n") == 0)      info = PillBigCMDInfo_Name;
	else *error = 1;

	return info;
}

static PillBigCMDFormat
parse_audio_format(char *arg, int *error)
{
	assert(error != NULL);
	PillBigCMDFormat format = PillBigCMDFormat_Auto;

         if (arg == NULL)             format = PillBigCMDFormat_Auto;
    else if (strcmp(arg, "pcm") == 0) format = PillBigCMDFormat_PCM;
	else if (strcmp(arg, "wav") == 0) format = PillBigCMDFormat_WAV;

	return format;
}

static PillBigCMDFormat
parse_bitmap_format(char *arg, int *error)
{
	assert(error != NULL);
	PillBigCMDFormat format = PillBigCMDFormat_Auto;

         if (arg == NULL)             format = PillBigCMDFormat_Auto;
    else if (strcmp(arg, "bmp") == 0) format = PillBigCMDFormat_BMP;
	else if (strcmp(arg, "png") == 0) format = PillBigCMDFormat_PNG;

	return format;
}

static PillBigReplaceMode
parse_replace_mode(char *arg, int *error)
{
	assert(error != NULL);
	PillBigReplaceMode mode = PillBigReplaceMode_Strict;

	     if (arg == NULL)                 mode = PillBigReplaceMode_Strict;
 	else if (strcmp(arg, "t") == 0)       mode = PillBigReplaceMode_Strict;
	else if (strcmp(arg, "shorter") == 0) mode = PillBigReplaceMode_AllowShorterFiles;
	else if (strcmp(arg, "s") == 0)       mode = PillBigReplaceMode_AllowShorterFiles;
	else if (strcmp(arg, "larger") == 0)  mode = PillBigReplaceMode_AllowLargerFiles;
	else if (strcmp(arg, "l") == 0)       mode = PillBigReplaceMode_AllowLargerFiles;
	else *error = 1;

	return mode;
}
