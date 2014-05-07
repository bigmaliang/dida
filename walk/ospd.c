#include "mheads.h"
#include "lheads.h"
#include "ospd.h"

NEOERR* spd_pre_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan");

    if (!cgi || !cgi->hdf || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_SPD_PEEL, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* spd_do_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt;
    HDF *plan;
    int cityid = 0;
    
    if (!cgi || !cgi->hdf) return nerr_raise(NERR_ASSERT, "paramter null");

    HDF_GET_OBJ(cgi->hdf, PRE_QUERY".plan", plan);
    
    /*
     * city
     */
    char *s = hdf_get_value(plan, "city", NULL);
    if (s) {
        evt = hash_lookup(evth, "city");
        if (!evt) return nerr_raise(NERR_ASSERT, "city null");

        hdf_set_value(evt->hdfsnd, "c", s);
        MEVENT_TRIGGER_NRET(evt, NULL, REQ_CMD_CITY_BY_S, FLAGS_SYNC);
        cityid = hdf_get_int_value(evt->hdfrcv, "citys.0.id", 0);
    }

    /*
     * plan
     */
    evt = hash_lookup(evth, "plan");
    if (!evt) return nerr_raise(NERR_ASSERT, "plan backend error");
    
    hdf_copy(evt->hdfsnd, NULL, plan);
    hdf_set_int_value(evt->hdfsnd, "cityid", cityid);
    hdf_set_int_value(evt->hdfsnd, "statu", PLAN_ST_SPD_FRESH);

    s = hdf_get_value(evt->hdfsnd, "stime", NULL);
    if (s && !reg_search("^[0-2][0-9](:[0-9][0-9])+$", s))
        hdf_remove_tree(evt->hdfsnd, "stime");

    s = hdf_get_value(evt->hdfsnd, "etime", NULL);
    if (s && !reg_search("^[0-2][0-9](:[0-9][0-9])+$", s))
        hdf_remove_tree(evt->hdfsnd, "etime");

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_ADD, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* spd_post_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "plan");

    if (!cgi || !cgi->hdf || !db) return nerr_raise(NERR_ASSERT, "paramter null");

    MDB_PAGEDIV_SET(cgi->hdf, PRE_OUTPUT, db, "plan", "statu=%d",
                    NULL, PLAN_ST_SPD_RBTED);
    
    return STATUS_OK;
}

NEOERR* spd_post_do_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "plan");
	NEOERR *err;

    if (!cgi || !cgi->hdf || !db) return nerr_raise(NERR_ASSERT, "paramter null");

    int id = hdf_get_int_value(cgi->hdf, PRE_QUERY".exceptid", 0);

    MDB_QUERY_RAW(db, "plan", _COL_PLAN_ADMIN, "statu=%d AND id !=%d LIMIT 1",
                  NULL, PLAN_ST_SPD_RBTED, id);
    err = mdb_set_row(cgi->hdf, db, _COL_PLAN_ADMIN, PRE_OUTPUT".plan", MDB_FLAG_Z);
	if (err != STATUS_OK) return nerr_pass(err);
    
    int cityid = hdf_get_int_value(cgi->hdf, PRE_OUTPUT".plan.cityid", 0);

    MDB_QUERY_RAW(db, "city", _COL_CITY, "id=%d", NULL, cityid);
    mdb_set_row(cgi->hdf, db, _COL_CITY, PRE_OUTPUT".city", MDB_FLAG_NO_ERR);
    
    return STATUS_OK;
}

