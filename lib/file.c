/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Basic pill.big metafile manipulation. Implementation file.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */



#include <malloc.h>
#include <string.h>
#include <ctype.h>
#include <pillbig/pillbig.h>
#include "common_internal.h"
#include "error_internal.h"
#include "file_internal.h"



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

/**
 *  Guesses the platform of a pill.big file.
 *
 *  @param pillbig
 *  	PillBig object with opened pill.big file.
 *  @return
 *  	PillBigPlatform.
 */
static PillBigPlatform
pillbig_guess_platform(PillBig pillbig);



PillBig
pillbig_open(FILE *input)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(input != NULL,
		PillBigError_InvalidStream, NULL);

	PillBig pillbig = (PillBig)malloc(sizeof(struct PillBig));
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_SystemError, NULL);

	/*
	 * Initialize the object.
	 */
	memset(pillbig, 0, sizeof(struct PillBig));
	pillbig->pillbig      = input;
	pillbig->platform     = PillBigPlatform_Unknown;
	pillbig->replace_mode = PillBigReplaceMode_Strict;

	/*
	 * Try to guess the pill.big platform.
	 */
	pillbig->platform = pillbig_guess_platform(pillbig);
	if (pillbig_no_error())
	{
		switch (pillbig->platform)
		{
			case PillBigPlatform_PC:
			case PillBigPlatform_PSX:
				/*
				 * Do nothing.
				 */
				break;
			default:
				/*
				 * Unsupported version of pill.big.
				 */
				pillbig_error_set(PillBigError_UnsupportedFormat);
				break;
		}
	}

	if (pillbig_no_error())
	{
		/*
		 * Try to read the pill.big's entries.
		 */
		PillBigError error = pillbig_read_file_entries(pillbig);
	}

	if (pillbig_any_error())
	{
		/*
		 * Cannot open pill.big. Free resources.
		 */
		if (pillbig->entries != NULL)
		{
			free(pillbig->entries);
		}
		free(pillbig);
		pillbig = NULL;
	}

	return pillbig;
}

PillBig
pillbig_open_from_filename(const char *filename)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(filename != NULL,
		PillBigError_InvalidFilename, NULL);

	PillBig pillbig = NULL;
	FILE *file = fopen(filename, "r+b");
	if (file == NULL)
	{
		/*
		 * Cannot open the file in read-write mode.
		 * Try to open in read-only mode.
		 */
		file = fopen(filename, "rb");
	}
	SET_ERROR_RETURN_VALUE_IF_FAIL(file != NULL, PillBigError_SystemError, NULL);

	pillbig = pillbig_open(file);

	/*
	 * We open the file here. Must be closed when pillbig_close() is called.
	 */
	if (pillbig != NULL)
	{
		pillbig->close_on_free = 1;
	}

	return pillbig;
}

PillBigPlatform
pillbig_get_platform(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject,
		PillBigPlatform_Unknown);

	return pillbig->platform;
}

PillBigReplaceMode
pillbig_get_replace_mode(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject,
		PillBigReplaceMode_Strict);

	return pillbig->replace_mode;
}

void
pillbig_set_replace_mode(PillBig pillbig, PillBigReplaceMode mode)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);
	SET_ERROR_RETURN_IF_FAIL(PillBigReplaceMode_Strict <= mode &&
		mode <= PillBigReplaceMode_AllowLargerFiles,
		PillBigError_InvalidReplaceMode);

	pillbig->replace_mode = mode;
}

unsigned int
pillbig_get_files_count(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject, -1);

	return pillbig->files_count;
}

