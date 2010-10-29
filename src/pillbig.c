/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Command line tool.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#include <stdlib.h>
#include <libintl.h>
#include <assert.h>
#include <string.h>
#include <regex.h>
#include <pillbig/pillbig.h>
#include "params.h"
#include "datadir.h"

#define DEFAULT_FILENAME_PATTERN "file*"
#define PATTERN_WILDCARD_CHAR '*'

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#define _(String) gettext(String)

PillBig
pillbig_cmd_open(PillBigCMDParams *params);

PillBigDB
pillbig_cmd_db_open(PillBigPlatform platform, PillBigCMDParams *params);

void
pillbig_cmd_help(PillBigCMDParams *params);

void
pillbig_cmd_version();

void
pillbig_cmd_infopb(PillBig pillbig, PillBigCMDParams *params);

void
pillbig_cmd_info(PillBig pillbig, int index, PillBigCMDParams *params);

void
pillbig_cmd_info_audio(PillBigDBAudioEntry *audio_entry);

void
pillbig_cmd_hash(PillBigCMDParams *params);

void
pillbig_cmd_extract(PillBig pillbig, int index, PillBigCMDParams *params);

void
pillbig_cmd_unimplemented();

const char *
pillbig_get_filename(PillBig pillbig, int index, PillBigCMDParams *params);

void
pillbig_fill_filename(const char *pattern, int index, char *buffer, int buffer_length);

PillBigFileType
pillbig_get_filetype(PillBig pillbig, int index, PillBigCMDParams *params);

PillBigDBAudioSpeech *
pillbig_get_best_l10n_speech(PillBigDBAudioEntry *entry);

int
pillbig_language_matches(const char *language);

typedef
void (* PillBigCMDActionCallback)(PillBig pillbig, int index, PillBigCMDParams *params);



int
main (int argc, char **argv)
{
	PillBig pillbig = NULL;
	PillBigDB db = NULL;
	PillBigCMDParams *params = pillbig_cmd_params_decode(argc, argv);
	PillBigCMDActionCallback callback = NULL;
	assert(params != NULL);

	if (params->error)
	{
		pillbig_cmd_help(params);
		pillbig_cmd_params_free(params);
		exit(EXIT_FAILURE);
	}

	/*
	 * Open the pill.big file for those modes that require it.
	 */
	switch (params->mode)
	{
		case PillBigCMDMode_Info:
		case PillBigCMDMode_Extract:
		case PillBigCMDMode_Replace:
			pillbig = pillbig_cmd_open(params);
			if (pillbig == NULL)
			{
				fprintf(stderr, (params->pillbig == NULL) ?
					_("Cannot find any pill.big file.\n") :
					_("Cannot open the specified pill.big file.\n"));
				exit(EXIT_FAILURE);
			}
			break;
	}

	/*
	 * Open the selected database, if any and needed for current mode.
	 */
	if (params->use_database)
	{
		switch (params->mode)
		{
			case PillBigCMDMode_Info:
			case PillBigCMDMode_Extract:
			case PillBigCMDMode_Replace:
				assert(pillbig != NULL);
				db = pillbig_cmd_db_open(pillbig_get_platform(pillbig), params);
				if (db == NULL)
				{
					fprintf(stderr, (params->database == NULL) ?
						_("Cannot find any pill.big database.\n") :
						_("Cannot open the specified database file.\n"));
					exit(EXIT_FAILURE);
				}
				pillbig_set_db(pillbig, db);
				break;
		}
	}

	/*
	 * Perform the command mode.
	 */
	switch (params->mode)
	{
		case PillBigCMDMode_Info:
			if (params->show_info & PillBigCMDInfo_PillBig)
			{
				pillbig_cmd_infopb(pillbig, params);
			}
			callback = pillbig_cmd_info;
			break;
		case PillBigCMDMode_Version:
			pillbig_cmd_version();
			break;
		case PillBigCMDMode_Hash:
			pillbig_cmd_hash(params);
			break;
		case PillBigCMDMode_Extract:
			callback = pillbig_cmd_extract;
			break;
		case PillBigCMDMode_Replace:
			pillbig_cmd_unimplemented();
			break;
		case PillBigCMDMode_Help:
		default:
			pillbig_cmd_help(params);
			break;
	}

	/*
	 * Iterate each file for the current mode if needed.
	 */
	if (callback != NULL)
	{
		assert(pillbig != NULL);
		int i = 0;
		if (params->files_count == 0)
		{
			for (i = 0; i < pillbig_get_files_count(pillbig); i++)
			{
				callback(pillbig, i, params);
			}
		}
		else
		{
			for (i = 0; i < params->files_count; i++)
			{
				callback(pillbig, params->indices[i], params);
			}
		}
	}

	/*
	 * Free resources
	 */
	if (db != NULL)
	{
		pillbig_set_db(pillbig, NULL);
		pillbig_db_close(db);
	}

	if (pillbig != NULL)
	{
		pillbig_close(pillbig);
	}

	pillbig_cmd_params_free(params);

	return EXIT_SUCCESS;
}

