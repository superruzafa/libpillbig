/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Internal error macros and definitions.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#ifndef __PILLBIG_INTERNAL_ERROR_H__
#define __PILLBIG_INTERNAL_ERROR_H__

#include <pillbig/error.h>

#define RETURN_IF_FAIL(assert) \
	{if (!(assert)) return;}

#define RETURN_VALUE_IF_FAIL(assert, value) \
	{if (!(assert)) return value;}

#define SET_ERROR_IF_FAIL(assert, error) \
	{if (!(assert)) {pillbig_error_set(error);}}

#define SET_ERROR_RETURN_IF_FAIL(assert, error) \
	{if (!(assert)) {pillbig_error_set(error); return;}}

#define SET_ERROR_RETURN_VALUE_IF_FAIL(assert, error, value) \
	{if (!(assert)) {pillbig_error_set(error); return value;}}

#define SET_RETURN_ERROR_IF_FAIL(assert, error) \
	{if (!(assert)) {pillbig_error_set(error); return pillbig_error_get();}}

#define pillbig_error_clear()    (pillbig_error_set(PillBigError_Success))
#define pillbig_any_error()      (pillbig_error_get() != PillBigError_Success)
#define pillbig_no_error()       (pillbig_error_get() == PillBigError_Success)

/**
 *  Sets the last occured error.
 *
 *  @param error
 *  	Last occured error.
 */
void
pillbig_error_set(PillBigError error);

#endif
