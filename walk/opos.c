#include "mheads.h"
#include "lheads.h"
#include "opos.h"

NEOERR* pos_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "city");
	NEOERR *err;

    if (!cgi || !cgi->hdf || !db) return nerr_raise(NERR_ASSERT, "paramter null");

    MDB_QUERY_RAW(db, "city", _COL_CITY, "geopos ?- point '(0,0)' LIMIT 1000", NULL);
    return nerr_pass(mdb_set_rows(cgi->hdf, db, _COL_CITY,
                                  PRE_OUTPUT".cts", NULL, MDB_FLAG_Z));
}

NEOERR* pos_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
	STRING str; string_init(&str);
    mdb_conn *db = hash_lookup(dbh, "city");
	NEOERR *err;

    if (!cgi || !cgi->hdf || !db) return nerr_raise(NERR_ASSERT, "paramter null");

    int id = hdf_get_int_value(cgi->hdf, PRE_QUERY".id", 0);
    
    err = mdb_build_upcol(hdf_get_obj(cgi->hdf, PRE_QUERY),
                          hdf_get_obj(g_cfg, "Db.UpdateCol.city"), &str);
	if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "UPDATE city SET %s WHERE id=%d;", NULL, str.buf, id);
    
    string_clear(&str);

    return STATUS_OK;
}

NEOERR* city_ip_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "city");
    char *ip;

    if (!cgi || !cgi->hdf || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

    ip = hdf_get_value(cgi->hdf, PRE_QUERY".ip", NULL);
    if (!ip) ip = hdf_get_value(cgi->hdf, PRE_REQ_IP, "0.0.0.0");

    hdf_set_value(evt->hdfsnd, "ip", ip);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_CITY_BY_IP, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    hdf_set_attr(cgi->hdf, PRE_OUTPUT".citys", "type", "array");

    return STATUS_OK;
}

NEOERR* city_s_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "city");
    char *s;

    if (!cgi || !cgi->hdf || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

    HDF_GET_STR(cgi->hdf, PRE_QUERY".c", s);

    hdf_set_value(evt->hdfsnd, "c", s);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_CITY_BY_S, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    hdf_set_attr(cgi->hdf, PRE_OUTPUT".citys", "type", "array");

    return STATUS_OK;
}

NEOERR* city_id_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "city");
    char *id;

    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".id", id);

    hdf_set_value(evt->hdfsnd, "id", id);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_CITY_BY_ID, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    hdf_set_attr(cgi->hdf, PRE_OUTPUT".citys", "type", "array");

    return STATUS_OK;
}