PillBig
pillbig_cmd_open(PillBigCMDParams *params)
{
	assert(params != NULL);

	PillBig pillbig = NULL;
	static char *filenames[] =
	{
		"PILL.BIG",
		"pill.big",
		NULL
	};

	if (params->pillbig != NULL)
	{
		pillbig = pillbig_open_from_filename(params->pillbig);
	}
	else
	{
		char **filename = filenames;
		while (pillbig == NULL && *filename != NULL)
		{
			pillbig = pillbig_open_from_filename(*filename);
			if (pillbig != NULL)
			{
				params->pillbig = *filename;
			}
			filename++;
		}
	}

	return pillbig;
}

PillBigDB
pillbig_cmd_db_open(PillBigPlatform platform, PillBigCMDParams *params)
{
	assert(params != NULL);

	PillBigDB db = NULL;
	char *filenames_pc[] =
	{
		"pillbig-pc.xml",
		DATADIR"/pillbig/pillbig-pc.xml",
		NULL
	};
	char *filenames_psx[] =
	{
		"pillbig-psx.xml",
		DATADIR"/pillbig/pillbig-psx.xml",
		NULL
	};
	char **filename;

	switch (platform)
	{
		case PillBigPlatform_PC:
			filename = filenames_pc;
			break;
		case PillBigPlatform_PSX:
			filename = filenames_psx;
			break;
	}

	if (params->database != NULL)
	{
		db = pillbig_db_open(params->database);
	}
	else
	{
		while (db == NULL && *filename != NULL)
		{
			db = pillbig_db_open(*filename);
			filename++;
		}
	}

	return db;
}

void
pillbig_cmd_help(PillBigCMDParams *params)
{
	assert(params != NULL);

	puts(_("Manage Blood Omen's pill.big files."));
	printf(_("Usage: %s [MODE] [OPTIONS] [INDICES] [FILENAMES]\n"), params->command);

	puts(_("Modes:\n\
    -h, --help                   Display this help and exit\n\
    -v, --version                Output version information and exit\n\
    -i, --info=INFO              Show information about pill.big files\n\
    -x, --extract                Extract files from pill.big\n\
    -r, --replace=REPLACEMODE    Replace pill.big files with external ones\n\
    -s, --hash                   Calculate Blood Omen hashnames from filenames"));

    puts("");

    puts(_("Options:\n\
    -p, --pillbig=PILLBIG        Specify the pill.big file to use\n\
    -d, --database=DATABASE      Specify the database file to use\n\
    -c, --convert=FORMAT         Set a conversion format\n\
    -t,	--pattern=PATTERN        External filenames pattern"));

	puts("");

	puts(_("Information:\n\
    a, all (default)             Show all available fields\n\
    i, index                     Include index information\n\
    h, hash                      Include hashes information\n\
    o, offset                    Include offsets information\n\
    s, size                      Include sizes information\n\
    n, name                      Include names information"));

	puts("");

	puts(_("Replace modes:\n\
    s, strict (default)          External and internal files must have exactly the same size\n\
    t, shorter                   External files can be shorter than internal files\n\
    l, larger                    External files can have any size"));

	puts("");

	puts(_("Formats:\n\
    auto (default)               Format is autodetected\n\
    wav, pcm                     Format for audio files\n\
    bmp, png                     Format for graphical files"));

	puts("");

	puts(_("Patterns:\n\
    Any string where all ocurrences of * will be replaced by the file index"));
}

