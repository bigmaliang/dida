#ifndef __LTPL_H__
#define __LTPL_H__
#include "mheads.h"

__BEGIN_DECLS

void ltpl_prepare_rend(HDF *hdf, char *tpl);

void ltpl_destroy(HASH *tplh);
#ifdef __MACH__
int ltpl_config(struct dirent *ent);
#else
int ltpl_config(const struct dirent *ent);
#endif
NEOERR* ltpl_init(HASH **tplh, char *path);
NEOERR* ltpl_parse_file(HASH *dbh, HASH *evth,
                        void *lib, char *dir, char *name, HASH *outhash);
NEOERR* ltpl_parse_dir(char *dir, HASH *outhash);

NEOERR* ltpl_render(CGI *cgi, HASH *tplh, session_t *ses);

NEOERR* ltpl_render2file(CGI *cgi, char *render, char *fname);
NEOERR* ltpl_render2filef(CGI *cgi, char *render, char *fmt, ...)
                          ATTRIBUTE_PRINTF(3, 4);

__END_DECLS
#endif    /* __LTPL_H__ */
