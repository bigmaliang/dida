#include "mevent_plugin.h"
#include "mevent_member.h"
#include "mevent_public.h"
#include "mevent_plan.h"

#define PLUGIN_NAME    "plan"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

struct plan_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct plan_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct plan_stats st;
};

static bool plan_spd_exist(struct plan_entry *e, QueueEntry *q,
                           char *ori, char *id)
{
    unsigned char *val = NULL; size_t vsize = 0;
    NEOERR *err;

    if (!ori || !id) return false;
    
    mdb_conn *db = e->db;
    struct cache *cd = e->cd;
    
    ori = hdf_get_valuef(g_cfg, "Odomain.%s", ori);
    
    if (!cache_getf(cd, &val, &vsize, PREFIX_SPD"%s%s", ori, id)) {
        MDB_QUERY_RAW(db, "plan", "mid", "ori=$1 AND oid=$2", "ss", ori, id);
        err = mdb_set_row(q->hdfsnd, db, "mid", NULL, MDB_FLAG_Z);
        if(nerr_handle(&err, NERR_NOT_FOUND)) return false;
        nerr_ignore(&err);
        
        CACHE_HDF(q->hdfsnd, SPD_CC_SEC, PREFIX_SPD"%s%s", ori, id);
    }
    return true;
}

static NEOERR* plan_cmd_plan_get(struct plan_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    int id;
    NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;
    
    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);

    if (cache_getf(cd, &val, &vsize, PREFIX_PLAN"%d", id)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_QUERY_RAW(db, "plan", _COL_PLAN, "id=%d", NULL, id);
        err = mdb_set_row(q->hdfsnd, db, _COL_PLAN, NULL, MDB_FLAG_Z);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, PLAN_CC_SEC, PREFIX_PLAN"%d", id);
    }
    
    return STATUS_OK;
}

static NEOERR* plan_cmd_plan_priv_get(struct plan_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    int id;
    NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;
    
    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);

    if (cache_getf(cd, &val, &vsize, PREFIX_PLAN_PRIV"%d", id)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_QUERY_RAW(db, "plan", _COL_PLAN_ADMIN, "id=%d", NULL, id);
        err = mdb_set_row(q->hdfsnd, db, _COL_PLAN_ADMIN, NULL, MDB_FLAG_Z);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, PLAN_CC_SEC, PREFIX_PLAN_PRIV"%d", id);
    }
    
    return STATUS_OK;
}

/*
 * get plans by geographically, and filter by time
 * keep plan clean (no history useless plan) please
 * limit by:
 *    dad, statu, (scityid, ecityid) / rect
 * PARAMETERS:
 *   dad, scityid, ecityid, rect, date, time
 *   dayaround : nearest day number returned
 *   _nmax     : max plans number returned (ignore if dayaround presented)
 */
