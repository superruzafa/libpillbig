#include <pillbig/pillbig.h>

int
main (int argc, char **argv)
{
	PillBig pillbig = NULL;

	pillbig = pillbig_open(NULL);

	const PillBigFileEntry *entry = pillbig_get_entry(pillbig, 0);

	pillbig_close(pillbig);

	return 0;
}