void
pillbig_cmd_version()
{
	printf(_("%s %s, Copyright (C) %s\n"), PACKAGE_NAME, PACKAGE_VERSION, PACKAGE_BUGREPORT);
	printf(_("%s comes with ABSOLUTELY NO WARRANTY.\n\
This is free software, and you are welcome to redistribute\n\
it under certain conditions.\n"), PACKAGE_NAME);
}

void
pillbig_cmd_infopb(PillBig pillbig, PillBigCMDParams *params)
{
	assert(pillbig != NULL);
	assert(params != NULL);

	char *platform;
	char *using_db;
	switch (pillbig_get_platform(pillbig))
	{
		case PillBigPlatform_PC:
			platform = _("PC");
			break;
		case PillBigPlatform_PSX:
			platform = _("PlayStation");
			break;
		case PillBigPlatform_Unknown:
		default:
			platform = _("Unknown");
			break;
	}

	using_db = params->use_database ? _("Yes") : _("No");

	printf(_("Filename: %s\n"), params->pillbig);
	printf(_("Platform: %s\n"), platform);
	printf(_("Files count: %d\n"), pillbig_get_files_count(pillbig));
	printf(_("Using database: %s\n"), using_db);
}

void
pillbig_cmd_info(PillBig pillbig, int index, PillBigCMDParams *params)
{
	assert(pillbig != NULL);
	assert(0 <= index && index < pillbig_get_files_count(pillbig));
	assert(params != NULL);

	PillBigDB db = pillbig_get_db(pillbig);
	const PillBigFileEntry *entry   = pillbig_get_entry(pillbig, index);
	const PillBigDBEntry   *dbentry = NULL;
	const char *filename = pillbig_get_filename(pillbig, index, params);

	if (db != NULL)
	{
		dbentry = pillbig_db_get_entry(db, index);
	}

	if (params->show_info & PillBigCMDInfo_Index)  printf(_("Index: %04d\n"), index);
	if (params->show_info & PillBigCMDInfo_Name)   printf(_("Filename: %s\n"), filename);
	if (params->show_info & PillBigCMDInfo_Hash)   printf(_("Hash: 0x%08X\n"), entry->hash);
	if (params->show_info & PillBigCMDInfo_Offset) printf(_("Offset: %d\n"), entry->offset);
	if (params->show_info & PillBigCMDInfo_Size)   printf(_("Size: %d\n"), entry->size);

	if (dbentry != NULL)
	{
		switch (dbentry->filetype)
		{
		case PillBigFileType_Audio:
			pillbig_cmd_info_audio(dbentry->audio);
			break;
		}
	}

}

void
pillbig_cmd_info_audio(PillBigDBAudioEntry *audio_entry)
{

	if (audio_entry->character != NULL)
	{
		printf(_("Character: %s\n"), audio_entry->character);
	}

	PillBigDBAudioSpeech *speech = pillbig_get_best_l10n_speech(audio_entry);
	if (speech != NULL)
	{
		printf(_("Speech: %s\n"), speech->speech);
	}
}

void
pillbig_cmd_hash(PillBigCMDParams *params)
{
	assert(params != NULL);

	PillBigFileHash hash;
	int i = 0;

	for (i = 0; i < params->filenames_count; i++)
	{
		hash = pillbig_get_hash_by_filename(params->filenames[i]);
		if (hash == -1)
		{
			printf(_("%s: Cannot calculate the hashname\n"), params->filenames[i]);
		}
		else
		{
			printf("%s: 0x%08X\n", params->filenames[i], hash);
		}
	}
}

void
pillbig_cmd_extract(PillBig pillbig, int index, PillBigCMDParams *params)
{
	assert(pillbig != NULL);
	assert(0 <= index && index < pillbig_get_files_count(pillbig));
	assert(params != NULL);

	PillBigDB db = pillbig_get_db(pillbig);
	const PillBigDBEntry *dbentry = NULL;
	PillBigFileType filetype = pillbig_get_filetype(pillbig, index, params);
	PillBigAudioFormat audio_output_format = PillBigAudioFormat_Unknown;
	const char *filename = pillbig_get_filename(pillbig, index, params);
	assert(filename != NULL);

	switch (filetype)
	{
		case PillBigFileType_Audio:
			switch (params->audio_format)
			{
				case PillBigCMDFormat_PCM: audio_output_format = PillBigAudioFormat_PCM; break;
				case PillBigCMDFormat_WAV: audio_output_format = PillBigAudioFormat_WAVE; break;
				default: audio_output_format = PillBigAudioFormat_Unknown; break;
			}
	}

	if (filetype == PillBigFileType_Audio &&
	    audio_output_format != PillBigAudioFormat_Unknown)
	{
		pillbig_audio_extract_to_filename(pillbig, index, filename, audio_output_format);
	}
	else
	{
		pillbig_file_extract_to_filename(pillbig, index, filename);
	}

	if (pillbig_error_get() == PillBigError_Success)
	{
		printf(_("%s(%04d) -> %s: OK\n"), params->pillbig, index, filename);
	}
	else
	{
		fprintf(stderr, _("%s(%04d) -> %s: Error!\n"), params->pillbig, index, filename);
	}
}

