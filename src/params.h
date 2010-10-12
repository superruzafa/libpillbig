/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Parameters parsing.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#ifndef __PILLBIG_CMD_PARAMS__
#define __PILLBIG_CMD_PARAMS__

#include <pillbig/pillbig.h>

typedef enum
{
	PillBigCMDMode_Help,       /**< Shows the help. */
	PillBigCMDMode_Version,    /**< Shows version information */
	PillBigCMDMode_Info,       /**< Shows information about pill.big files. */
	PillBigCMDMode_Extract,    /**< Extracts (and optionally converts) pill.big files. */
	PillBigCMDMode_Replace,    /**< Replace (and optionally converts) pill.big files */
	PillBigCMDMode_Hash,       /**< Calculate the Blood Omen hashes of filenames. */
}
PillBigCMDMode;

typedef enum
{
	PillBigCMDFormat_Auto,    /**< The format is autodetected. */

	PillBigCMDFormat_PCM,     /**< Converts to/from PCM audio format. */
	PillBigCMDFormat_WAV,     /**< Converts to/from WAV audio format. */

	PillBigCMDFormat_BMP,     /**< Converts to/from BMP graphical format. */
	PillBigCMDFormat_PNG,     /**< Converts to/from PNG graphical format. */
}
PillBigCMDFormat;

typedef enum
{
	PillBigCMDInfo_All     = 0xffff,    /**< Shows all known fields. */
	PillBigCMDInfo_PillBig = 1 << 0,    /**< Shows pill.big information. */
	PillBigCMDInfo_Index   = 1 << 1,    /**< Shows index information. */
	PillBigCMDInfo_Hash    = 1 << 2,    /**< Shows hashes information. */
	PillBigCMDInfo_Offset  = 1 << 3,    /**< Shows offsets information. */
	PillBigCMDInfo_Size    = 1 << 4,    /**< Shows sizes information. */
	PillBigCMDInfo_Name    = 1 << 5,    /**< Shows filenames information. */
}
PillBigCMDInfo;

typedef struct
{
	char                 *command;         /**< Command line tool name. */
	PillBigCMDMode        mode;            /**< Operation mode. */
	int                   error;           /**< Error flag. */
	char                 *pillbig;         /**< pill.big filename to use. */
	int                   use_database;    /**< Use database flag. */
	char                 *database;        /**< Database filename, if specified. */
	PillBigReplaceMode    replace_mode;    /**< Replacement mode, when operation mode is replace */
	PillBigCMDFormat      audio_format;    /**< Audio format. */
	PillBigCMDFormat      bitmap_format;   /**< Bitmap format. */
	PillBigCMDInfo        show_info;       /**< Info flags. */
	char                 *pattern;         /**< Filename pattern. */
	int                   files_count;     /**< Count of file indices. */
	int                  *indices;         /**< File indices. */
}
PillBigCMDParams;

PillBigCMDParams *
pillbig_cmd_params_decode(int argc, char **argv);

void
pillbig_cmd_params_free(PillBigCMDParams *params);

#endif
