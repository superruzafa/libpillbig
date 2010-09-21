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



#include <malloc.h>
#include <string.h>
#include <pillbig/pillbig.h>
#include "error_internal.h"



struct PillBig
{
	FILE                *pillbig;         /**< pill.big file descriptor .*/
	PillBigPlatform      platform;        /**< Blood Omen's pill.big platform. */
	unsigned int         files_count;     /**< pill.big files count. */
	PillBigFileEntry    *entries;         /**< File entries table. */
	PillBigReplaceMode   replace_mode;    /**< Replacement mode. */
};



/**
 *  Reads the file entries table.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @return
 *  	Error occured.
 */
static PillBigError
pillbig_read_file_entries(PillBig pillbig);



PillBig
pillbig_open(FILE *input)
{
	pillbig_error_clear();
	SET_ERROR_IF_FAIL(input != NULL, PillBigError_InvalidStream);
	RETURN_VALUE_IF_FAIL(input != NULL, NULL);

	PillBig pillbig = (PillBig)malloc(sizeof(struct PillBig));
	SET_ERROR_IF_FAIL(pillbig != NULL, PillBigError_SystemError);
	RETURN_VALUE_IF_FAIL(pillbig != NULL, NULL);

	memset(pillbig, 0, sizeof(struct PillBig));

	pillbig->pillbig      = input;
	pillbig->platform     = PillBigPlatform_Unknown;
	pillbig->replace_mode = PillBigReplaceMode_Strict;

	PillBigError error = pillbig_read_file_entries(pillbig);
	if (error != PillBigError_Success)
	{
		if (pillbig->entries != NULL)
		{
			free(pillbig->entries);
		}
		free(pillbig);
		pillbig = NULL;
	}

	return pillbig;
}

PillBigPlatform
pillbig_get_platform(PillBig pillbig)
{
	pillbig_error_set(PillBigError_NotImplemented);

	return PillBigPlatform_Unknown;
}

PillBigReplaceMode
pillbig_get_replace_mode(PillBig pillbig)
{
	pillbig_error_set(PillBigError_NotImplemented);
	return PillBigReplaceMode_Strict;
}

void
pillbig_set_replace_mode(PillBig pillbig, PillBigReplaceMode mode)
{
	pillbig_error_set(PillBigError_NotImplemented);
}

unsigned int
pillbig_get_files_count(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);
	RETURN_VALUE_IF_FAIL(pillbig != NULL, -1);

	return pillbig->files_count;
}

PillBigFileHash
pillbig_get_hash_by_filename(char *filename)
{
	pillbig_error_set(PillBigError_NotImplemented);

	return -1;
}

unsigned int
pillbig_get_entry_index_by_hash(PillBig pillbig, PillBigFileHash hash)
{
	pillbig_error_set(PillBigError_NotImplemented);

	return -1;
}

PillBigFileEntry *
pillbig_get_entry(PillBig pillbig, unsigned int index)
{
	pillbig_error_clear();
	SET_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);
	RETURN_VALUE_IF_FAIL(pillbig != NULL, NULL);

	SET_ERROR_IF_FAIL(0 <= index && index < pillbig->files_count, PillBigError_FileIndexOutOfRange);
	RETURN_VALUE_IF_FAIL(0 <= index && index < pillbig->files_count, NULL);

	return (PillBigFileEntry *)&pillbig->entries[index];
}

PillBigError
pillbig_file_extract(PillBig pillbig, unsigned int index, FILE *output)
{
	pillbig_error_set(PillBigError_NotImplemented);
	return pillbig_error_get();
}

PillBigError
pillbig_file_replace(PillBig pillbig, unsigned int index, FILE *input)
{
	pillbig_error_set(PillBigError_NotImplemented);
	return pillbig_error_get();
}

void
pillbig_close(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);
	RETURN_IF_FAIL(pillbig != NULL);

	if (pillbig->entries != NULL)
	{
		free(pillbig->entries);
	}

	free(pillbig);
}



static PillBigError
pillbig_read_file_entries(PillBig pillbig)
{
	pillbig_error_clear();
	RETURN_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);

	int result;
	int i;

	result = fseek(pillbig->pillbig, 0, SEEK_SET);
	RETURN_ERROR_IF_FAIL(result == 0, PillBigError_SystemError);

	result = fread(&pillbig->files_count, 4, 1, pillbig->pillbig);
	RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);

	pillbig->entries = (PillBigFileEntry *)calloc(sizeof(PillBigFileEntry), pillbig->files_count);
	RETURN_ERROR_IF_FAIL(pillbig->entries != NULL, PillBigError_SystemError);

	i = 0;
	while (i < pillbig->files_count)
	{
		result = fread(&pillbig->entries[i].hash, 4, 1, pillbig->pillbig);
		RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);

		result = fread(&pillbig->entries[i].size, 4, 1, pillbig->pillbig);
		RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);

		result = fread(&pillbig->entries[i].offset, 4, 1, pillbig->pillbig);
		RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);
		i++;
	}

	return PillBigError_Success;
}
