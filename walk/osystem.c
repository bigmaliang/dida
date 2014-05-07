#include "mheads.h"
#include "lheads.h"
#include "osystem.h"

#define _COL_EMAP_PAGEVIEW "count(*), "                             \
    " date_part('epoch', date_trunc('hour', intime))*1000 AS msec"
#define _COL_EMAP_USERVIEW "count(DISTINCT sender), "               \
    " date_part('epoch', date_trunc('hour', intime))*1000 AS msec"

#define _COL_EMAP_DATEVIEW "count(*), "                             \
    " date_part('epoch', date_trunc('day', intime))*1000 AS msec"
#define _COL_EMAP_DATEUSER "count(DISTINCT sender), "               \
    " date_part('epoch', date_trunc('day', intime))*1000 AS msec"

NEOERR* system_view_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "trace");
    STRING str; string_init(&str);
    char *mname, *s;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, db);
    
    MEMBER_CHECK_ADMIN();
    SET_DASHBOARD_ACTION(cgi->hdf);

    /*
     * prepare query condition
     */
    hdf_set_int_value(cgi->hdf, PRE_QUERY".type", TRACE_TYPE_PAGEVIEW);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".times", s);
    if (!s) hdf_set_value(cgi->hdf, PRE_QUERY".timed", "current_date - 2");
    err = mdb_build_querycond(hdf_get_obj(cgi->hdf, PRE_QUERY),
                              hdf_get_obj(g_cfg, "Db.QueryCond.system.view"),
                              &str, NULL);
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * pageview
     */
    MDB_QUERY_RAW(db, "emap", _COL_EMAP_PAGEVIEW,
                  " %s GROUP BY date_trunc('hour', intime) "
                  " ORDER BY date_trunc('hour', intime)", NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_EMAP_PAGEVIEW, PRE_OUTPUT".pageviews",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * userview
     */
    MDB_QUERY_RAW(db, "emap", _COL_EMAP_USERVIEW,
                  " %s GROUP BY date_trunc('hour', intime) "
                  " ORDER BY date_trunc('hour', intime)", NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_EMAP_USERVIEW, PRE_OUTPUT".userviews",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * day count
     */
    MDB_QUERY_RAW(db, "emap", _COL_EMAP_DATEVIEW,
                  " %s GROUP BY date_trunc('day', intime) "
                  " ORDER BY date_trunc('day', intime)", NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_EMAP_DATEVIEW, PRE_OUTPUT".dateviews",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);
    MDB_QUERY_RAW(db, "emap", _COL_EMAP_DATEUSER,
                  " %s GROUP BY date_trunc('day', intime) "
                  " ORDER BY date_trunc('day', intime)", NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_EMAP_DATEUSER, PRE_OUTPUT".dateusers",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    string_clear(&str);

    hdf_set_attr(cgi->hdf, PRE_OUTPUT".pageviews", "type", "array");
    hdf_set_attr(cgi->hdf, PRE_OUTPUT".userviews", "type", "array");
    
    return STATUS_OK;
}

#define _COL_EMAP_WHO "sender, max(provid) AS provid, max(cityid) AS cityid, " \
    " count(sender), "                                                  \
    " to_char(max(intime), 'YYYY-MM-DD 00:00:00') AS day, "             \
    " to_char(min(intime), 'YYYY-MM-DD HH24:MI:SS') AS intime"
NEOERR* system_who_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    STRING str; string_init(&str);
    mdb_conn *db = hash_lookup(dbh, "trace");
    char *mname;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, db);
    
    MEMBER_CHECK_ADMIN();
    SET_DASHBOARD_ACTION(cgi->hdf);

    hdf_set_int_value(cgi->hdf, PRE_QUERY".type", TRACE_TYPE_PAGEVIEW);
    err = mdb_build_querycond(hdf_get_obj(cgi->hdf, PRE_QUERY),
                              hdf_get_obj(g_cfg, "Db.QueryCond.system.who"),
                              &str, NULL);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_QUERY_RAW(db, "emap", _COL_EMAP_WHO,
                  " %s GROUP BY sender ORDER BY min(intime) DESC LIMIT 100",
                  NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_EMAP_WHO, PRE_OUTPUT".who",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    string_clear(&str);

    hdf_set_attr(cgi->hdf, PRE_OUTPUT".who", "type", "array");

    return STATUS_OK;
}

#define _COL_EMAP_VIEW_DETAIL "sender, type, "              \
    " ei_one, ei_two, ei_three, es_one, es_two, es_three, " \
    " et_one, et_two, et_three, "                           \
    " to_char(intime, 'YYYY-MM-DD HH24:MI:SS') as intime"
NEOERR* system_view_detail_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    STRING str; string_init(&str);
    mdb_conn *db = hash_lookup(dbh, "trace");
    char *mname;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, db);
    
    MEMBER_CHECK_ADMIN();
    SET_DASHBOARD_ACTION(cgi->hdf);

    err = mdb_build_querycond(hdf_get_obj(cgi->hdf, PRE_QUERY),
                              hdf_get_obj(g_cfg, "Db.QueryCond.system.viewdetail"),
                              &str, NULL);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_QUERY_RAW(db, "emap", _COL_EMAP_VIEW_DETAIL,
                  " %s ORDER BY intime LIMIT 100", NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_EMAP_VIEW_DETAIL, PRE_OUTPUT".details",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    string_clear(&str);

    hdf_set_attr(cgi->hdf, PRE_OUTPUT".details", "type", "array");

    return STATUS_OK;
}

NEOERR* system_comment_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "aux");
    STRING str; string_init(&str);
    char *mname;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, db);
    
    MEMBER_CHECK_ADMIN();
    SET_DASHBOARD_ACTION(cgi->hdf);

    if (!hdf_get_value(cgi->hdf, PRE_QUERY".times", NULL))
        hdf_set_value(cgi->hdf, PRE_QUERY".timed", "current_date - 7");
    err = mdb_build_querycond(hdf_get_obj(cgi->hdf, PRE_QUERY),
                              hdf_get_obj(g_cfg, "Db.QueryCond.system.comment"),
                              &str, NULL);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_QUERY_RAW(db, "comment", _COL_CMT, "%s ORDER BY id DESC", NULL, str.buf);
    err = mdb_set_rows(cgi->hdf, db, _COL_CMT, PRE_OUTPUT".rows",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    return STATUS_OK;
}

NEOERR* system_plan_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "plan");
    STRING str; string_init(&str);
    int count, offset;
    char *mname;
    HDF *node;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, db);
    
    MEMBER_CHECK_ADMIN();
    SET_DASHBOARD_ACTION(cgi->hdf);

    hdf_get_node(cgi->hdf, PRE_OUTPUT, &node);

    mdb_pagediv(hdf_get_obj(cgi->hdf, PRE_QUERY),
                NULL, &count, &offset, NULL, node);

    err = mdb_build_querycond(hdf_get_obj(cgi->hdf, PRE_QUERY),
                              hdf_get_obj(g_cfg, "Db.QueryCond.system.plan"),
                              &str, "intime > current_date - 1");
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_PAGEDIV_SET_N(node, db, "plan", "%s", NULL, str.buf);

    MDB_QUERY_RAW(db, "plan", _COL_PLAN_ADMIN,
                  "%s ORDER BY id DESC LIMIT %d OFFSET %d",
                  NULL, str.buf, count, offset);
    err = mdb_set_rows(cgi->hdf, db, _COL_PLAN_ADMIN, PRE_OUTPUT".rows",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    return STATUS_OK;
}
