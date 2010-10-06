/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Audio management. Implementation file.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */



#include <stdio.h>
#include <malloc.h>
#include <pillbig/pillbig.h>
#include "error_internal.h"
#include "file_internal.h"
#include "audio_internal.h"

#include "adpcm.h"
#include "vag.h"


/**
 *  Callback for codecs.
 */
typedef
PillBigError
(*PillBigAudioConverterCallback)(
	FILE *input, FILE *output, PillBigAudioParameters *parameters);



/**
 *  Writes a RIFF WAVE header.
 *
 *  @param output
 *  	Output stream.
 *  @param parameters
 *  	Audio parameters.
 *  @return
 *  	Operation result.
 */
static PillBigError
pillbig_audio_write_wave_header(FILE *output, PillBigAudioParameters *parameters);

/**
 *  Writes a PSX VAG header.
 *
 *  @param output
 *  	Output stream.
 *  @param parameters
 *  	Audio parameters.
 *  @return
 *  	Operation result.
 */
static PillBigError
pillbig_audio_write_vag_header(FILE *output, PillBigAudioParameters *parameters);



PillBigError
pillbig_audio_extract(PillBig pillbig, int index, FILE *output,
                      PillBigAudioFormat output_format)
{
	pillbig_error_clear();
	SET_RETURN_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);
	SET_RETURN_ERROR_IF_FAIL(0 <= index && index < pillbig->files_count,
		PillBigError_FileIndexOutOfRange);
	SET_RETURN_ERROR_IF_FAIL(output != NULL, PillBigError_InvalidStream);

	PillBigAudioConverterCallback callback = NULL;
	PillBigAudioParameters parameters;
	int result;

	/*
	 * FIX: Input format should be autodetected or queried to a database.
	 */
	PillBigAudioFormat input_format = pillbig_audio_get_format(pillbig, index);

	if (input_format == output_format)
	{
		pillbig_file_extract(pillbig, index, output);
	}
	else
	{
		result = fseek(pillbig->pillbig, pillbig->entries[index].offset, SEEK_SET);
		SET_RETURN_ERROR_IF_FAIL(result == 0, PillBigError_SystemError);

		switch (input_format)
		{
			case PillBigAudioFormat_ADPCM:
				/* There are 2 IMA ADPCM samples for each byte. */
				parameters.samples_count   = pillbig->entries[index].size * 2;
				parameters.sample_rate     = 11025;
				parameters.channels_count  = 1;
				parameters.bits_per_sample = 16;

				callback = pillbig_audio_adpcm_decode;
				break;

			case PillBigAudioFormat_VAG:
				/*
				 * VAG audio files from the PSX version of Blood Omen
				 * has a non-standard headerless format. The lenght of the
				 * audio must be calculated on-the-fly.
				 */
				parameters.samples_count   =
					pillbig_audio_vag_get_samples_count(pillbig->pillbig,
					pillbig->entries[index].size);
				parameters.sample_rate     = 11025;
				parameters.channels_count  = 1;
				parameters.bits_per_sample = 16;

				callback = pillbig_audio_vag_decode;

				/*
				 * If this file has a header then skip it.
				 */
				if (pillbig_audio_vag_has_header(pillbig->pillbig))
				{
					result = fseek(pillbig->pillbig, 64, SEEK_CUR);
					SET_RETURN_ERROR_IF_FAIL(result == 0, PillBigError_SystemError);
				}
				break;
		}
	}

	if (callback != NULL)
	{
		if (output_format == PillBigAudioFormat_WAVE)
		{
			pillbig_audio_write_wave_header(output, &parameters);
		}
		pillbig_error_set(callback(pillbig->pillbig, output, &parameters));
	}
	else
	{
		pillbig_error_set(PillBigError_NotImplemented);
	}

	return pillbig_error_get();
}

