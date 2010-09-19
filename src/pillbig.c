#include <pillbig/pillbig.h>

int
main (int argc, char **argv)
{
	PillBig pillbig = NULL;

	pillbig = pillbig_open(NULL);
	pillbig_close(pillbig);

	return 0;
}