static NEOERR* plan_cmd_plan_match(struct plan_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    unsigned char *val = NULL; size_t vsize = 0;
    NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;
    
    int dad, scityid = 0, ecityid = 0, ttnum, nmax = 0, maxday = 0;
    char *rect = NULL, *pdate, *ptime;
    time_t thatsec;
    float km;

    REQ_GET_PARAM_INT(q->hdfrcv, "dad", dad);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "scityid", scityid);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "ecityid", ecityid);
    REQ_FETCH_PARAM_STR(q->hdfrcv, "rect", rect);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "dayaround", maxday);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "_nmax", nmax);

    pdate = hdf_get_value(q->hdfrcv, "date", NULL);
    ptime = hdf_get_value(q->hdfrcv, "time", "08:00:00");
    km = mcs_get_float_value(q->hdfrcv, "km", 0.0);
    if (nmax <= 0) nmax = hdf_get_int_value(g_cfg, CONFIG_PATH".geoMax", 30);
    
    hdf_set_int_value(q->hdfrcv, "statu", PLAN_ST_PAUSE);

    if ((scityid == 0 || ecityid == 0) && rect == NULL)
        return nerr_raise(REP_ERR_BADPARAM, "paramter null");

    if (cache_getf(cd, &val, &vsize, PREFIX_PLAN"%d_%d_%d_%s_%d_%d_%s_%s",
                   dad, scityid, ecityid, rect, nmax, maxday, pdate, ptime)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        err = mdb_build_querycond(q->hdfrcv,
                                  hdf_get_obj(g_cfg, CONFIG_PATH".QueryCond.geoa"),
                                  &str, NULL);
        if (err != STATUS_OK) return nerr_pass(err);

        MDB_QUERY_RAW(db, "plan", _COL_PLAN, "%s", NULL, str.buf);
        err = mdb_set_rows(q->hdfsnd, db, _COL_PLAN, "plans", "0", MDB_FLAG_Z);
        ttnum = mdb_get_rows(db);

        /*
         * too many plans
         */
        if (ttnum > hdf_get_int_value(g_cfg, CONFIG_PATH".geoaMax", 0)) {
            hdf_destroy(&q->hdfsnd);
            hdf_init(&q->hdfsnd);
            ttnum = 0;
        }
        
        /*
         * too few/many plans by cityid
         */
        if (nerr_handle(&err, NERR_NOT_FOUND) ||
            (ttnum < hdf_get_int_value(g_cfg, CONFIG_PATH".geoaMin", 0)) ) {
            string_clear(&str);
            hdf_remove_tree(q->hdfsnd, "plans");
            mdb_build_querycond(q->hdfrcv,
                                hdf_get_obj(g_cfg, CONFIG_PATH".QueryCond.geob"),
                                &str, NULL);
            MDB_QUERY_RAW(db, "plan", _COL_PLAN, "%s", NULL, str.buf);
            err = mdb_set_rows(q->hdfsnd, db, _COL_PLAN, "plans", "0", MDB_FLAG_Z);
            ttnum = mdb_get_rows(db);

            if (nerr_handle(&err, NERR_NOT_FOUND)) {
                /*
                 * ok, try larger at last
                 */
                string_clear(&str);
                hdf_remove_tree(q->hdfsnd, "plans");
                mdb_build_querycond(q->hdfrcv,
                                    hdf_get_obj(g_cfg, CONFIG_PATH".QueryCond.geox"),
                                    &str, NULL);
                MDB_QUERY_RAW(db, "plan", _COL_PLAN, "%s", NULL, str.buf);
                err = mdb_set_rows(q->hdfsnd, db, _COL_PLAN, "plans", "0",
                                   MDB_FLAG_EMPTY_OK);
                ttnum = mdb_get_rows(db);
            }
            if (err != STATUS_OK) return nerr_pass(err);
        }

        if (!pdate || ttnum == 0) goto done;
            
        /*
         * fuck the time
         * this could be done by robot side,
         * process here for more efficient transport, and more user likely
         */
        thatsec = pub_plan_get_abssec(pdate, ptime);
        HDF *node = hdf_get_obj(q->hdfsnd, "plans");
        err = pub_plan_sort_by_time(node, km, thatsec, pdate);
        if (err != STATUS_OK) return nerr_pass(err);
        
        char name[LEN_HDF_KEY];
        if (maxday > 0) {
            /*
             * remove plan according datetime
             */
            node = hdf_get_child(q->hdfsnd, "plans");
            while (node) {
                if (pub_plan_get_relsec(node, thatsec) < (maxday * 24 * 60 * 60)) {
                    node = hdf_obj_next(node);
                    continue;
                }
                    
                snprintf(name, sizeof(name), "plans.%s", hdf_obj_name(node));
                node = hdf_obj_next(node);
                hdf_remove_tree(q->hdfsnd, name);
                ttnum--;
            }
        } else if (ttnum > nmax){
            /*
             * remove plan according nmax
             */
            int rnum = 0;
            node = hdf_get_child(q->hdfsnd, "plans");
            while (node) {
                rnum++;
                        
                if (rnum <= nmax) {
                    node = hdf_obj_next(node);
                    continue;
                }

                snprintf(name, sizeof(name), "plans.%s", hdf_obj_name(node));
                node = hdf_obj_next(node);
                hdf_remove_tree(q->hdfsnd, name);
                ttnum--;
            }
        }

    done:
        mtc_foo("get %d results", ttnum);
        hdf_set_int_value(q->hdfsnd, "_ntt", ttnum);
        if (ttnum == 0) hdf_remove_tree(q->hdfsnd, "plans");
        CACHE_HDF(q->hdfsnd, PLAN_CC_SEC, PREFIX_PLAN"%d_%d_%d_%s_%d_%d_%s_%s",
                  dad, scityid, ecityid, rect, nmax, maxday, pdate, ptime);
    }
    
    string_clear(&str);
    
    return STATUS_OK;
}

