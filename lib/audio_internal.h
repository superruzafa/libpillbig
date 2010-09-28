/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Internal audio macros and definitions.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#ifndef __PILLBIG_AUDIO_INTERNAL_H__
#define __PILLBIG_AUDIO_INTERNAL_H__

#define VAG_MAGIC_ID 0x70474156



typedef struct
{
	int samples_count;
	int sample_rate;
	int channels_count;
	int bits_per_sample;
}
PillBigAudioParameters;

#endif
