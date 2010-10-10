#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include "params.h"

static PillBigCMDInfo
parse_info(char *arg);

static PillBigCMDFormat
parse_audio_format(char *arg);

static PillBigCMDFormat
parse_bitmap_format(char *arg);

static PillBigReplaceMode
parse_replace_mode(char *arg);



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
		{"name",     required_argument, 0, 'n'},

		{0,          0,                 0, 0}
	};

	params = (PillBigCMDParams *)malloc(sizeof(PillBigCMDParams));
	if (params == NULL) return NULL;
	memset(params, 0, sizeof(PillBigCMDParams));

	params->command = argv[0];

	int c;
	int index = 0;
	while (1)
	{
		c = getopt_long(argc, argv, "hvi::xr::sp:d::c:n:", options, &index);
		if (c == -1) break;

		switch (c)
		{
			case 'h': // --help
				params->mode = PillBigCMDMode_Help;
				break;
			case 'v': // --version
				params->mode = PillBigCMDMode_Version;
				break;
			case 'i': // --info
				params->mode = PillBigCMDMode_Info;
				params->show_info |= parse_info(optarg);
				break;
			case 'x': // --extract
				params->mode = PillBigCMDMode_Extract;
				break;
			case 'r': // --replace
				params->mode = PillBigCMDMode_Replace;
				params->replace_mode = parse_replace_mode(optarg);
				break;
			case 's': // --hash
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
				params->audio_format = parse_audio_format(optarg);
				params->bitmap_format = parse_bitmap_format(optarg);
				break;
			case 'n': // -names
				if (strcmp(optarg, "internal") != 0)
				{
					params->pattern = optarg;
				}
				break;
		}

	}

	params->files_count = argc - optind;
	params->indices = (int *)calloc(params->files_count, sizeof(int));

	int i = 0;
	while (i < params->files_count)
	{
		params->indices[i++] = atoi(argv[optind++]);
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
		free(params);
	}
}



static PillBigCMDInfo
parse_info(char *arg)
{
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

	return info;
}

static PillBigCMDFormat
parse_audio_format(char *arg)
{
	PillBigCMDFormat format = PillBigCMDFormat_Auto;

         if (arg == NULL)             format = PillBigCMDFormat_Auto;
    else if (strcmp(arg, "pcm") == 0) format = PillBigCMDFormat_PCM;
	else if (strcmp(arg, "wav") == 0) format = PillBigCMDFormat_WAV;

	return format;
}

static PillBigCMDFormat
parse_bitmap_format(char *arg)
{
	PillBigCMDFormat format = PillBigCMDFormat_Auto;

         if (arg == NULL)             format = PillBigCMDFormat_Auto;
    else if (strcmp(arg, "bmp") == 0) format = PillBigCMDFormat_BMP;
	else if (strcmp(arg, "png") == 0) format = PillBigCMDFormat_PNG;

	return format;
}

static PillBigReplaceMode
parse_replace_mode(char *arg)
{
	PillBigReplaceMode mode = PillBigReplaceMode_Strict;

	     if (arg == NULL)                 mode = PillBigReplaceMode_Strict;
 	else if (strcmp(arg, "t") == 0)       mode = PillBigReplaceMode_Strict;
	else if (strcmp(arg, "shorter") == 0) mode = PillBigReplaceMode_AllowShorterFiles;
	else if (strcmp(arg, "s") == 0)       mode = PillBigReplaceMode_AllowShorterFiles;
	else if (strcmp(arg, "larger") == 0)  mode = PillBigReplaceMode_AllowLargerFiles;
	else if (strcmp(arg, "l") == 0)       mode = PillBigReplaceMode_AllowLargerFiles;

	return mode;

}