NEOERR* spd_post_do_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt;

    if (!cgi || !cgi->hdf) return nerr_raise(NERR_ASSERT, "paramter null");

    evt = hash_lookup(evth, "plan");
    if (!evt) return nerr_raise(NERR_ASSERT, "plan null");
    
    if (!hdf_get_obj(cgi->hdf, PRE_QUERY".plan"))
        return nerr_raise(NERR_ASSERT, "plan null");
    
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY".plan"));
    hdf_set_int_value(evt->hdfsnd, "statu", PLAN_ST_SPD_OK);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_UP, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* spd_post_do_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt;

    if (!cgi || !cgi->hdf) return nerr_raise(NERR_ASSERT, "paramter null");

    evt = hash_lookup(evth, "plan");
    if (!evt) return nerr_raise(NERR_ASSERT, "plan null");
    
    if (!hdf_get_obj(cgi->hdf, PRE_QUERY".plan"))
        return nerr_raise(NERR_ASSERT, "plan null");
    
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY".plan"));
    hdf_set_int_value(evt->hdfsnd, "statu", PLAN_ST_DELETE);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_UP, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* spd_robot_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    return STATUS_OK;
}

NEOERR* spd_post_robot_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mdb_conn *db = hash_lookup(dbh, "plan");
    NEOERR *err;

    if (!cgi || !cgi->hdf || !db) return nerr_raise(NERR_ASSERT, "paramter null");
    
    MDB_EXEC(db, NULL, "SELECT " _COL_PLAN_C " FROM plan p, city c WHERE "
             "p.statu=%d AND c.id=p.cityid LIMIT 100", NULL, PLAN_ST_SPD_FRESH);
    err = mdb_set_rows(cgi->hdf, db, _COL_PLAN_C, PRE_OUTPUT".plans",
                       NULL, MDB_FLAG_EMPTY_OK);
    if (err != STATUS_OK) return nerr_pass(err);

    hdf_set_attr(cgi->hdf, PRE_OUTPUT".plans", "type", "array");

    return STATUS_OK;
}

NEOERR* spd_post_robot_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan");
    HDF *plan;
    NEOERR *err;
    
    if (!cgi || !cgi->hdf || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

    HDF_GET_OBJ(cgi->hdf, PRE_QUERY".plan", plan);

    hdf_copy(evt->hdfsnd, NULL, plan);
    if (hdf_get_int_value(plan, "statu", PLAN_ST_SPD_FRESH) != PLAN_ST_SPD_RBTED) {
        hdf_set_int_value(evt->hdfsnd, "statu", PLAN_ST_SPD_RBT_OK);

        MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_UP, FLAGS_SYNC);

        /*
         * NOTIFY EXPECTED USER
         * ====================
         * copied from plan_leave_data_add()
         */
        mevent_t *evtf = hash_lookup(evth, "fft");
        MCS_NOT_NULLA(evtf);

        hdf_init(&plan);
        hdf_copy(plan, NULL, evt->hdfrcv);
        
        hdf_copy(evtf->hdfsnd, NULL, plan);

        MEVENT_TRIGGER(evtf, NULL, REQ_CMD_FFT_EXPECT_MATCH, FLAGS_SYNC);

        HDF *obj = hdf_get_obj(evtf->hdfrcv, "expects");
        if (obj) {
            /*
             * inbox notify
             */
            hdf_copy(obj, PRE_DATASET".Output.plans.0", plan);

            err = inbox_multi_add(obj, evth, "PlanMatched");
            if (err != STATUS_OK) return nerr_pass(err);

            /*
             * email notify
             */
            HDF *child, *tnode;
            hdf_init(&tnode);
            child = hdf_obj_child(obj);
            while (child) {
                if (hdf_get_int_value(child, "addrtype", FFT_EXPECT_NONE) ==
                    FFT_EXPECT_EMAIL) {
                    hdf_copy(tnode, hdf_obj_name(child), child);
                }
            
                child = hdf_obj_next(child);
            }

            if (hdf_obj_child(tnode)) {
                hdf_copy(tnode, PRE_DATASET".Output.plans.0", plan);

                err = email_multi_add(tnode, evth, "PlanMatched");
                if (err != STATUS_OK) return nerr_pass(err);
            }

            hdf_destroy(&tnode);

            /*
             * TODO sms notify
             */
        }
        hdf_destroy(&plan);
    } else {
        MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_UP, FLAGS_NONE);
    }

    return STATUS_OK;
}
