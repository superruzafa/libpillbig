/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Error management and definitions.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#ifndef __PILLBIG_ERROR_H__
#define __PILLBIG_ERROR_H__

typedef enum
{
	PillBigError_Success,                       /**< The last operation was done successfully. */
	PillBigError_UnknownError,                  /**< An unknown error occured. */
	PillBigError_SystemError,                   /**< A system error occured. */
	PillBigError_NotImplemented,                /**< Current function isn't implemented yet. */
	PillBigError_UnsupportedFormat,             /**< Unsupported format. */

	PillBigError_InvalidPillBigObject = 32,     /**< Invalid PillBig object. */
	PillBigError_InvalidStream,                 /**< Invalid stream. */
	PillBigError_InvalidReplaceMode,            /**< Invalid replacement mode. */
	PillBigError_InvalidFilename,               /**< Provided filename was invalid. */

	PillBigError_FileIndexOutOfRange = 64,      /**< File index out of range. */
	PillBigError_ExternalFileShorter,           /**< The replacement file was shorter than expected. */
	PillBigError_ExternalFileLarger,            /**< The replacement file was larger than expected. */
}
PillBigError;

/**
 *  Returns the last ocurred error.
 *  @return
 *  	Last error occured.
 */
PillBigError
pillbig_error_get();

#endif
