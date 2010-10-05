/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	Unit tests suite file.
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

Suite *pillbig_file_test_get_suite();

int
main(int argc, char **argv)
{
	int tests_failed = 0;

	SRunner *runner;

	runner = srunner_create(pillbig_file_test_get_suite());
	srunner_add_suite(runner, pillbig_db_test_get_suite());

	srunner_run_all(runner, CK_VERBOSE);

	tests_failed += srunner_ntests_failed(runner);
	srunner_free(runner);

	return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
