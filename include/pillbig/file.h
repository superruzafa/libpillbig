/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Basic pill.big metafile manipulation.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version $Id$
 */

#ifndef __PILLBIG_FILE_H__
#define __PILLBIG_FILE_H__

#include <stdio.h>
#include <pillbig/common.h>
#include <pillbig/error.h>



/**
 *  Number of files in the pill.big file, PC version.
 */
#define FILES_COUNT_PC 2572

/**
 *  Number of files in the pill.big file, PSX version.
 */
#define FILES_COUNT_PSX 2321



/**
 *  Game platform.
 */
typedef enum
{
	PillBigPlatform_Unknown,    /**< Unknown platform. */
	PillBigPlatform_PC,         /**< PC platform. */
	PillBigPlatform_PSX,        /**< PlayStation platform. */
}
PillBigPlatform;

/**
 *  Replacement modes.
 */
typedef enum
{
	PillBigReplaceMode_Strict,               /**< The replace file size must be the same than the one from pill.big. */
	PillBigReplaceMode_AllowShorterFiles,    /**< The replace file size must be shorter or equal than the one from pill.big. */
	PillBigReplaceMode_AllowLargerFiles,     /**< The replace file size could be different than the one from pill.big. */
}
PillBigReplaceMode;

/**
 *  pill.big file hashname.
 */
typedef unsigned int PillBigFileHash;

/**
 *  pill.big file entry.
 */
typedef struct
{
	PillBigFileHash    hash;      /**< File hashname. */
	int                offset;    /**< File offset. */
	int                size;      /**< File size. */
}
PillBigFileEntry;

/**
 *  PillBig object.
 */
typedef struct PillBig *PillBig;



BEGIN_C_DECLS

/**
 *  Opens a pill.big file.
 *
 *  @param input
 *  	Opened stream to a pill.big file.
 *  @return
 *  	PillBig object if successful.
 *  	NULL otherwise.
 */
PillBig
pillbig_open(FILE *input);

/**
 *  Opens a pill.big file given its filename.
 *
 *  @param
 *  	filename pill.big file name.
 *  @return
 *  	PillBig object if successful.
 *  	NULL otherwise.
 */
PillBig
pillbig_open_from_filename(char *filename);

/**
 *  Gets the platform for the Blood Omen's pill.big metafile.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @return
 *  	Blood Omen platform if successful.
 *  	PillBigPlatform_Unknown otherwise.
 */
PillBigPlatform
pillbig_get_platform(PillBig pillbig);

/**
 *  Gets the current PillBig replacement mode.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @return
 *  	PillBigReplaceMode.
 */
PillBigReplaceMode
pillbig_get_replace_mode(PillBig pillbig);

/**
 *  Sets the current PillBig replacement mode.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param mode
 *  	New replacement mode.
 *  @return
 *  	PillBigReplaceMode.
 */
void
pillbig_set_replace_mode(PillBig pillbig, PillBigReplaceMode mode);

/**
 *  Gets the count of stored files in pill.big file.
 *
 *  @param pillbig
 *  	PillBig object
 *  @return
 *  	Count of stored files in pill.big file if successful.
 *  	-1 otherwise.
 */
unsigned int
pillbig_get_files_count(PillBig pillbig);

/**
 *  Encodes a filename to its pill.big hashname.
 *
 *  @param filename
 *  	External filename.
 *  @return
 *  	pill.big hashname if successful. -1 otherwise.
 */
PillBigFileHash
pillbig_get_hash_by_filename(char *filename);

/**
 *  Gets the index of a pill.big file looking for its hashname.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param hash
 *  	pill.big file hash.
 *  @return
 *  	pill.big file index.
 */
int
pillbig_get_entry_index_by_hash(PillBig pillbig, PillBigFileHash hash);

/**
 *  Gets the entry corresponding to a pill.big file by its index.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index.
 *  @return
 *  	PillBigFileEntry object if successful.
 *  	NULL otherwise.
 */
const PillBigFileEntry *
pillbig_get_entry(PillBig pillbig, unsigned int index);

/**
 *  Dumps the contents of a pill.big file into a stream.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index to be dumped.
 *  @param output
 *  	Stream where the pill.big file will be dumped.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_file_extract(PillBig pillbig, unsigned int index, FILE *output);

/**
 * Dumps the contents of a pill.big file into a file.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index to be dumped.
 *  @param filename
 *  	External file where the pill.big file will be dumped.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_file_extract_to_filename(PillBig pillbig, unsigned int index, char *filename);

/**
 *  Replaces the contents of a pill.big file with external raw data.
 *
 *  @remarks
 *  This function replaces pill.big files with external ones, their size
 *  must be the same. Otherwise an error will be generated.
 *
 *  In order to allow shorter files you must set the PillBig's replacement
 *  mode to PillBigReplaceMode_AllowShorterFiles.
 *
 *  @warning
 *  Replacing files for shorter ones will modify the file entry's size.
 *  Replacing files by larger ones will modify the file entry's offset
 *  and size. Additionally the only way to replace files by larger ones
 *  is appending them to pill.big so pill.big size will grown.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index.
 *  @param input
 *  	Input stream whose contents will replace the pill.big file contents.
 *  @return
 *  	Operation result.
 *  	- PillBigError_Success
 *  		If pill.big file could be replaced and both file sizes are
 *  		the same.
 *  	- PillBigError_ExternalFileShorter
 *  		When PillBig replacement mode allows shorter files and the
 *  		external file is shorter than the pill.big one.
 *  	- PillBigError_ExternalFileLarger
 *  		When PillBig replacement mode allows larger files and the
 *  		external file is larger than the pill.big one.
 */
PillBigError
pillbig_file_replace(PillBig pillbig, unsigned int index, FILE *input);

/**
 *  Closes a PillBig object.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @remarks
 *  	If pill.big was opened with pillbig_open(), pillbig_close()
 *  won't close the related FILE, you should close manually.
 */
void
pillbig_close(PillBig pillbig);

END_C_DECLS

#endif
