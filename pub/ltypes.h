#ifndef __LTYPES_H__
#define __LTYPES_H__

#include "mheads.h"

__BEGIN_DECLS

typedef struct _session {
    int reqtype;
    time_t tm_cache_browser;

    char *mname;
    HDF  *province;
    HDF  *city;
    int   browser;
    float bversion;

    char *dataer;
    char *render;

    void *data;
} session_t;

session_t* session_default();
NEOERR* session_init(CGI *cgi, HASH *dbh, session_t **ses);
void session_destroy(session_t **ses);

__END_DECLS
#endif    /* __LTYPES_H__ */