void
pillbig_cmd_unimplemented()
{
	puts(_("This feature is not yet implemented.\n"));
}

const char *
pillbig_get_filename(PillBig pillbig, int index, PillBigCMDParams *params)
{
	assert(pillbig != NULL);
	assert(0 <= index && index < pillbig_get_files_count(pillbig));
	assert(params != NULL);

	PillBigDB db = pillbig_get_db(pillbig);
	const PillBigDBEntry *dbentry = NULL;
	static char buffer[256];
	memset(buffer, 0, 256);

	if (db != NULL)
	{
		dbentry = pillbig_db_get_entry(db, index);
	}

	if (params->filename_pattern != NULL)
	{
		pillbig_fill_filename(params->filename_pattern, index, buffer, 256);
	}
	else if (dbentry != NULL && dbentry->filename != NULL)
	{
		// TODO: strip directory from database filename
		strcpy(buffer, dbentry->filename);
	}
	else
	{
		pillbig_fill_filename(DEFAULT_FILENAME_PATTERN, index, buffer, 256);
	}

	return buffer;
}

void
pillbig_fill_filename(const char *pattern, int index, char *buffer, int buffer_length)
{
	assert(pattern != NULL);
	assert(index >= 0);
	assert(buffer != NULL);
	assert(buffer_length > 0);

	memset(buffer, 0, buffer_length);
	while (buffer_length > 0 && *pattern != '\0')
	{
		if (*pattern == PATTERN_WILDCARD_CHAR)
		{
			if (buffer_length >= 4)
			{
				sprintf(buffer, "%04d", index);
			}
			buffer_length -= 4;
			buffer += 4;
		}
		else
		{
			*buffer = *pattern;
			buffer++;
			buffer_length--;
		}
		pattern++;
	}
}

PillBigFileType
pillbig_get_filetype(PillBig pillbig, int index, PillBigCMDParams *params)
{
	assert(pillbig != NULL);
	assert(0 <= index & index < pillbig_get_files_count(pillbig));
	assert(params != NULL);

	PillBigFileType filetype = PillBigFileType_Unknown;
	PillBigDB db = pillbig_get_db(pillbig);

	if (db != NULL)
	{
		const PillBigDBEntry *dbentry = NULL;
		dbentry = pillbig_db_get_entry(db, index);
		if (dbentry != NULL)
		{
			filetype = dbentry->filetype;
		}
	}
	else
	{
		filetype = pillbig_file_get_type(pillbig, index);
	}
	return filetype;
}

PillBigDBAudioSpeech *
pillbig_get_best_l10n_speech(PillBigDBAudioEntry *entry)
{
	assert(entry != NULL);
	PillBigDBAudioSpeech *speech = NULL;
	int i = 0;

	while (i < entry->speeches_count)
	{
		if (speech == NULL)
		{
			speech = &entry->speeches[i];
		}
		else if (pillbig_language_matches(entry->speeches[i].language))
		{
			speech = &entry->speeches[i];
			break;
		}
		i++;
	}

	return speech;
}

int
pillbig_language_matches(const char *language)
{
	static char *envlang = NULL;
	int result;
	char buffer[16];

	if (strlen(language) > 14) return 0;

	sprintf(buffer, "^%s", language);
	if (envlang == NULL) envlang = getenv("LANG");

	regex_t regex;
	result = regcomp(&regex, buffer, REG_NOSUB);
	assert(result == 0);

	result = regexec(&regex, envlang, 0, NULL, 0);
	result = result == 0 ? 1 : 0;
	regfree(&regex);

	return result;
}
