/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/* *********************************************************
Copyright 1992 by Stichting Mathematisch Centrum, Amsterdam, The
Netherlands.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of Stichting Mathematisch
Centrum or CWI not be used in advertising or publicity pertaining to
distribution of the software without specific, written prior permission.

STICHTING MATHEMATISCH CENTRUM DISCLAIMS ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL STICHTING MATHEMATISCH CENTRUM BE LIABLE
FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT
OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/**
 *  @file
 *  @brief
 *  	IMA/Intel ADPCM format packer & unpacker.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *  @remarks
 *  Copyright 1992 by Stichting Mathematisch Centrum, Amsterdam,
 *  The Netherlands.
 */

#ifndef __PILLBIG_AUDIO_ADPCM_H__
#define __PILLBIG_AUDIO_ADPCM_H__

#include <stdio.h>
#include <pillbig/pillbig.h>
#include "audio_internal.h"

BEGIN_C_DECLS

/**
 *  Encodes a PCM audio into IMA ADPCM.
 *
 *  @param input
 *  	Input PCM stream.
 *  @param output
 *  	Output ADPCM stream.
 *  @param parameters
 *  	Audio parameters.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_audio_adpcm_encode(FILE *input, FILE *output,
	PillBigAudioParameters *parameters);

/**
 *  Decodes an IMA ADPCM audio into PCM.
 *
 *  @param input
 *  	Input ADPCM stream.
 *  @param output
 *  	Output PCM stream.
 *  @param parameters
 *  	Audio parameters.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_audio_adpcm_decode(FILE *input, FILE *output,
	PillBigAudioParameters *parameters);

END_C_DECLS

#endif
