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
 *  	IMA/Intel ADPCM format packer & unpacker. Implementation.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *  @remarks
 *  Copyright 1992 by Stichting Mathematisch Centrum, Amsterdam,
 *  The Netherlands.
 */

#include <stdio.h>
#include <malloc.h>
#include "adpcm.h"
#include "error_internal.h"

struct adpcm_state
{
	short valprev;    /* Previous output value */
	char index;       /* Index into stepsize table */
};

/* Intel ADPCM step variation table */
int indexTable[] = {
	-1, -1, -1, -1, 2, 4, 6, 8,
	-1, -1, -1, -1, 2, 4, 6, 8,
};

int stepsizeTable[] = {
	7, 8, 9, 10, 11, 12, 13, 14, 16, 17,
	19, 21, 23, 25, 28, 31, 34, 37, 41, 45,
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118,
	130, 143, 157, 173, 190, 209, 230, 253, 279, 307,
	337, 371, 408, 449, 494, 544, 598, 658, 724, 796,
	876, 963, 1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066,
	2272, 2499, 2749, 3024, 3327, 3660, 4026, 4428, 4871, 5358,
	5894, 6484, 7132, 7845, 8630, 9493, 10442, 11487, 12635, 13899,
	15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 32767
};

static void
adpcm_coder(short *indata, char *outdata, int len, struct adpcm_state *state);

static void
adpcm_decoder(char *indata, short *outdata, int len, struct adpcm_state *state);



PillBigError
pillbig_audio_adpcm_decode(FILE *input, FILE *output, PillBigAudioParameters *parameters)
{
	pillbig_error_clear();
	SET_RETURN_ERROR_IF_FAIL(input != NULL, PillBigError_InvalidStream);
	SET_RETURN_ERROR_IF_FAIL(output != NULL, PillBigError_InvalidStream);
	SET_RETURN_ERROR_IF_FAIL(parameters != NULL, PillBigError_UnknownError);

	int remaining_bytes, remaining_shorts;
	int read_bytes, written_bytes;
	char  *input_buffer,  *input_buffer_ptr;
	short *output_buffer, *output_buffer_ptr;
	struct adpcm_state status;

	input_buffer  = (char *) calloc(parameters->samples_count, sizeof(char));
	SET_ERROR_IF_FAIL(input_buffer != NULL, PillBigError_SystemError);
	output_buffer = (short *)calloc(parameters->samples_count, sizeof(short));
	SET_ERROR_IF_FAIL(output_buffer != NULL, PillBigError_SystemError);

	if (pillbig_no_error())
	{
		remaining_bytes = parameters->samples_count / 2;
 		input_buffer_ptr = input_buffer;
		while (!feof(input) && remaining_bytes > 0 && pillbig_no_error())
		{
			read_bytes = fread(input_buffer_ptr, sizeof(char), remaining_bytes, input);
			SET_ERROR_IF_FAIL(read_bytes > 0, PillBigError_SystemError);
			input_buffer_ptr += read_bytes;
			remaining_bytes -= read_bytes;
		}

		if (pillbig_no_error())
		{
			adpcm_decoder(input_buffer, output_buffer, parameters->samples_count, &status);

			remaining_shorts = parameters->samples_count;
			output_buffer_ptr = output_buffer;
			while (remaining_shorts > 0 && pillbig_no_error())
			{
				written_bytes = fwrite(output_buffer_ptr, sizeof(short), remaining_shorts, output);
				SET_ERROR_IF_FAIL(written_bytes > 0, PillBigError_SystemError);
				output_buffer_ptr += written_bytes;
				remaining_shorts -= written_bytes;
			}
		}
	}

	if (input_buffer != NULL)
	{
		free(input_buffer);
	}
	if (output_buffer != NULL)
	{
		free(output_buffer);
	}

	return pillbig_error_get();
}