PillBigFileHash
pillbig_get_hash_by_filename(const char *filename)
{
	#define HASH_BUFFER_SIZE 64
	/*
	 * Hash function courtesy of Ben Lincoln from The Lost Worlds.
	 */

	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(filename != NULL, PillBigError_InvalidFilename, -1);

	PillBigFileHash hashname;
	int filename_length, buffer_length;
	char buffer[HASH_BUFFER_SIZE];
	int i;

	/*
	 * Blood Omen's hash algorithm operates in groups of four characters
	 * so if the filename is not four characters long, it needs to be padded out
	 */
	filename_length = strlen(filename);
	buffer_length = filename_length + 4 - filename_length % 4;
	SET_ERROR_RETURN_VALUE_IF_FAIL(buffer_length <= HASH_BUFFER_SIZE, PillBigError_UnknownError, -1);

	memset(buffer, 0, HASH_BUFFER_SIZE);
	strcpy(buffer, filename);

	/*
	 * Replace directory slashes and backslashes by colons
	 * and convert chars to uppercase.
	 */
	i = 0;
	while (buffer[i] != '\0' && i < buffer_length)
	{
		switch (buffer[i])
		{
			case '/':
			case '\\':
				buffer[i] = ':';
				break;
			default:
				buffer[i] = toupper(buffer[i]);
				break;
		}
		i++;
	}

	/*
	 * Calculate the hashname.
	 */
	hashname = 0;
	for (i = 0; i < buffer_length; i += 4)
	{
		hashname ^= (buffer[i + 3] << 24) | (buffer[i + 2] << 16) |
		            (buffer[i + 1] << 8)  |  buffer[i];
	}

	return hashname;
}

int
pillbig_get_entry_index_by_hash(PillBig pillbig, PillBigFileHash hash)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject, -1);

	int i = 0;
	while (i < pillbig->files_count)
	{
		if (pillbig->entries[i].hash == hash)
		{
			return i;
		}
		i++;
	}

	return -1;
}

const PillBigFileEntry *
pillbig_get_entry(PillBig pillbig, int index)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject, NULL);
	SET_ERROR_RETURN_VALUE_IF_FAIL(0 <= index && index < pillbig->files_count,
		PillBigError_FileIndexOutOfRange, NULL);

	return (PillBigFileEntry *)&pillbig->entries[index];
}

void
pillbig_set_db(PillBig pillbig, PillBigDB db)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject);
	SET_ERROR_RETURN_IF_FAIL(db != NULL,
		PillBigError_UnknownError);

	pillbig->db = db;
}

PillBigDB
pillbig_get_db(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject);

	return pillbig->db;
}

PillBigFileType
pillbig_file_get_type(PillBig pillbig, int index)
{
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject, PillBigFileType_Unknown);
	SET_ERROR_RETURN_VALUE_IF_FAIL(0 <= index && index < pillbig->files_count,
		PillBigError_FileIndexOutOfRange, PillBigFileType_Unknown);

	PillBigFileType filetype = PillBigFileType_Unknown;

	if (pillbig->db != NULL)
	{
		const PillBigDBEntry *dbentry = pillbig_db_get_entry(pillbig->db, index);
		if (dbentry != NULL)
		{
			filetype = dbentry->filetype;
		}
	}

	return filetype;
}

PillBigError
pillbig_file_extract(PillBig pillbig, int index, FILE *output)
{
	#define EXTRACT_BUFFER_SIZE 1024

	pillbig_error_clear();
	SET_RETURN_ERROR_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject);
	SET_RETURN_ERROR_IF_FAIL(0 <= index && index < pillbig->files_count,
		PillBigError_FileIndexOutOfRange);
	SET_RETURN_ERROR_IF_FAIL(output != NULL, PillBigError_InvalidStream);

	PillBigFileEntry *entry = &pillbig->entries[index];
	SET_RETURN_ERROR_IF_FAIL(entry != NULL, PillBigError_UnknownError);

	char buffer[EXTRACT_BUFFER_SIZE], *ptr;
	int remaining_bytes = entry->size, result;
	int bytes_read, bytes_written;

	result = fseek(pillbig->pillbig, entry->offset, SEEK_SET);
	SET_RETURN_ERROR_IF_FAIL(result == 0, PillBigError_SystemError);

	while (remaining_bytes > 0)
	{
		bytes_read = fread(buffer, sizeof(char), MIN(EXTRACT_BUFFER_SIZE, remaining_bytes), pillbig->pillbig);
		remaining_bytes -= bytes_read;
		SET_RETURN_ERROR_IF_FAIL(bytes_read > 0, PillBigError_SystemError);
		ptr = buffer;
		while (bytes_read > 0)
		{
			bytes_written = fwrite(ptr, sizeof(char), bytes_read, output);
			bytes_read -= bytes_written;
			SET_RETURN_ERROR_IF_FAIL(bytes_written > 0, PillBigError_SystemError);
			ptr += bytes_written;
		}
	}

	return pillbig_error_get();
}

