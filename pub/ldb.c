#include "mheads.h"
#include "lheads.h"

NEOERR* ldb_init(HASH **dbh)
{
    HDF *node;
    HASH *ldbh;
    NEOERR *err;

    node = hdf_get_obj(g_cfg, "Db.Dsn");
    if (!node) return nerr_raise(NERR_ASSERT, "Db.Dsn config not found");

    err = hash_init(&ldbh, hash_str_hash, hash_str_comp, hash_str_free);
    if (err != STATUS_OK) return nerr_pass(err);

    node = hdf_obj_child(node);
    mdb_conn *conn;
    while (node != NULL) {
        err = mdb_init(&conn, hdf_obj_value(node));
        if (err != STATUS_OK) return nerr_pass(err);
        hash_insert(ldbh, (void*)strdup(hdf_obj_name(node)), (void*)conn);

        node = hdf_obj_next(node);
    }

    *dbh = ldbh;
    return STATUS_OK;
}

void ldb_destroy(HASH *dbh)
{
    char *key = NULL;

    mdb_conn *conn = (mdb_conn*)hash_next(dbh, (void**)&key);

    while (conn != NULL) {
        mdb_destroy(conn);
        conn = hash_next(dbh, (void**)&key);
    }

    hash_destroy(&dbh);
}
