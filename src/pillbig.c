#include <pillbig/pillbig.h>
#include <stdlib.h>

int
main (int argc, char **argv)
{
	PillBig pillbig = pillbig_open_from_filename("PILL.BIG-PC");
	char buffer[16];

	int i;
	for (i = 570; i < 600; i++)
	{
		sprintf(buffer, "file%04d.wav", i);
		pillbig_audio_extract_to_filename(pillbig, i, buffer, PillBigAudioFormat_WAVE);
	}
	pillbig_close(pillbig);

	return 0;
}
