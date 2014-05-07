#include "mevent_plugin.h"
#include "mevent_member.h"
#include "mevent_plan.h"
#include "mevent_public.h"
#include "mevent_fft.h"

#define PLUGIN_NAME    "fft"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

struct fft_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct fft_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct fft_stats st;
};

static NEOERR* fft_cmd_expect_add(struct fft_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    NEOERR *err;
    
    mdb_conn *db = e->db;

    MEMBER_SET_PARAM_MID(q->hdfrcv);
    hdf_set_int_value(q->hdfrcv, "statu", FFT_EXPECT_STATU_OK);

    err = mdb_build_mgcol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".MergeCol.expect"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "SELECT merge_expect(%s)", NULL, str.buf);

    string_clear(&str);
    
    return STATUS_OK;
}

static NEOERR* fft_cmd_expect_up(struct fft_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    int mid, pid, addrtype, caomin = 0;
    mdb_conn *db = e->db;
    NEOERR *err;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    REQ_GET_PARAM_INT(q->hdfrcv, "pid", pid);
    REQ_GET_PARAM_INT(q->hdfrcv, "addrtype", addrtype);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "_caomin", caomin);

    err = mdb_build_upcol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".UpdateCol.expect"), &str);
    if (err != STATUS_OK) return nerr_pass(err);
    
    if (caomin == 1) {
        MDB_EXEC(db, NULL, "UPDATE expect SET %s WHERE "
                 " pid=%d AND mid=%d AND addrtype=%d",
                 NULL, str.buf, pid, mid, addrtype);
    } else {
        MDB_EXEC(db, NULL, "UPDATE expect SET %s WHERE pid=%d AND addrtype=%d",
                 NULL, str.buf, pid, addrtype);
    }

    string_clear(&str);

    return STATUS_OK;
}

/*
 * plan 入库后，遍历 expect 表，看是否这条 plan match 到了 expect 中的 plan
 * 输入参数:
 *  必传 id, dad, rect, sgeo, km
 *  可选 [sdate, stime] or repeat
 * 返回: 1 + 2
 *  1, 匹配到的 expect， insert into meet table
 *  2, 匹配到的 expect 且其 gotime == FFT_GOTIME_IMMEDIATE 以数组形式返回，以便后续处理
 */
static NEOERR* fft_cmd_expect_match(struct fft_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    HDF *node, *child;
    char *pdate, *ptime;
    int ttnum = 0, retnum = 0, eid, pid, repeat, maxday;
    time_t thatsec;
    float km;
    NEOERR *err;
    mdb_conn *db = e->db;

    REQ_GET_PARAM_INT(q->hdfrcv, "id", pid);
    REQ_FETCH_PARAM_STR(q->hdfrcv, "sdate", pdate);
    REQ_FETCH_PARAM_STR(q->hdfrcv, "stime", ptime);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "repeat", repeat);
    km = mcs_get_float_value(q->hdfrcv, "km", 0.0);
    hdf_set_int_value(q->hdfrcv, "statu", FFT_EXPECT_STATU_OK);
    
    err = hdf_init(&node);
    if (err != STATUS_OK) return nerr_pass(err);
    
    err = mdb_build_querycond(q->hdfrcv,
                              hdf_get_obj(g_cfg, CONFIG_PATH".QueryCond.expect"),
                              &str, NULL);
    if (err != STATUS_OK) return nerr_pass(err);

    /*
     * get all matched expect by position
     */
    MDB_QUERY_RAW(db, "expect", _COL_EXPECT, "%s", NULL, str.buf);
    err = mdb_set_rows(node, db, _COL_EXPECT, "expects", "0", MDB_FLAG_Z);
    if (nerr_handle(&err, NERR_NOT_FOUND)) return STATUS_OK;
    if (err != STATUS_OK) return nerr_pass(err);

    child = hdf_get_obj(node, "expects");

    if (repeat > PLAN_RPT_NONE) {
        /*
         * TODO match repeated plan ignore sdate currently
         */
        while (child) {
            ttnum++;
            eid = hdf_get_int_value(child, "id", 0);
            MDB_EXEC(db, NULL, "INSERT INTO meet (eid, pid) VALUES (%d, %d);",
                     NULL, eid, pid);
            if (hdf_get_int_value(child, "gotime", FFT_GOTIME_WEEK) ==
                FFT_GOTIME_IMMEDIATE) {
                retnum++;
                mcs_copyf(node, child, "rexpects.%s", hdf_obj_name(child));
            }

            child = hdf_obj_next(child);
        }
    } else {
        /*
         * sort expect by time
         */
        thatsec = pub_plan_get_abssec(pdate, ptime);
        err = pub_plan_sort_by_time(child, km, thatsec, pdate);
        if (err != STATUS_OK) return nerr_pass(err);

        /*
         * filter by relative second
         */
        maxday = hdf_get_int_value(g_cfg, CONFIG_PATH".meetMaxDay", 1);
        child = hdf_get_child(node, "expects");
        while (child) {
            if (pub_plan_get_relsec(child, thatsec) < (maxday * ONE_DAY)) {
                ttnum++;
                eid = hdf_get_int_value(child, "id", 0);
                MDB_EXEC(db, NULL, "INSERT INTO meet (eid, pid) VALUES (%d, %d);",
                         NULL, eid, pid);
                if (hdf_get_int_value(child, "gotime", FFT_GOTIME_WEEK) ==
                    FFT_GOTIME_IMMEDIATE) {
                    retnum++;
                    mcs_copyf(node, child, "rexpects.%s", hdf_obj_name(child));
                }
            }

            child = hdf_obj_next(child);
        }
    }

    child = hdf_get_obj(node, "rexpects");
    if (child) hdf_copy(q->hdfsnd, "expects", child);

    string_clear(&str);
    
    return STATUS_OK;
}

static void fft_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct fft_entry *e = (struct fft_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct fft_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_FFT_EXPECT_ADD:
        err = fft_cmd_expect_add(e, q);
        break;
    case REQ_CMD_FFT_EXPECT_UP:
        err = fft_cmd_expect_up(e, q);
        break;
    case REQ_CMD_FFT_EXPECT_MATCH:
        err = fft_cmd_expect_match(e, q);
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

static void fft_stop_driver(EventEntry *entry)
{
    struct fft_entry *e = (struct fft_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* fft_init_driver(void)
{
    struct fft_entry *e = calloc(1, sizeof(struct fft_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = fft_process_driver;
    e->base.stop_driver = fft_stop_driver;
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

struct event_driver fft_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = fft_init_driver,
};
