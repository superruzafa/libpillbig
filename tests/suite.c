/**
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version SVN $Id$
 *
 *  @file
 *  @brief
 *  	Unit tests suite file.
 */

#include <check.h>
#include <pillbig/pillbig.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef HAVE_CONFIG_H
#	include <config.h>
#endif

#include "file.c"

int
main(int argc, char **argv)
{
	int tests_failed = 0;

	Suite *suite = pillbig_file_test_get_suite();
	SRunner *runner = srunner_create(suite);
	srunner_run_all(runner, CK_VERBOSE);

	tests_failed += srunner_ntests_failed(runner);
	srunner_free(runner);

	return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
