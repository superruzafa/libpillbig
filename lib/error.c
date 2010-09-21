/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Error management and definitions. Implementation file.
 */

#include <pillbig/error.h>
#include "error_internal.h"

static PillBigError last_error = PillBigError_Success;

PillBigError
pillbig_error_get()
{
	return last_error;
}

void
pillbig_error_set(PillBigError error)
{
	last_error = error;
}
