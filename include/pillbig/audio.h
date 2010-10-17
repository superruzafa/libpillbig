/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Audio management.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version $Id$
 */

#ifndef __PILLBIG_AUDIO_H__
#define __PILLBIG_AUDIO_H__

#include <stdio.h>
#include <pillbig/common.h>
#include <pillbig/error.h>
#include <pillbig/file.h>

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



BEGIN_C_DECLS

/**
 *  Extracts and converts a pill.big audio file into a stream.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big audio file index to be extracted and converted.
 *  @param output
 *  	Stream where the pill.big file will be extracted and converted.
 *  @param output_format
 *  	Conversion format.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_audio_extract(
	PillBig pillbig, int index, FILE *output,
	PillBigAudioFormat output_format);

/**
 *  Extracts and converts a pill.big audio file into a file.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big audio file index to be extracted and converted.
 *  @param filename
 *  	External file where the pill.big file will be extracted and converted..
 *  @param output_format
 *  	Conversion format.
 *  @return
 *  	Operation result.
 */
PillBigError
pillbig_audio_extract_to_filename(
	PillBig pillbig, int index, const char *filename,
    PillBigAudioFormat output_format);

/**
 *  Replaces the contents of a pill.big file from external audio data.
 *
 *  @see pillbig_file_replace()
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index.
 *  @param input
 *  	Input stream whose contents will replace the pill.big file contents.
 *  @return
 *  	Operation result.
 *  	- PillBigError_Success
 *  		If pill.big file could be replaced and both file sizes are
 *  		the same.
 *  	- PillBigError_ExternalFileShorter
 *  		When PillBig replacement mode allows shorter files and the
 *  		external file is shorter than the pill.big one.
 *  	- PillBigError_ExternalFileLarger
 *  		When PillBig replacement mode allows larger files and the
 *  		external file is larger than the pill.big one.
 */
PillBigError
pillbig_audio_replace(
	PillBig pillbig, int index, FILE *input,
	PillBigAudioFormat input_format);

/**
 *  Replaces the contents of a pill.big file from external audio file.
 *
 *  @see pillbig_file_replace()
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index.
 *  @param filename
 *  	Filename whose contents will replace the pill.big file contents.
 *  @param input_format
 *  	Audio input format.
 *  @return
 *  	Operation result.
 *  	- PillBigError_Success
 *  		If pill.big file could be replaced and both file sizes are
 *  		the same.
 *  	- PillBigError_ExternalFileShorter
 *  		When PillBig replacement mode allows shorter files and the
 *  		external file is shorter than the pill.big one.
 *  	- PillBigError_ExternalFileLarger
 *  		When PillBig replacement mode allows larger files and the
 *  		external file is larger than the pill.big one.
 */
PillBigError
pillbig_audio_replace_from_filename(
	PillBig pillbig, int index, const char *filename,
	PillBigAudioFormat input_format);

/**
 *  Gets the audio format.
 *
 *  @param pillbig
 *  	PillBig object.
 *  @param index
 *  	pill.big file index.
 *  @return
 *  	Audio format if sucessful. PillBigAudioFormat_Unknown otherwise.
 */
PillBigAudioFormat
pillbig_audio_get_format(PillBig pillbig, int index);

END_C_DECLS

#endif