static NEOERR* plan_cmd_plan_add(struct plan_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    char *ori = NULL, *oid = NULL, *tmps;
    int mid;
    NEOERR *err;

    struct cache *cd = e->cd;
    mdb_conn *db = e->db;

    REQ_FETCH_PARAM_STR(q->hdfrcv, "ori", ori);
    REQ_FETCH_PARAM_STR(q->hdfrcv, "oid", oid);
    
    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    
    if (ori) {
        if (plan_spd_exist(e, q, ori, oid)) {
            mtc_warn("%s %s planed", ori, oid);
            return STATUS_OK;
        }
        
        tmps = hdf_get_valuef(g_cfg, "Odomain.%s", ori);
        if (tmps) hdf_set_value(q->hdfrcv, "ori", tmps);
    }


    err = mdb_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.plan"),
                          &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "INSERT INTO plan %s RETURNING id", NULL, str.buf);

    err = mdb_set_row(q->hdfsnd, db, "id", NULL, MDB_FLAG_Z);
    if (err != STATUS_OK) return nerr_pass(err);

    string_clear(&str);

    cache_delf(cd, PREFIX_PLAN_MINE"%d_0", mid);

    return STATUS_OK;
}

static NEOERR* plan_cmd_plan_up(struct plan_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    int id, mid, caomin = 0;
    char *pmid;
    NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "_caomin", caomin);

    if (caomin == 1) {
        MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    }

    err = plan_cmd_plan_get(e, q);
    if (err != STATUS_OK) return nerr_pass(err);
    
    pmid = hdf_get_value(q->hdfsnd, "mid", NULL);
    if (!pmid || (caomin == 1 && mid != atoi(pmid)))
        return nerr_raise(REP_ERR_PLAN_NEXIST, "plan %d not exist", id);

    err = mdb_build_upcol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".UpdateCol.plan"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    if (caomin == 1) {
        MDB_EXEC(db, NULL, "UPDATE plan SET %s WHERE id=%d AND (mid=%d OR mid=0);",
                 NULL, str.buf, id, mid);
    } else {
        MDB_EXEC(db, NULL, "UPDATE plan SET %s WHERE id=%d;", NULL, str.buf, id);
    }

    string_clear(&str);

    cache_delf(cd, PREFIX_PLAN"%d", id);
    cache_delf(cd, PREFIX_PLAN_MINE"%d_0", atoi(pmid));
    
    return STATUS_OK;
}

static NEOERR* plan_cmd_plan_mine(struct plan_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    struct cache *cd = e->cd;
    mdb_conn *db = e->db;
    int count, offset;
    int mid, guest = 0;
    char *cols = _COL_PLAN_ADMIN;
    NEOERR *err;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "_guest", guest);
    if (guest) cols = _COL_PLAN;

    mdb_pagediv(q->hdfrcv, NULL, &count, &offset, PRE_RESERVE, q->hdfsnd);

    if (cache_getf(cd, &val, &vsize, PREFIX_PLAN_MINE"%d_%d", mid, offset)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_PAGEDIV_SET(q->hdfsnd, PRE_RESERVE, db, "plan", "statu<%d AND mid=%d",
                        NULL, PLAN_ST_DELETE, mid);
        if (guest) {
            MDB_QUERY_RAW(db, "plan", _COL_PLAN, "statu<%d AND mid=%d "
                          " ORDER BY id DESC LIMIT %d OFFSET %d",
                          NULL, PLAN_ST_DELETE, mid, count, offset);
        } else {
            MDB_QUERY_RAW(db, "plan", _COL_PLAN_ADMIN, "statu<%d AND mid=%d "
                          " ORDER BY id DESC LIMIT %d OFFSET %d",
                          NULL, PLAN_ST_DELETE, mid, count, offset);
        }
        err = mdb_set_rows(q->hdfsnd, db, cols, "plans", NULL, MDB_FLAG_EMPTY_OK);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, PLAN_MINE_CC_SEC, PREFIX_PLAN_MINE"%d_%d", mid, offset);
    }

    return STATUS_OK;
}

static NEOERR* plan_cmd_plan_recent(struct plan_entry *e, QueueEntry *q)
{
    //unsigned char *val = NULL; size_t vsize = 0;
    int maxday, limit;
    NEOERR *err;

    //struct cache *cd = e->cd;
    mdb_conn *db = e->db;

    REQ_FETCH_PARAM_INT(q->hdfrcv, "maxday", maxday);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "limit", limit);

    if (limit <= 0) limit = hdf_get_int_value(g_cfg, CONFIG_PATH".recentMax", 100);

    /*
    if (cache_getf(cd, &val, &vsize, PREFIX_RECENT"%d_%d", maxday, limit)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
    */
        if (maxday > 0) {
            MDB_QUERY_RAW(db, "plan", _COL_PLAN, "statu < %d AND intime > "
                          " current_date - %d ORDER BY RANDOM() LIMIT %d", NULL,
                          PLAN_ST_PAUSE, maxday - 1, limit);
        } else {
            /*
             * don't care intime
             */
            MDB_QUERY_RAW(db, "plan", _COL_PLAN, "statu < %d ORDER BY RANDOM() "
                          " LIMIT %d", NULL, PLAN_ST_PAUSE, limit);
        }
        err = mdb_set_rows(q->hdfsnd, db, _COL_PLAN, NULL, NULL, MDB_FLAG_EMPTY_OK);
        if (err != STATUS_OK) return nerr_pass(err);

        /*
        CACHE_HDF(q->hdfsnd, RECENT_CC_SEC, PREFIX_RECENT"%d_%d", maxday, limit);
    }
        */
    
    return STATUS_OK;
}