PillBigError
pillbig_audio_extract_to_filename(
	PillBig pillbig, int index, char *filename,
    PillBigAudioFormat output_format)
{
	pillbig_error_clear();
	SET_RETURN_ERROR_IF_FAIL(pillbig != NULL, PillBigError_InvalidPillBigObject);
	SET_RETURN_ERROR_IF_FAIL(0 <= index && index < pillbig->files_count,
		PillBigError_FileIndexOutOfRange);
	SET_RETURN_ERROR_IF_FAIL(filename != NULL, PillBigError_InvalidFilename);

	FILE *output = fopen(filename, "wb");
	SET_RETURN_ERROR_IF_FAIL(output != NULL, PillBigError_SystemError);
	pillbig_audio_extract(pillbig, index, output, output_format);
	fclose(output);

	return pillbig_error_get();
}

PillBigError
pillbig_audio_replace(
	PillBig pillbig, int index, FILE *input,
	PillBigAudioFormat input_format)
{
	pillbig_error_set(PillBigError_NotImplemented);
	return pillbig_error_get();
}

PillBigError
pillbig_audio_replace_from_filename(
	PillBig pillbig, int index, char *filename,
	PillBigAudioFormat input_format)
{
	pillbig_error_set(PillBigError_NotImplemented);
	return pillbig_error_get();
}

PillBigAudioFormat
pillbig_audio_get_format(PillBig pillbig, int index)
{
	PillBigAudioFormat format = PillBigAudioFormat_Unknown;

	int result;
	int magic32, magic16;

	result = fseek(pillbig->pillbig, pillbig->entries[index].offset, SEEK_SET);
	result = fread(&magic32, 4, 1, pillbig->pillbig);
	magic16 = (magic32 & 0xffff);

	switch (pillbig->platform)
	{
		case PillBigPlatform_PC:
			if (magic16 == 0x0000)
			{
				format = PillBigAudioFormat_ADPCM;
			}
			else if (magic16 == 0x0200 || magic32 == VAG_MAGIC_ID)
			{
				format = PillBigAudioFormat_VAG;
			}
			break;
		case PillBigPlatform_PSX:
			if (magic16 == 0x0200 || magic16 == 0x0000 || magic32 == VAG_MAGIC_ID)
			{
				format = PillBigAudioFormat_VAG;
			}
			break;
		default:
			/* TODO: add assert */
			break;
	}

	return format;
}



static PillBigError
pillbig_audio_write_wave_header(FILE *output, PillBigAudioParameters *parameters)
{
	#define RIFF 0x46464952
	#define WAVE 0x45564157
	#define fmt  0x20746D66
	#define data 0x61746164

	#define WRITE_WAVE_FIELD_2(v) {value = v; value = fwrite(&value, 2, 1, output); SET_RETURN_ERROR_IF_FAIL(value == 1, PillBigError_SystemError);}
	#define WRITE_WAVE_FIELD_4(v) {value = v; value = fwrite(&value, 4, 1, output); SET_RETURN_ERROR_IF_FAIL(value == 1, PillBigError_SystemError);}

	int value;
	int bytes_per_sample = parameters->bits_per_sample / 8;

	/* RIFF chunk Id */
	WRITE_WAVE_FIELD_4(RIFF);
	/* All chunks size starting next field */
	WRITE_WAVE_FIELD_4(36 + parameters->samples_count * bytes_per_sample);
	/* WAVE Id */
	WRITE_WAVE_FIELD_4(WAVE);

	/* Format chunk Id */
	WRITE_WAVE_FIELD_4(fmt);
	/* Chunk size */
	WRITE_WAVE_FIELD_4(16);
	/* Audio format, 1 = PCM */
	WRITE_WAVE_FIELD_2(1);
	/* Number of channels */
	WRITE_WAVE_FIELD_2(parameters->channels_count);
	/* Sample rate */
	WRITE_WAVE_FIELD_4(parameters->sample_rate);
	/* Byte rate */
	WRITE_WAVE_FIELD_4(parameters->sample_rate * parameters->channels_count * bytes_per_sample);
	/* Block align */
	WRITE_WAVE_FIELD_2(parameters->channels_count * bytes_per_sample);
	/* Bits per sample */
	WRITE_WAVE_FIELD_2(parameters->bits_per_sample);

	/* DATA chunk */
	WRITE_WAVE_FIELD_4(data);
	/* Chunk size */
	WRITE_WAVE_FIELD_4(parameters->samples_count * bytes_per_sample);

	return PillBigError_Success;
}
