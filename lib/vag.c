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



#include <stdio.h>
#include <string.h>
#include <pillbig/pillbig.h>
#include "error_internal.h"
#include "common_internal.h"
#include "vag.h"



double f[5][2] =
{
	{          0.0,          0.0 },
	{  60.0 / 64.0,          0.0 },
	{ 115.0 / 64.0, -52.0 / 64.0 },
	{  98.0 / 64.0, -55.0 / 64.0 },
	{ 122.0 / 64.0, -60.0 / 64.0 }
};



PillBigError
pillbig_audio_vag_decode(FILE *input, FILE *output,
	PillBigAudioParameters *parameters)
{
	SET_RETURN_ERROR_IF_FAIL(input != NULL, PillBigError_InvalidStream);
	SET_RETURN_ERROR_IF_FAIL(output != NULL, PillBigError_InvalidStream);
	SET_RETURN_ERROR_IF_FAIL(parameters != NULL, PillBigError_UnknownError);

	int remaining_samples = parameters->samples_count;
	int predict_nr, shift_factor, flags;
	int i, d, s;
	int result;
	double samples[28];
	double s_1 = 0.0, s_2 = 0.0;

	parameters->samples_count = 0;

	predict_nr = fgetc(input);
	if (predict_nr != EOF)
	{
		flags = fgetc(input);
	}
	while (!feof(input) && remaining_samples > 0
	       && flags != 0x07 && flags != 0x05)
	{
		shift_factor = predict_nr & 0x0f;
		predict_nr >>= 4;

		i = 0;
		while (!feof(input) && i < 28)
		{
			d = fgetc(input);
			s = (d & 0x0f) << 12;
			if (s & 0x8000)
			{
				s |= 0xffff0000;
			}
			samples[i] = (double)(s >> shift_factor);

			s = (d & 0xf0) << 8;
			if (s & 0x8000)
			{
				s |= 0xffff0000;
			}
			samples[i + 1] = (double)(s >> shift_factor);
			i += 2;
		}

		i = 0;
		while (remaining_samples > 0 && i < 28)
		{
			samples[i] += s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
			s_2 = s_1;
			s_1 = samples[i];
			d = (int)(samples[i] + 0.5);
			result = fputc(d & 0xff, output);
			SET_ERROR_RETURN_IF_FAIL(result != EOF, PillBigError_SystemError);
			result = fputc(d >> 8, output);
			SET_ERROR_RETURN_IF_FAIL(result != EOF, PillBigError_SystemError);

			i++;
		}

		predict_nr = fgetc(input);
		if (predict_nr != EOF)
		{
			flags = fgetc(input);
		}

		remaining_samples -= 28;
	}

	while (remaining_samples-- > 0)
	{
		fputc(0, output);
	}

	return PillBigError_Success;
}

int
pillbig_audio_vag_get_samples_count(FILE *input, int filesize)
{
	int file_position;
	int samples_count = 0;
	int flags;
	int result;

	file_position = ftell(input);

	if (pillbig_audio_vag_has_header(input))
	{
		result = fseek(input, 64, SEEK_CUR);
		SET_ERROR_RETURN_VALUE_IF_FAIL(result == 0, PillBigError_SystemError, -1);
		filesize -= 64;
	}

	result = fseek(input, 1, SEEK_CUR);
	SET_ERROR_RETURN_VALUE_IF_FAIL(result == 0, PillBigError_SystemError, -1);
	flags = fgetc(input);
	SET_ERROR_RETURN_VALUE_IF_FAIL(flags != EOF, PillBigError_SystemError, -1);

	while (!feof(input) &&
	       flags != 0x05 && flags != 0x07)
	{
		samples_count += 28;
		result = fseek(input, 15, SEEK_CUR);
		SET_ERROR_RETURN_VALUE_IF_FAIL(result == 0, PillBigError_SystemError, -1);
		flags = fgetc(input);
		SET_ERROR_RETURN_VALUE_IF_FAIL(flags != EOF, PillBigError_SystemError, -1);
	}

	fseek(input, file_position, SEEK_SET);

	return MAX(MIN((filesize - 16) / 16 * 28, samples_count), 0);
}

int
pillbig_audio_vag_has_header(FILE *input)
{
	int magic32;
	int result;

	result = fread(&magic32, 4, 1, input);
	fseek(input, -4, SEEK_CUR);

	return magic32 == VAG_MAGIC_ID;
}
