/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Basic pill.big metafile manipulation. Implementation file.
 */


#include <pillbig/pillbig.h>
#include "error_internal.h"

struct PillBig
{
	FILE                *pillbig;         /**< pill.big file descriptor .*/
	PillBigPlatform      platform;        /**< Blood Omen's pill.big platform. */
	int                  files_count;     /**< pill.big files count. */
	PillBigFileEntry    *entries;         /**< File entries table. */
	PillBigReplaceMode   replace_mode;    /**< Replacement mode. */
};

PillBig
pillbig_open(FILE *input)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, (struct PillBig *)NULL);
}

PillBigPlatform
pillbig_get_platform(PillBig pillbig)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, PillBigPlatform_Unknown);
}

PillBigReplaceMode
pillbig_get_replace_mode(PillBig pillbig)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, PillBigReplaceMode_Strict);
}

void
pillbig_set_replace_mode(PillBig pillbig, PillBigReplaceMode mode)
{
	pillbig_error_set(PillBigError_NotImplemented);
}

int
pillbig_get_files_count(PillBig pillbig)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, -1);
}

PillBigFileHash
pillbig_get_hash_by_filename(char *filename)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, -1);
}

int
pillbig_get_entry_index_by_hash(PillBig pillbig, PillBigFileHash hash)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, -1);
}

PillBigFileEntry *
pillbig_get_entry(PillBig pillbig, int index)
{
	SET_ERROR_RETURN_VALUE(PillBigError_NotImplemented, NULL);
}

PillBigError
pillbig_file_extract(PillBig pillbig, int index, FILE *output)
{
	SET_RETURN_ERROR(PillBigError_NotImplemented);
}

PillBigError
pillbig_file_replace(PillBig pillbig, int index, FILE *input)
{
	SET_RETURN_ERROR(PillBigError_NotImplemented);
}

void
pillbig_close(PillBig pillbig)
{
	pillbig_error_set(PillBigError_NotImplemented);
}
