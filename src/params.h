#ifndef __PILLBIG_CMD_PARAMS__
#define __PILLBIG_CMD_PARAMS__

#include <pillbig/pillbig.h>

typedef enum
{
	PillBigCMDMode_Help,
	PillBigCMDMode_Version,
	PillBigCMDMode_Info,
	PillBigCMDMode_Extract,
	PillBigCMDMode_Replace,
	PillBigCMDMode_Hash,
}
PillBigCMDMode;

typedef enum
{
	PillBigCMDFormat_Auto,

	PillBigCMDFormat_PCM,
	PillBigCMDFormat_WAV,

	PillBigCMDFormat_BMP,
	PillBigCMDFormat_PNG,
}
PillBigCMDFormat;

typedef enum
{
	PillBigCMDInfo_All     = 0xffff,
	PillBigCMDInfo_PillBig = 1 << 0,
	PillBigCMDInfo_Index   = 1 << 1,
	PillBigCMDInfo_Hash    = 1 << 2,
	PillBigCMDInfo_Offset  = 1 << 3,
	PillBigCMDInfo_Size    = 1 << 4,
	PillBigCMDInfo_Name    = 1 << 5,
}
PillBigCMDInfo;

typedef struct
{
	char                 *command;
	PillBigCMDMode        mode;
	char                 *pillbig;
	int                   use_database;
	char                 *database;
	PillBigReplaceMode    replace_mode;
	PillBigCMDFormat      audio_format;
	PillBigCMDFormat      bitmap_format;
	PillBigCMDInfo        show_info;
	char                 *pattern;
	int                   files_count;
	int                  *indices;
}
PillBigCMDParams;

PillBigCMDParams *
pillbig_cmd_params_decode(int argc, char **argv);

void
pillbig_cmd_params_free(PillBigCMDParams *params);

#endif