static void
adpcm_coder(short *indata, char *outdata, int len, struct adpcm_state *state)
{
	short *inp;			/* Input buffer pointer */
	signed char *outp;	/* output buffer pointer */
	int val;			/* Current input sample value */
	int sign;			/* Current adpcm sign bit */
	int delta;			/* Current adpcm output value */
	int diff;			/* Difference between val and valprev */
	int step;			/* Stepsize */
	int valpred;		/* Predicted output value */
	int vpdiff;			/* Current change to valpred */
	int index;			/* Current step change index */
	int outputbuffer;	/* place to keep previous 4-bit value */
	int bufferstep;		/* toggle between outputbuffer/output */

	outp = (signed char *)outdata;
	inp = indata;

	valpred = state->valprev;
	index = state->index;
	step = stepsizeTable[index];

	bufferstep = 1;

	while (len-- > 0)
	{
		val = *inp++;

		/* Step 1 - compute difference with previous value */
		diff = val - valpred;
		sign = (diff < 0) ? 8 : 0;
		if (sign)
		{
			diff = -diff;
		}

		/* Step 2 - Divide and clamp */
		/* Note:
		** This code *approximately* computes:
		**	delta = diff*4/step;
		**	vpdiff = (delta+0.5)*step/4;
		** but in shift step bits are dropped. The net result of this is
		** that even if you have fast mul/div hardware you cannot put it to
		** good use since the fixup would be too expensive.
		*/
		delta = 0;
		vpdiff = (step >> 3);

		if (diff >= step)
		{
			delta = 4;
			diff -= step;
			vpdiff += step;
		}

		step >>= 1;
		if (diff >= step)
		{
			delta |= 2;
			diff -= step;
			vpdiff += step;
		}

		step >>= 1;
		if (diff >= step)
		{
			delta |= 1;
			vpdiff += step;
		}

		/* Step 3 - Update previous value */
		if (sign)
		{
			valpred -= vpdiff;
		}
		else
		{
			valpred += vpdiff;
		}

		/* Step 4 - Clamp previous value to 16 bits */
		if (valpred > 32767)
		{
			valpred = 32767;
		}
		else if (valpred < -32768)
		{
			valpred = -32768;
		}

		/* Step 5 - Assemble value, update index and step values */
		delta |= sign;

		index += indexTable[delta];
		if (index < 0)
		{
			index = 0;
		}
		else if (index > 88)
		{
			index = 88;
		}
		step = stepsizeTable[index];

		/* Step 6 - Output value */
		if (bufferstep)
		{
			outputbuffer = (delta << 4) & 0xf0;
		}
		else
		{
			*outp++ = (delta & 0x0f) | outputbuffer;
		}

		bufferstep = !bufferstep;
	}

	/* Output last step, if needed */
	if (!bufferstep)
	{
		*outp++ = outputbuffer;
	}

	state->valprev = valpred;
	state->index = index;
}

static void
adpcm_decoder(char *indata, short *outdata, int len, struct adpcm_state *state)
{
	signed char *inp;    /* Input buffer pointer */
	short *outp;         /* output buffer pointer */
	int sign;            /* Current adpcm sign bit */
	int delta;           /* Current adpcm output value */
	int step;            /* Stepsize */
	int valpred;         /* Predicted value */
	int vpdiff;          /* Current change to valpred */
	int index;           /* Current step change index */
	int inputbuffer;     /* place to keep next 4-bit value */
	int bufferstep;      /* toggle between inputbuffer/input */

	outp = outdata;
	inp = (signed char *)indata;

	valpred = state->valprev;
	index = state->index;
	step = stepsizeTable[index];

	bufferstep = 0;

	while (len-- > 0)
	{
		/* Step 1 - get the delta value */
		if (bufferstep)
		{
			delta = inputbuffer & 0xf;
		}
		else
		{
			inputbuffer = *inp++;
			delta = (inputbuffer >> 4) & 0x0f;
		}

		bufferstep = !bufferstep;

		/* Step 2 - Find new index value (for later) */
		index += indexTable[delta];
		if (index < 0)
		{
			index = 0;
		}
		else if (index > 88)
		{
			index = 88;
		}

		/* Step 3 - Separate sign and magnitude */
		sign = delta & 8;
		delta = delta & 7;

		/* Step 4 - Compute difference and new predicted value */
		/*
		** Computes 'vpdiff = (delta+0.5)*step/4', but see comment
		** in adpcm_coder.
		*/
		vpdiff = step >> 3;
		if (delta & 4)
		{
			vpdiff += step;
		}
		if (delta & 2)
		{
			vpdiff += step >> 1;
		}
		if (delta & 1)
		{
			vpdiff += step >> 2;
		}

		if (sign)
		{
			valpred -= vpdiff;
		}
		else
		{
			valpred += vpdiff;
		}

		/* Step 5 - clamp output value */
		if (valpred > 32767)
		{
			valpred = 32767;
		}
		else if (valpred < -32768)
		{
			valpred = -32768;
		}

		/* Step 6 - Update step value */
		step = stepsizeTable[index];

		/* Step 7 - Output value */
		*outp++ = valpred;
	}

	state->valprev = valpred;
	state->index = index;
}
