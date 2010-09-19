/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Internal error macros and definitions.
 */

#ifndef __PILLBIG_INTERNAL_ERROR_H__
#define __PILLBIG_INTERNAL_ERROR_H__

#include <pillbig/error.h>

#define RETURN_IF_FAIL(assert)                                  {if (!(assert)) return;}
#define RETURN_VALUE_IF_FAIL(assert, value)                     {if (!(assert)) return value;}
#define SET_ERROR_RETURN_VALUE(error, value)                    {pillbig_error_set(error); return value;}
#define SET_RETURN_ERROR(error)                                 SET_ERROR_RETURN_VALUE(error, error)
#define SET_ERROR_RETURN_VALUE_IF_FAIL(assert, error, value)    {if (!(assert)) SET_ERROR_RETURN_VALUE(error, value)}

#define pillbig_error_clear() pillbig_error_set_last(PillBigError_Success);

/**
 *  Sets the last occured error.
 *
 *  @param error
 *  	Last occured error.
 */
void
pillbig_error_set(PillBigError error);

#endif
