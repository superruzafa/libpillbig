/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Error management and definitions.
 */

#ifndef __PILLBIG_ERROR_H__
#define __PILLBIG_ERROR_H__

typedef enum
{
	PillBigError_Success,                 /**< The last operation was done successfully. */
	PillBigError_UnknownError,            /**< An unknown error occured. */
	PillBigError_SystemError,             /**< A system error occured. */
	PillBigError_NotImplemented,

	PillBigError_InvalidPillBigObject,    /**< Invalid PillBig object. */

	PillBigError_ExternalFileShorter,     /**< The replacement file was shorter than expected. */
	PillBigError_ExternalFileLarger,      /**< The replacement file was larger than expected. */
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
