/* Part of the CBSD Project */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "sqlite3.h"

#include "sqlcli.h"

char *
nm(void)
{
	return "sqlcli";
}

void usage() {
    printf("Tools for execute SQLite query from CLI\n");
    printf("%s: format: %s <dbfile> <query>\n",nm(),nm());
}


int
sqlCB(void *none, int rows, char **rowV, char **rowN)
{
	int	 i;
	int	*cnt = (int *)none;
	char *delim;
	int printheader=0;
	char *cp;

    if ((cp = getenv("sqldelimer")) == NULL) 
	delim=DEFSQLDELIMER;
    else
	delim=cp;

//	delim=lookupvar("sqldelimer");

	if ( delim == NULL ) delim=DEFSQLDELIMER;

	if (printheader) {
		if (!(*cnt)) {
		    for (i = 0; i < rows; i++)
			if (i<rows-1)
			    printf("%s%s",rowN[i],delim);
			else
			    printf("%s\n",rowN[i]);
		}
	}
	(*cnt)++;
	for (i = 0; i < rows; i++)
		if (i<rows-1)
		    printf("%s%s",rowV[i],delim);
		else
		    printf("%s\n", rowV[i]);
	return 0;
}

int
main(int argc, char **argv)
{
	sqlite3	*db;
	int	 res;
	int	 i;
	char	*query;
	char	*tmp;
	char	*err;

	if (argc<3) {
		usage();
		return 0;
	}

	if (SQLITE_OK != (res = sqlite3_open(argv[1], &db))) {
		printf("%s: Can't open database file: %s\n", nm(), argv[1]);
		return 1;
	}

	res = 0;
	for (i = 2; i < argc; i++)
		res += strlen(argv[i]) + 1;
	if (res) {
		query = (char *)sqlite3_malloc(res);
		tmp = query;
		for (i = 2; i < argc; i++) {
			strcpy(tmp, argv[i]);
			tmp += strlen(tmp);
			*tmp = ' ';
			tmp++;
		}
		tmp[-1] = 0;
		err = 0;
		i = 0;
		sqlite3_exec(db, query, sqlCB, (void *)&i, &err);
		if (err) {
			printf("%s: sqlite_error: %s\n", nm(), err);
			sqlite3_free(err);
			sqlite3_free(query);
			sqlite3_close(db);
			return 1;
		}
		sqlite3_free(query);
	}
	sqlite3_close(db);
	return 0;
}
