/*
 *  libpillbig
 *  A library to deal with Blood Omen: Legacy of Kain pill.big files.
 */

/**
 *  @file
 *  @brief
 *  	API for the pill.big file information database. Implementation.
 *
 *  @author  Alfonso Ruzafa <superruzafa@gmail.com>
 *  @version $Id$
 */

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <pillbig/pillbig.h>
#include "pillbig_internal.h"



struct _PillBigDB
{
	xmlDocPtr           doc;
	xmlXPathContextPtr  xpathContext;
	int                 files_count;
	PillBigDBEntry    **entries;
	PillBigFileType     filetype;
};

static char *
pillbig_db_get_xpath_query_as_string(PillBigDB db, const char *query);

static int
pillbig_db_get_xpath_query_as_int(PillBigDB db, const char *query);

static PillBigFileType
pillbig_db_get_xpath_query_as_filetype(PillBigDB db, const char *query);

static PillBigDBAudioEntry *
pillbig_db_get_audio_entry(PillBigDB db, int index);

static PillBigDBAudioSpeech *
pillbig_db_get_audio_speeches(PillBigDB db, int index, int *speeches_count);


PillBigDB
pillbig_db_open(const char *filename)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(filename != "", PillBigError_InvalidFilename, NULL);

	PillBigDB db = NULL;

	xmlDocPtr doc = xmlParseFile(filename);
	SET_ERROR_RETURN_VALUE_IF_FAIL(doc != NULL, PillBigError_UnknownError, NULL);

	xmlXPathContextPtr xpathContext = xmlXPathNewContext(doc);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathContext != NULL, PillBigError_UnknownError, NULL);

	db = (PillBigDB)malloc(sizeof(struct _PillBigDB));
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_SystemError, NULL);

	memset(db, 0, sizeof(struct _PillBigDB));

	db->doc = doc;
	db->xpathContext = xpathContext;
	/*
	 * Collateral effect: db->files_count gets updated
	 * when calling pillbig_db_get_files_count().
	 */
	db->files_count = -1;
	pillbig_db_get_files_count(db);
	SET_ERROR_IF_FAIL(db->files_count >= 0, PillBigError_UnknownError);

	db->entries = (PillBigDBEntry **)calloc(db->files_count, sizeof(PillBigDBEntry *));
	SET_ERROR_IF_FAIL(db->entries != NULL, PillBigError_SystemError);

	if (pillbig_no_error())
	{
		memset(db->entries, 0, sizeof(PillBigDBEntry *) * db->files_count);
	}
	else
	{
		pillbig_db_close(db);
		db = NULL;
	}

	return db;
}

int
pillbig_db_get_files_count(PillBigDB db)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_UnknownError, -1);

	if (db->files_count == -1)
	{
		xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression("/pillbig/file", db->xpathContext);
		SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, -1);
		db->files_count = xpathObject->nodesetval->nodeNr;
		free(xpathObject);
	}

	return db->files_count;
}

const PillBigDBEntry *
pillbig_db_get_entry(PillBigDB db, int index)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_UnknownError, NULL);
	SET_ERROR_RETURN_VALUE_IF_FAIL(0 <= index && index < db->files_count,
		PillBigError_UnknownError, NULL);

	char query[64];
	char *hash;

	if (db->entries[index] == NULL)
	{
		db->entries[index] = (PillBigDBEntry *)malloc(sizeof(PillBigDBEntry));
		SET_ERROR_RETURN_VALUE_IF_FAIL(db->entries[index] != NULL, PillBigError_SystemError, NULL);

		sprintf(query, "/pillbig/file[@index='%d']/filename", index);
		db->entries[index]->filename = pillbig_db_get_xpath_query_as_string(db, query);

		sprintf(query, "/pillbig/file[@index='%d']/hash", index);
		hash = pillbig_db_get_xpath_query_as_string(db, query);
		if (hash != NULL)
		{
			sscanf(hash, "%X", &db->entries[index]->hash);
			free(hash);
		}

		sprintf(query, "/pillbig/file[@index='%d']/offset", index);
		db->entries[index]->offset = pillbig_db_get_xpath_query_as_int(db, query);

		sprintf(query, "/pillbig/file[@index='%d']/size", index);
		db->entries[index]->size = pillbig_db_get_xpath_query_as_int(db, query);

		sprintf(query, "/pillbig/file[@index='%d']/@type", index);
		db->entries[index]->filetype = pillbig_db_get_xpath_query_as_filetype(db, query);

		switch (db->entries[index]->filetype)
		{
		case PillBigFileType_Audio:
			db->entries[index]->audio = pillbig_db_get_audio_entry(db, index);
			break;
		}
	}

	return db->entries[index];
}

int
pillbig_db_get_entry_index_by_hash(PillBigDB db, PillBigFileHash hash)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_UnknownError, -1);

	char query[64];
	sprintf(query, "/pillbig/file[hash='%X']/@index", hash);

	return pillbig_db_get_xpath_query_as_int(db, query);
}

int
pillbig_db_has_entry(PillBigDB db, int index)
{
	pillbig_error_clear();
	return pillbig_db_get_entry(db, index) != NULL;
}

