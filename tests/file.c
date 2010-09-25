/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Unit tests for file module.
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

#ifndef TEST_PILLBIG_FILENAME
#	define TEST_PILLBIG_FILENAME "pill.big"
#endif

static PillBig pillbig;
static FILE *pillbig_file;

void
setup()
{
	pillbig_file = fopen(TEST_PILLBIG_FILENAME, "r+b");
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

START_TEST(set_replace_mode_fail)
{
	pillbig_set_replace_mode(pillbig, 1000);
	fail_unless(pillbig_error_get() == PillBigError_InvalidReplaceMode);
	fail_unless(pillbig_get_replace_mode(pillbig) == PillBigReplaceMode_Strict);
}
END_TEST



START_TEST(get_hash)
{
	char *filenames[] =
	{
		"GAME\\ALL.VAG", "GAME\\ANTITOXN.SDR", "GAME\\ATRACTNS.TIM",
		"GAME\\AV0001.FAG", "GAME\\BATS.VAG", "GAME\\BONE2DIS.SHD",
		"GAME\\CHAR0000.SHA", "GAME\\CHAR0178.SHA", "GAME\\CONCEPT.HTM",
		"GAME\\GRP00000.CTM", "GAME\\M0000000.CMP", "GAME\\MAPGLOW.SDT"
	};

	PillBigFileHash hashes[] =
	{
		1312839251, 220166696, 270101794, 1533899019, 1348087145,
		169156399, 2087066718, 2070355030, 302386960, 1902909488,
		135139885, 369562388, -1
	};

	int i = 0;
	while (hashes[i] != -1)
	{
		fail_unless(pillbig_get_hash_by_filename(filenames[i]) == hashes[i]);
		i++;
	}
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
	switch (pillbig_get_platform(pillbig))
	{
		case PillBigPlatform_PC:
			fail_unless(pillbig_get_files_count(pillbig) == FILES_COUNT_PC);
			break;
		case PillBigPlatform_PSX:
			fail_unless(pillbig_get_files_count(pillbig) == FILES_COUNT_PSX);
			break;
		default:
			fail("Unsupported format.");
			break;
	}
	fail_unless(pillbig_error_get() == PillBigError_Success);
}
END_TEST

START_TEST(get_first_entry)
{
	const PillBigFileEntry *entry = pillbig_get_entry(pillbig, 0);
	fail_unless(entry != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}
END_TEST

START_TEST(get_last_entry)
{
	const PillBigFileEntry *entry = pillbig_get_entry(pillbig, pillbig_get_files_count(pillbig) - 1);
	fail_unless(entry != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}
END_TEST

START_TEST(get_unexistent_entry)
{
	const PillBigFileEntry *entry = pillbig_get_entry(pillbig, pillbig_get_files_count(pillbig) + 1000);
	fail_unless(entry == NULL);
	fail_unless(pillbig_error_get() == PillBigError_FileIndexOutOfRange);
}
END_TEST

START_TEST(get_index_by_hash)
{
	const PillBigFileEntry *entry;

	int i;
	for (i = 0; i < pillbig_get_files_count(pillbig); i++)
	{
		entry = pillbig_get_entry(pillbig, i);
		fail_unless(pillbig_get_entry_index_by_hash(pillbig, entry->hash) == i);
	}
}
END_TEST



START_TEST(open_from_filename)
{
	PillBig pillbig = pillbig_open_from_filename(TEST_PILLBIG_FILENAME);
	fail_unless(pillbig != NULL);
	pillbig_close(pillbig);
}
END_TEST

START_TEST(open_from_filename_fail)
{
	PillBig pillbig = pillbig_open_from_filename(__FILE__);
	fail_unless(pillbig == NULL);
	fail_unless(pillbig_error_get() == PillBigError_UnsupportedFormat);
}
END_TEST

START_TEST(file_extract)
{
	FILE *file = fopen("test", "wb");
	fail_unless(file != NULL);
	pillbig_file_extract(pillbig, 0, file);
	fail_unless(pillbig_error_get() == PillBigError_Success);

	const PillBigFileEntry *entry = pillbig_get_entry(pillbig, 0);
	int result = fseek(file, 0, SEEK_END);
	fail_unless(result == 0);
	result = ftell(file);
	fclose(file);
	fail_unless(result == entry->size);
	unlink("test");
}
END_TEST

START_TEST(file_extract_to_filename)
{
	pillbig_file_extract_to_filename(pillbig, 0, "test");
	fail_unless(pillbig_error_get() == PillBigError_Success);

	const PillBigFileEntry *entry = pillbig_get_entry(pillbig, 0);
	FILE *file = fopen("test", "rb");
	fail_unless(file != NULL);
	int result = fseek(file, 0, SEEK_END);
	fail_unless(result == 0);
	result = ftell(file);
	fclose(file);
	fail_unless(result == entry->size);
	unlink("test");
}
END_TEST



Suite *
pillbig_file_test_get_suite(void)
{
	Suite *suite = suite_create("File");

	TCase *test_case = tcase_create("Configuration");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, set_replace_mode);
	tcase_add_test(test_case, set_replace_mode_fail);
	suite_add_tcase(suite, test_case);

	test_case = tcase_create("Conversion");
	tcase_add_test(test_case, get_hash);
	suite_add_tcase(suite, test_case);

	test_case = tcase_create("Queries");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, get_platform);
	tcase_add_test(test_case, get_files_count);
	tcase_add_test(test_case, get_first_entry);
	tcase_add_test(test_case, get_last_entry);
	tcase_add_test(test_case, get_unexistent_entry);
	tcase_add_test(test_case, get_index_by_hash);
	suite_add_tcase(suite, test_case);

	test_case = tcase_create("IO");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, open_from_filename);
	tcase_add_test(test_case, open_from_filename_fail);
	tcase_add_test(test_case, file_extract);
	tcase_add_test(test_case, file_extract_to_filename);
	suite_add_tcase(suite, test_case);

	return suite;
}
