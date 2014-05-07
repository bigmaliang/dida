#ifndef __LDB_H__
#define __LDB_H__
#include "mheads.h"

__BEGIN_DECLS

NEOERR* ldb_init(HASH **dbh);
void ldb_destroy(HASH *dbh);

__END_DECLS
#endif    /* __LDB_H__ */