int
pillbig_db_get_entry_index_by_position(PillBigDB db, int position)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_UnknownError, -1);
	SET_ERROR_RETURN_VALUE_IF_FAIL(position >= 0, PillBigError_UnknownError, -1);

	char query[96];
	sprintf(query, "/pillbig/file[count(preceding-sibling::file) = %d]/@index", position);

	return pillbig_db_get_xpath_query_as_int(db, query);
}

void
pillbig_db_close(PillBigDB db)
{
	pillbig_error_clear();
	SET_ERROR_RETURN_IF_FAIL(db != NULL, PillBigError_UnknownError);

	PillBigDBEntry *entry;

	int i = 0;

	if (db->entries != NULL)
	{
		while (i < db->files_count)
		{
			entry = db->entries[i];
			if (entry != NULL)
			{
				if (entry->filename != NULL) free(entry->filename);

				switch (entry->filetype)
				{
				case PillBigFileType_Audio:
					if (entry->audio->character != NULL) free(entry->audio->character);
					if (entry->audio->speeches != NULL)
					{
						int i;
						for (i = 0; i < entry->audio->speeches_count; i++)
						{
							PillBigDBAudioSpeech *speech = &entry->audio->speeches[i];
							if (speech->language != NULL) free(speech->language);
							if (speech->speech   != NULL) free(speech->speech);
						}
						free(entry->audio->speeches);
					}
					break;
				}

				free(entry);
			}
			i++;
		}
		free(db->entries);
	}

	xmlFreeDoc(db->doc);
	xmlXPathFreeContext(db->xpathContext);
	free(db);
}

static int
pillbig_db_get_xpath_query_as_int(PillBigDB db, const char *query)
{
	int value;

	xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(query, db->xpathContext);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, -1);
	value = xmlXPathCastNodeSetToNumber(xpathObject->nodesetval);
	free(xpathObject);

	return value;
}

static char *
pillbig_db_get_xpath_query_as_string(PillBigDB db, const char *query)
{
	char *value = NULL;

	xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(query, db->xpathContext);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, NULL);
	value = xmlXPathCastNodeSetToString(xpathObject->nodesetval);
	value = (strlen(value) > 0) ? strdup(value) : NULL;
	free(xpathObject);

	return value;
}

static PillBigFileType
pillbig_db_get_xpath_query_as_filetype(PillBigDB db, const char *query)
{
	PillBigFileType filetype = PillBigFileType_Unknown;
	char *value;

	xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(query, db->xpathContext);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, PillBigFileType_Unknown);
	value = xmlXPathCastNodeSetToString(xpathObject->nodesetval);

	if (strlen(value) > 0)
	{
		     if (strcmp(value, "audio") == 0)   filetype = PillBigFileType_Audio;
		else if (strcmp(value, "bitmap") == 0)  filetype = PillBigFileType_Bitmap;
		else if (strcmp(value, "map") == 0)     filetype = PillBigFileType_Map;
		else if (strcmp(value, "sprite") == 0)  filetype = PillBigFileType_Sprite;
		else if (strcmp(value, "tilemap") == 0) filetype = PillBigFileType_Tilemap;
	}
	free(xpathObject);

	return filetype;
}

static PillBigDBAudioEntry *
pillbig_db_get_audio_entry(PillBigDB db, int index)
{
	PillBigDBAudioEntry *entry;
	char query[128];

	entry = (PillBigDBAudioEntry *)malloc(sizeof(PillBigDBAudioEntry));
	SET_ERROR_RETURN_VALUE_IF_FAIL(entry != NULL, PillBigError_SystemError, NULL);
	memset(entry, 0, sizeof(PillBigDBAudioEntry));

	sprintf(query, "/pillbig/file[@index='%d']/audio/@character", index);
	entry->character = pillbig_db_get_xpath_query_as_string(db, query);
	entry->speeches = pillbig_db_get_audio_speeches(db, index, &entry->speeches_count);

	return entry;
}

static PillBigDBAudioSpeech *
pillbig_db_get_audio_speeches(PillBigDB db, int index, int *speeches_count)
{
	PillBigDBAudioSpeech *speeches = NULL;
	*speeches_count = 0;
	char query[128];

	sprintf(query, "/pillbig/file[@index='%d']/audio/speech", index);
	xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(query, db->xpathContext);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, NULL);
	*speeches_count = xpathObject->nodesetval->nodeNr;

	speeches = (PillBigDBAudioSpeech *)calloc(*speeches_count, sizeof(PillBigDBAudioSpeech));
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, NULL);
	memset(speeches, 0, sizeof(*speeches_count * sizeof(PillBigDBAudioSpeech)));

	int i;
	for (i = 0; i < *speeches_count; i++)
	{
		db->xpathContext->node = xpathObject->nodesetval->nodeTab[i];
		speeches[i].language = pillbig_db_get_xpath_query_as_string(db, "@xml:lang");
		speeches[i].speech   = pillbig_db_get_xpath_query_as_string(db, ".");
	}
	xmlFree(xpathObject);

	return speeches;
}
