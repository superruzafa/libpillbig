/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Basic pill.big metafile manipulation.
 */

#ifndef __PILLBIG_FILE_H__
#define __PILLBIG_FILE_H__

#include <stdio.h>
#include <pillbig/common.h>
#include <pillbig/error.h>

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

typedef enum
{
	PillBigReplaceMode_Strict,               /**< The replace file size must be the same than the pill.big one. */
	PillBigReplaceMode_AllowShorterFiles,    /**< The replace file size must be shorter or equal than the pill.big one. */
	PillBigReplaceMode_AllowLargerFiles,     /**< The replace file size could be different than the pill.big one. */
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
int
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
PillBigFileEntry *
pillbig_get_entry(PillBig pillbig, int index);

/**
 *  Dumps the contents of a pill.big file.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index to be dumped.
 *  @param output
 *  	External file where the pill.big file will be dumped.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_file_extract(PillBig pillbig, int index, FILE *output);

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
 *  @see pillbig_file_append()
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
 *  		if pill.big file could be replaced and both file sizes are
 *  		the same.
 *  	- PillBigError_ExternalFileShorter
 *  		if file could be replaced but external file was shorter than
 *  		pill.big file.
 */
PillBigError
pillbig_file_replace(PillBig pillbig, int index, FILE *input);

/**
 *  Closes a PillBig object.
 *
 *  @param pillbig
 *  	PillBig object.
 */
void
pillbig_close(PillBig pillbig);

END_C_DECLS

#endif
