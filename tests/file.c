/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Unit tests for file module.
 */

#include <check.h>
#include <pillbig/pillbig.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#ifndef TEST_PILLBIG_FILENAME
#	define TEST_PILLBIG_FILENAME "pill.big"
#endif

static PillBig pillbig;
static FILE *pillbig_file;

void
setup()
{
	pillbig_file = fopen(TEST_PILLBIG_FILENAME, "rb");
	fail_unless(pillbig_file != NULL);

	pillbig = pillbig_open(pillbig_file);
	fail_unless(pillbig != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}

void
teardown()
{
	pillbig_close(pillbig);
	fclose(pillbig_file);
}



START_TEST(set_replace_mode)
{
	PillBigReplaceMode mode;

	for (mode = PillBigReplaceMode_Strict; mode <= PillBigReplaceMode_AllowLargerFiles; mode++)
	{
		pillbig_set_replace_mode(pillbig, mode);
		fail_unless(pillbig_error_get() == PillBigError_Success);
		fail_unless(pillbig_get_replace_mode(pillbig) == mode);
	}
}
END_TEST

START_TEST(set_invalid_replace_mode)
{
	pillbig_set_replace_mode(pillbig, 1000);
	fail_unless(pillbig_error_get() == PillBigError_InvalidReplaceMode);
	fail_if(pillbig_get_replace_mode(pillbig) == 1000);
}
END_TEST



START_TEST(get_platform)
{
	PillBigPlatform platform = pillbig_get_platform(pillbig);

	fail_unless(platform == PillBigPlatform_PC ||
	            platform == PillBigPlatform_PSX);
}
END_TEST

START_TEST(get_files_count)
{
	fail_unless(pillbig_get_files_count(pillbig) == 2572);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}
END_TEST

START_TEST(get_first_entry)
{
	PillBigFileEntry *entry = pillbig_get_entry(pillbig, 0);
	fail_unless(entry != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}
END_TEST

START_TEST(get_last_entry)
{
	PillBigFileEntry *entry = pillbig_get_entry(pillbig, pillbig_get_files_count(pillbig) - 1);
	fail_unless(entry != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}
END_TEST

START_TEST(get_unexistent_entry)
{
	PillBigFileEntry *entry = pillbig_get_entry(pillbig, pillbig_get_files_count(pillbig) + 1000);
	fail_unless(entry == NULL);
	fail_unless(pillbig_error_get() == PillBigError_FileIndexOutOfRange);
}
END_TEST



Suite *
pillbig_file_test_get_suite(void)
{
	Suite *suite = suite_create("File");

	TCase *test_case = tcase_create("Configuration");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, set_replace_mode);
	tcase_add_test(test_case, set_invalid_replace_mode);

	suite_add_tcase(suite, test_case);

	test_case = tcase_create("Queries");
	tcase_add_checked_fixture(test_case, setup, teardown);

	tcase_add_test(test_case, get_platform);
	tcase_add_test(test_case, get_files_count);
	tcase_add_test(test_case, get_first_entry);
	tcase_add_test(test_case, get_last_entry);
	tcase_add_test(test_case, get_unexistent_entry);

	suite_add_tcase(suite, test_case);

	return suite;
}