static NEOERR* plan_cmd_spd_peel(struct plan_entry *e, QueueEntry *q)
{
    char *ori, *id;
    HDF *oids;
    int cnt = 0;

    REQ_GET_PARAM_STR(q->hdfrcv, "ori", ori);
    REQ_GET_PARAM_CHILD(q->hdfrcv, "oids", oids);

    while (oids) {
        id = hdf_obj_value(oids);
        if (!plan_spd_exist(e, q, ori, id)) {
            hdf_set_valuef(q->hdfsnd, "oids.%d=%s", cnt++, id);
        }
        oids = hdf_obj_next(oids);
    }

    return STATUS_OK;
}

static void plan_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct plan_entry *e = (struct plan_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct plan_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_PLAN_GET:
        err = plan_cmd_plan_get(e, q);
        break;
    case REQ_CMD_PLAN_PRIV_GET:
        err = plan_cmd_plan_priv_get(e, q);
        break;
    case REQ_CMD_PLAN_MATCH:
        err = plan_cmd_plan_match(e, q);
        break;
    case REQ_CMD_PLAN_ADD:
        err = plan_cmd_plan_add(e, q);
        break;
    case REQ_CMD_PLAN_UP:
        err = plan_cmd_plan_up(e, q);
        break;
    case REQ_CMD_PLAN_MINE:
        err = plan_cmd_plan_mine(e, q);
        break;
    case REQ_CMD_PLAN_RECENT:
        err = plan_cmd_plan_recent(e, q);
        break;
    case REQ_CMD_SPD_PEEL:
        err = plan_cmd_spd_peel(e, q);
        break;
    case REQ_CMD_STATS:
        st->msg_stats++;
        err = STATUS_OK;
        hdf_set_int_value(q->hdfsnd, "msg_total", st->msg_total);
        hdf_set_int_value(q->hdfsnd, "msg_unrec", st->msg_unrec);
        hdf_set_int_value(q->hdfsnd, "msg_badparam", st->msg_badparam);
        hdf_set_int_value(q->hdfsnd, "msg_stats", st->msg_stats);
        hdf_set_int_value(q->hdfsnd, "proc_suc", st->proc_suc);
        hdf_set_int_value(q->hdfsnd, "proc_fai", st->proc_fai);
        break;
    default:
        st->msg_unrec++;
        err = nerr_raise(REP_ERR_UNKREQ, "unknown command %u", q->operation);
        break;
    }
    
    NEOERR *neede = mcs_err_valid(err);
    ret = neede ? neede->error : REP_OK;
    if (PROCESS_OK(ret)) {
        st->proc_suc++;
    } else {
        st->proc_fai++;
        if (ret == REP_ERR_BADPARAM) {
            st->msg_badparam++;
        }
        TRACE_ERR(q, ret, err);
    }
    if (q->req->flags & FLAGS_SYNC) {
        reply_trigger(q, ret);
    }
}

static void plan_stop_driver(EventEntry *entry)
{
    struct plan_entry *e = (struct plan_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* plan_init_driver(void)
{
    struct plan_entry *e = calloc(1, sizeof(struct plan_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = plan_process_driver;
    e->base.stop_driver = plan_stop_driver;
    //mevent_add_timer(&e->base.timers, 60, true, hint_timer_up_term);

    char *s = hdf_get_value(g_cfg, CONFIG_PATH".dbsn", NULL);
    err = mdb_init(&e->db, s);
    JUMP_NOK(err, error);
    
    e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
    if (e->cd == NULL) {
        wlog("init cache failure");
        goto error;
    }
    
    return (EventEntry*)e;
    
error:
    if (e->base.name) free(e->base.name);
    if (e->db) mdb_destroy(e->db);
    if (e->cd) cache_free(e->cd);
    free(e);
    return NULL;
}

struct event_driver plan_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = plan_init_driver,
};
