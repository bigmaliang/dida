#include "mevent_plugin.h"
#include "mevent_trace.h"

#define PLUGIN_NAME    "trace"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

struct trace_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct trace_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct trace_stats st;
};

static NEOERR* trace_cmd_get(struct trace_entry *e, QueueEntry *q)
{
    return STATUS_OK;
}

static NEOERR* trace_cmd_add(struct trace_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    NEOERR *err;

    mdb_conn *db = e->db;

    err = mdb_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.emap"), &str);
    if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "INSERT INTO emap %s", NULL, str.buf);

    string_clear(&str);
    
    return STATUS_OK;
}

static void trace_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct trace_entry *e = (struct trace_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct trace_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_TRACE_GET:
        err = trace_cmd_get(e, q);
        break;
    case REQ_CMD_TRACE_ADD:
        err = trace_cmd_add(e, q);
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

static void trace_stop_driver(EventEntry *entry)
{
    struct trace_entry *e = (struct trace_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* trace_init_driver(void)
{
    struct trace_entry *e = calloc(1, sizeof(struct trace_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = trace_process_driver;
    e->base.stop_driver = trace_stop_driver;
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

struct event_driver trace_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = trace_init_driver,
};
