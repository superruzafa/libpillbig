/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	PlayStation ADPCM VAG format packer & unpacker.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *  @remarks
 *  	Modified from the VAG packer & depacker of bITmASTER@bigfoot.com
 *  	(http://hitmen.c02.at/html/psx_faq.html)
 */

#ifndef __PILLBIG_AUDIO_VAG_H__
#define __PILLBIG_AUDIO_VAG_H__

#include <stdio.h>
#include <pillbig/common.h>
#include "audio_internal.h"

BEGIN_C_DECLS

/**
 *  Encodes a PCM audio into PlayStation VAG ADPCM.
 *
 *  @param input
 *  	Input PCM stream.
 *  @param output
 *  	Output VAG ADPCM stream.
 *  @param parameters
 *  	Audio parameters.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_audio_vag_encode(FILE *input, FILE *output,
	PillBigAudioParameters *parameters);

/**
 *  Decodes a PlayStation VAG ADPCM audio into PCM.
 *
 *  @param input
 *  	Input VAG ADPCM stream.
 *  @param output
 *  	Output PCM stream.
 *  @param parameters
 *  	Audio parameters.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_audio_vag_decode(FILE *input, FILE *output,
	PillBigAudioParameters *parameters);

/**
 *  Counts the samples of the non-standard headerless VAG files.
 *
 *  @param input
 *  	Input VAG ADPCM stream.
 *  @return
 *  	Samples count if successful. -1 otherwise.
 */
int
pillbig_audio_vag_count_samples(FILE *input);

END_C_DECLS

#endif
