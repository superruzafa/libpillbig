/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Unit tests for audio module.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 */

#include <check.h>
#include <pillbig/pillbig.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

static PillBig pillbig;
static FILE *pillbig_file;

static void
setup()
{
	pillbig_file = fopen(TEST_PILLBIG_FILENAME, "r+b");
	fail_unless(pillbig_file != NULL);

	pillbig = pillbig_open(pillbig_file);
	fail_unless(pillbig != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}

static void
teardown()
{
	pillbig_close(pillbig);
	fclose(pillbig_file);
}



START_TEST(get_format)
{
	PillBigPlatform platform = pillbig_get_platform(pillbig);

	switch (platform)
	{
	case PillBigPlatform_PC:
		fail_unless(pillbig_audio_get_format(pillbig,    0) == PillBigAudioFormat_Unknown);
		fail_unless(pillbig_audio_get_format(pillbig,   16) == PillBigAudioFormat_ADPCM);
		fail_unless(pillbig_audio_get_format(pillbig,  315) == PillBigAudioFormat_VAG);
		fail_unless(pillbig_audio_get_format(pillbig, 2330) == PillBigAudioFormat_WAVE);
		break;
	case PillBigPlatform_PSX:
	default:
		fail("Invalid pill.big file");
		break;
	}
}
END_TEST




Suite *
pillbig_audio_test_get_suite(void)
{
	Suite *suite = suite_create("Audio");

	TCase *test_case = tcase_create("Queries");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, get_format);
	suite_add_tcase(suite, test_case);

	return suite;
}
