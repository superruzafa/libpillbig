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
#include "error_internal.h"



struct PillBigDB
{
	xmlDocPtr           doc;
	xmlXPathContextPtr  xpathContext;
	int                 files_count;
	PillBigDBEntry    **entries;
};

static char *
pillbig_db_get_xpath_query_as_string(PillBigDB db, char *query);

static int
pillbig_db_get_xpath_query_as_int(PillBigDB db, char *query);



PillBigDB
pillbig_db_open(char *filename)
{
	SET_ERROR_RETURN_VALUE_IF_FAIL(filename != "", PillBigError_InvalidFilename, NULL);

	PillBigDB db = NULL;

	xmlDocPtr doc = xmlParseFile(filename);
	SET_ERROR_RETURN_VALUE_IF_FAIL(doc != NULL, PillBigError_UnknownError, NULL);

	xmlXPathContextPtr xpathContext = xmlXPathNewContext(doc);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathContext != NULL, PillBigError_UnknownError, NULL);

	db = (PillBigDB)malloc(sizeof(struct PillBigDB));
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_SystemError, NULL);

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
	}

	return db->entries[index];
}

int
pillbig_db_get_entry_by_hash(PillBigDB db, PillBigFileHash hash)
{
	SET_ERROR_RETURN_VALUE_IF_FAIL(db != NULL, PillBigError_UnknownError, NULL);
	SET_ERROR_RETURN_VALUE_IF_FAIL(hash != NULL, PillBigError_UnknownError, NULL);

	char query[64];
	sprintf(query, "/pillbig/file[hash='%X']/@index", hash);

	return pillbig_db_get_xpath_query_as_int(db, query);
}

void
pillbig_db_close(PillBigDB db)
{
	SET_ERROR_RETURN_IF_FAIL(db != NULL, PillBigError_UnknownError);

	int i = 0;

	if (db->entries != NULL)
	{
		while (i < db->files_count)
		{
			if (db->entries[i] != NULL)
			{
				if (db->entries[i]->filename != NULL)
				{
					free(db->entries[i]->filename);
				}
				free(db->entries[i]);
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
pillbig_db_get_xpath_query_as_int(PillBigDB db, char *query)
{
	int value;

	xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(query, db->xpathContext);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, -1);
	value = xmlXPathCastNodeSetToNumber(xpathObject->nodesetval);
	free(xpathObject);

	return value;
}

static char *
pillbig_db_get_xpath_query_as_string(PillBigDB db, char *query)
{
	char *value = NULL;

	xmlXPathObjectPtr xpathObject = xmlXPathEvalExpression(query, db->xpathContext);
	SET_ERROR_RETURN_VALUE_IF_FAIL(xpathObject != NULL, PillBigError_SystemError, NULL);
	value = xmlXPathCastNodeSetToString(xpathObject->nodesetval);
	value = (strlen(value) > 0) ? strdup(value) : NULL;
	free(xpathObject);

	return value;
}
