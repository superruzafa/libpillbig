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

/**
 *  Audio formats.
 */
typedef enum
{
	PillBigAudioFormat_Autodetect = -1,    /**< Autodetect audio format */
	PillBigAudioFormat_Unknown = 0,        /**< Unknown audio format */
	PillBigAudioFormat_PCM,                /**< PCM audio format */
	PillBigAudioFormat_VAG,                /**< PlayStation VAG audio format */
	PillBigAudioFormat_ADPCM,              /**< ADPCM audio format */
	PillBigAudioFormat_WAVE,               /**< RIFF wave format */
}
PillBigAudioFormat;

#endif