PillBigError
pillbig_file_extract_to_filename(PillBig pillbig, int index, const char *filename)
{
	pillbig_error_clear();
	SET_RETURN_ERROR_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject);
	SET_RETURN_ERROR_IF_FAIL(0 <= index && index < pillbig->files_count,
		PillBigError_FileIndexOutOfRange);
	SET_RETURN_ERROR_IF_FAIL(filename != NULL, PillBigError_InvalidFilename);

	FILE *file = fopen(filename, "wb");
	SET_RETURN_ERROR_IF_FAIL(file != NULL, PillBigError_SystemError);
	pillbig_file_extract(pillbig, index, file);
	fclose(file);

	return pillbig_error_get();
}

PillBigError
pillbig_file_replace(PillBig pillbig, int index, FILE *input)
{
	pillbig_error_set(PillBigError_NotImplemented);
	return pillbig_error_get();
}

void
pillbig_close(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);

	if (pillbig->close_on_free)
	{
		fclose(pillbig->pillbig);
	}

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
	SET_RETURN_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);

	int result;
	int i;

	result = fseek(pillbig->pillbig, 0, SEEK_SET);
	SET_RETURN_ERROR_IF_FAIL(result == 0, PillBigError_SystemError);

	result = fread(&pillbig->files_count, 4, 1, pillbig->pillbig);
	SET_RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);

	pillbig->entries = (PillBigFileEntry *)calloc(sizeof(PillBigFileEntry), pillbig->files_count);
	SET_RETURN_ERROR_IF_FAIL(pillbig->entries != NULL, PillBigError_SystemError);

	i = 0;
	while (i < pillbig->files_count)
	{
		result = fread(&pillbig->entries[i].hash, 4, 1, pillbig->pillbig);
		SET_RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);

		result = fread(&pillbig->entries[i].size, 4, 1, pillbig->pillbig);
		SET_RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);

		result = fread(&pillbig->entries[i].offset, 4, 1, pillbig->pillbig);
		SET_RETURN_ERROR_IF_FAIL(result == 1, PillBigError_SystemError);
		i++;
	}

	return PillBigError_Success;
}

static PillBigPlatform
pillbig_guess_platform(PillBig pillbig)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(pillbig != NULL,
		PillBigError_InvalidPillBigObject, PillBigPlatform_Unknown);

	PillBigPlatform platform = PillBigPlatform_Unknown;
	int result;
	int magic;

	result = fseek(pillbig->pillbig, 0, SEEK_SET);
	SET_ERROR_RETURN_VALUE_IF_FAIL(result == 0,
		PillBigError_SystemError, PillBigPlatform_Unknown);

	result = fread(&magic, 4, 1, pillbig->pillbig);
	SET_ERROR_RETURN_VALUE_IF_FAIL(result == 1,
		PillBigError_SystemError, PillBigPlatform_Unknown);

	switch (magic)
	{
		case FILES_COUNT_PC:
			platform = PillBigPlatform_PC;
			break;
		case FILES_COUNT_PSX:
			platform = PillBigPlatform_PSX;
			break;
		default:
			platform = PillBigPlatform_Unknown;
			break;
	}

	return platform;
}
