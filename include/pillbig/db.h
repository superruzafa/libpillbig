/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	API for the pill.big file information database.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version $Id$
 */

#ifndef __PILLBIG_DB_H__
#define __PILLBIG_DB_H__

#include <pillbig/common.h>

/**
 *  Database entry.
 */
typedef struct PillBigDBEntry
{
	PillBigFileHash  hash;        /**< File hash. */
	int              offset;      /**< File offset. */
	int              size;        /**< File size. */
	char            *filename;    /**< Internal name. */
}
PillBigDBEntry;

/**
 *  Database object.
 */
typedef struct PillBigDB *PillBigDB;



BEGIN_C_DECLS

/**
 *  Opens a pill.big database.
 *
 *  @param filename
 *  	Database filename.
 *  @return
 *  	PillBigDB if successful.
 *  	NULL otherwise.
 */
PillBigDB
pillbig_db_open(char *filename);

/**
 *  Gets the count of stored files in the database.
 *
 *  @param db
 *  	PillBigDB object.
 *  @return
 *
 */
int
pillbig_db_get_files_count(PillBigDB db);

/**
 *  Gets an entry from the database.
 *
 *  @param db
 *  	PillBigDB object.
 *  @param index
 *  	File index.
 *  @return
 *  	PillBigDBEntry if successful.
 *  	NULL otherwise.
 */
const PillBigDBEntry *
pillbig_db_get_entry(PillBigDB db, int index);

/**
 *  Gets the index of a database file looking for its hashname.
 *
 *  @param db
 *  	PillBigDB object.
 *  @param hash
 *  	File hashname.
 *  @return
 *  	File index if found. -1 otherwise.
 */
int
pillbig_db_get_entry_by_hash(PillBigDB db, PillBigFileHash hash);

/**
 *  Closes a database.
 *
 *  @param db
 *  	PillBigDB object.
 */
void
pillbig_db_close(PillBigDB db);

END_C_DECLS

#endif
