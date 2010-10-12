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
#include <pillbig/pillbig.h>
#include "params.h"

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
pillbig_cmd_unimplemented();

typedef
void (* PillBigCMDActionCallback)(PillBig pillbig, int index, PillBigCMDParams *params);


int
main (int argc, char **argv)
{
	PillBig pillbig = pillbig_open_from_filename("PILL.BIG-PC");
	char buffer[16];
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
		case PillBigCMDMode_Extract:
		case PillBigCMDMode_Hash:
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
		"/usr/share/pillbig/pillbig-pc.xml",
		NULL
	};
	char *filenames_psx[] =
	{
		"pillbig-psx.xml",
		"/usr/share/pillbig/pillbig-psx.xml",
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
    -n,	--name=PATTERN           External filenames pattern"));

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

	puts(_("Infos:\n\
    a, all (default)             Show all available fields\n\
    i, index                     Include index information\n\
    h, hash                      Include hashes information\n\
    o, offset                    Include offsets information\n\
    s, size                      Include sizes information\n\
    n, name                      Include names information"));

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
	char buffer[32];
	char *filename = buffer;
	sprintf(buffer, "file%04d", index);

	if (db != NULL)
	{
		dbentry = pillbig_db_get_entry(db, index);
		if (dbentry != NULL && dbentry->filename != NULL)
		{
			filename = dbentry->filename;
		}
	}

	if (params->show_info & PillBigCMDInfo_Index)  printf(_("Index: %04d\n"), index);
	if (params->show_info & PillBigCMDInfo_Name)   printf(_("Filename: %s\n"), filename);
	if (params->show_info & PillBigCMDInfo_Hash)   printf(_("Hash: %d\n"), entry->hash);
	if (params->show_info & PillBigCMDInfo_Offset) printf(_("Offset: %d\n"), entry->offset);
	if (params->show_info & PillBigCMDInfo_Size)   printf(_("Size: %d\n"), entry->size);
}

void
pillbig_cmd_unimplemented()
{
	puts(_("This feature is not yet implemented."));
}
