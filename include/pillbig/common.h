/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Common macros and definitions.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#ifndef __PILLBIG_COMMON_H__
#define __PILLBIG_COMMON_H__

#ifdef __cplusplus
#	define BEGIN_C_DECLS extern "C" {
#	define END_C_DECLS   }
#else
#	define BEGIN_C_DECLS
#	define END_C_DECLS
#endif

/**
 *  pill.big file hashname.
 */
typedef unsigned int PillBigFileHash;

/**
 *  File types.
 */
typedef enum
{
	PillBigFileType_Unknown,    /**< Unknown file type. */
	PillBigFileType_Audio,      /**< Audio file type. */
	PillBigFileType_Bitmap,     /**< Bitmap file type. */
	PillBigFileType_Tilemap,    /**< Tilemap file type. */
	PillBigFileType_Sprite,     /**< Sprite file type. */
	PillBigFileType_Map,        /**< Map file type. */
}
PillBigFileType;

#endif
