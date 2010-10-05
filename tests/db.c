/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Unit tests for db module.
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

PillBigDB db;
PillBig pillbig;

static void
setup()
{
	db = pillbig_db_open("pillbig.xml");
	fail_unless(db != NULL);
	fail_unless(pillbig_error_get() == PillBigError_Success);
}

static void
teardown()
{
	pillbig_db_close(db);
}



START_TEST(get_files_count)
{
	int files_count= pillbig_db_get_files_count(db);
	fail_unless(files_count >= 0);
}
END_TEST

START_TEST(get_entry_index_by_position)
{
	int first_entry_index, last_entry_index;

	first_entry_index = pillbig_db_get_entry_index_by_position(db, 0);
	last_entry_index  = pillbig_db_get_entry_index_by_position(db,
			pillbig_db_get_files_count(db) - 1);

	if (pillbig_db_get_files_count(db) > 0)
	{
		fail_if(first_entry_index == -1);
		fail_if(last_entry_index == -1);
	}

}
END_TEST

START_TEST(get_first_entry)
{
	const PillBigDBEntry *dbentry;

	int first_entry_index = pillbig_db_get_entry_index_by_position(db, 0);
	if (pillbig_db_get_files_count(db) > 0)
	{
		fail_if(first_entry_index == -1);
	}

	dbentry = pillbig_db_get_entry(db, first_entry_index);
	fail_unless(dbentry != NULL);
}
END_TEST

START_TEST(get_last_entry)
{
	const PillBigDBEntry *dbentry;

	int last_entry_index = pillbig_db_get_entry_index_by_position(db,
			pillbig_db_get_files_count(db) - 1);
	if (pillbig_db_get_files_count(db) > 0)
	{
		fail_if(last_entry_index == -1);
	}

	dbentry = pillbig_db_get_entry(db, last_entry_index);
	fail_unless(dbentry != NULL);
}
END_TEST

START_TEST(get_entry_index_by_hash)
{
	const PillBigDBEntry *dbentry;
	int first_entry_index = pillbig_db_get_entry_index_by_position(db, 0);
	if (pillbig_db_get_files_count(db) > 0)
	{
		fail_if(first_entry_index == -1);
	}

	dbentry = pillbig_db_get_entry(db, first_entry_index);
	fail_unless(dbentry != NULL);
	fail_unless(first_entry_index == pillbig_db_get_entry_index_by_hash(db, dbentry->hash));
}
END_TEST

START_TEST(validate_database)
{
	int result = system("xmllint --noout --dtdvalid pillbig.dtd pillbig.xml 2>/dev/null");
	fail_unless(result == 0);
}
END_TEST

START_TEST(set_database)
{
	PillBig pillbig = pillbig_open_from_filename("pill.big");
	fail_unless(pillbig != NULL);

	pillbig_set_db(pillbig, db);
	fail_unless(pillbig_error_get() == PillBigError_Success);
	fail_unless(db == pillbig_get_db(pillbig));

	pillbig_close(pillbig);
}
END_TEST


Suite *
pillbig_db_test_get_suite(void)
{
	Suite *suite = suite_create("Database");

	TCase *test_case = tcase_create("Queries");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, get_entry_index_by_position);
	tcase_add_test(test_case, get_files_count);
	tcase_add_test(test_case, get_first_entry);
	tcase_add_test(test_case, get_last_entry);
	tcase_add_test(test_case, get_entry_index_by_hash);
	suite_add_tcase(suite, test_case);

	test_case = tcase_create("Integration");
	tcase_add_checked_fixture(test_case, setup, teardown);
	tcase_add_test(test_case, set_database);
	suite_add_tcase(suite, test_case);

	test_case = tcase_create("Validation");
	tcase_add_test(test_case, validate_database);
	suite_add_tcase(suite, test_case);

	return suite;
}
