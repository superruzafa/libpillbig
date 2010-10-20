/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Internal shared implementation of the PillBig object.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#ifndef __PILLBIG_FILE_INTERNAL_H__
#define __PILLBIG_FILE_INTERNAL_H__

#include <stdio.h>
#include <pillbig/pillbig.h>

struct _PillBig
{
	FILE                *pillbig;          /**< pill.big file descriptor .*/
	PillBigPlatform      platform;         /**< Blood Omen's pill.big platform. */
	PillBigDB            db;               /**< Files database. */
	unsigned int         files_count;      /**< pill.big files count. */
	PillBigFileEntry    *entries;          /**< File entries table. */
	PillBigReplaceMode   replace_mode;     /**< Replacement mode. */
	int                  close_on_free;    /**< 1 if pill.big FILE must be closed when freeing the object. */
};

#endif
