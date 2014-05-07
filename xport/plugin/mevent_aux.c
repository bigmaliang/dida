#include "mevent_plugin.h"
#include "mevent_member.h"
#include "mevent_aux.h"

#define PLUGIN_NAME    "aux"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

static unsigned int m_memory_maxid = 0;

struct aux_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct aux_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct aux_stats st;
};

/*
 * ids=0:1
 * { "0": { "1": { "ntt": "0", "nst": "0" } }, "success": "1" }
 *
 * ids=0:1,0:20
 * { "0": { "1": { "ntt": "0", "nst": "0" }, "20": { "ntt": "0", "nst": "0" } }, "success": "1" }
 *
 * ids=0:1,1:20
 * { "0": { "1": { "ntt": "0", "nst": "0" } }, "1": { "20": { "ntt": "0", "nst": "0" } }, "success": "1" } 
 */
static NEOERR* aux_cmd_cmtget(struct aux_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    int count, offset;
    char *ids, *idsdump, tok[128];
    int type = -1, oid = -1;
    NEOERR *err;

    REQ_GET_PARAM_STR(q->hdfrcv, "ids", ids);

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    mdb_pagediv(q->hdfrcv, NULL, &count, &offset, NULL, q->hdfsnd);

    if (cache_getf(cd, &val, &vsize, PREFIX_COMMENT"%s_%d", ids, offset)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        idsdump = strdup(ids);
        char *p = ids;
        while (*p) {
            if (*p == ':') {
                *p = '\0';
                type = atoi(ids);
                ids = p+1;
            }
            if (*p == ',') {
                *p = '\0';
                oid = atoi(ids);
                if (type >= 0 && oid >= 0) {
                    sprintf(tok, "%d.%d", type, oid);
                    MDB_PAGEDIV_SET(q->hdfsnd, tok, db, "comment",
                                    "type=%d AND statu=%d AND oid=%d",
                                      NULL, type, CMT_ST_NORMAL, oid);
                    MDB_QUERY_RAW(db, "comment", _COL_CMT,
                                  "type=%d AND statu=%d AND oid=%d "
                                  " ORDER BY intime DESC LIMIT %d OFFSET %d",
                                  NULL, type, CMT_ST_NORMAL, oid, count, offset);
                    sprintf(tok, "%d.%d.cmts", type, oid);
                    err = mdb_set_rows(q->hdfsnd, db, _COL_CMT, tok,
                                       NULL, MDB_FLAG_EMPTY_OK);
                    if (err != STATUS_OK) return nerr_pass(err);
                    mstr_html_escape(hdf_get_child(q->hdfsnd, tok), "content");
                    type = oid = -1;
                }
                ids = p+1;
            }
            p++;
        }
        oid = atoi(ids);
        if (type >= 0 && oid >=0) {
            sprintf(tok, "%d.%d", type, oid);
            MDB_PAGEDIV_SET(q->hdfsnd, tok, db, "comment",
                            "type=%d AND statu=%d AND oid=%d",
                            NULL, type, CMT_ST_NORMAL, oid);
            MDB_QUERY_RAW(db, "comment", _COL_CMT,
                          "type=%d AND statu=%d AND oid=%d "
                          " ORDER BY intime DESC LIMIT %d OFFSET %d",
                          NULL, type, CMT_ST_NORMAL, oid, count, offset);
            sprintf(tok, "%d.%d.cmts", type, oid);
            err = mdb_set_rows(q->hdfsnd, db, _COL_CMT, tok,
                               NULL, MDB_FLAG_EMPTY_OK);
            if (err != STATUS_OK) return nerr_pass(err);
            mstr_html_escape(hdf_get_child(q->hdfsnd, tok), "content");
        }
        
        CACHE_HDF(q->hdfsnd, CMT_CC_SEC, PREFIX_COMMENT"%s_%d", idsdump, offset);
        free(idsdump);
    }

    return STATUS_OK;
}

static NEOERR* aux_cmd_cmtadd(struct aux_entry *e, QueueEntry *q)
{
	STRING str; string_init(&str);
    int type, oid;
    NEOERR *err;

    REQ_GET_PARAM_INT(q->hdfrcv, "type", type);
    REQ_GET_PARAM_INT(q->hdfrcv, "oid", oid);

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    err = mdb_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.comment"),
                          &str);
	if (err != STATUS_OK) return nerr_pass(err);
    
    MDB_EXEC(db, NULL, "INSERT INTO comment %s", NULL, str.buf);
    
    string_clear(&str);
    
    cache_delf(cd, PREFIX_COMMENT"%d:%d_0", type, oid);
    
    return STATUS_OK;
}

static NEOERR* aux_cmd_cmtdel(struct aux_entry *e, QueueEntry *q)
{
    int id;
    NEOERR *err;

    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);

    mdb_conn *db = e->db;

    MDB_EXEC(db, NULL, "UPDATE comment SET statu=%d WHERE id=%d;",
             NULL, CMT_ST_DEL, id);
    
    return STATUS_OK;
}

static NEOERR* aux_cmd_memoryget(struct aux_entry *e, QueueEntry *q)
{
	unsigned char *val = NULL; size_t vsize = 0;
	NEOERR *err;
    int id = 0;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    REQ_FETCH_PARAM_INT(q->hdfrcv, "id", id);

    if (m_memory_maxid < 1) return nerr_raise(NERR_ASSERT, "empty memory");

    /*
     * neo_rand() may get 0, so, +1
     */
    if (id == 0) id = neo_rand(m_memory_maxid) + 1;

    if (cache_getf(cd, &val, &vsize, PREFIX_MEMORY"%d", id)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_QUERY_RAW(db, "memory", _COL_MEMORY,
                      "id<=%d AND statu=%d ORDER BY id DESC LIMIT 1",
                      NULL, id, MEMORY_ST_OK);
        err = mdb_set_row(q->hdfsnd, db, _COL_MEMORY, NULL, MDB_FLAG_Z);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, MEMORY_CC_SEC, PREFIX_MEMORY"%d", id);
    }
    
    return STATUS_OK;
}

static NEOERR* aux_cmd_memoryadd(struct aux_entry *e, QueueEntry *q)
{
	STRING str; string_init(&str);
	NEOERR *err;
    
    mdb_conn *db = e->db;

    err = mdb_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.memory"), &str);
	if (err != STATUS_OK) return nerr_pass(err);
    
    MDB_EXEC(db, NULL, "INSERT INTO memory %s", NULL, str.buf);

    string_clear(&str);

    m_memory_maxid++;

    return STATUS_OK;
}

static NEOERR* aux_cmd_memorymod(struct aux_entry *e, QueueEntry *q)
{
	STRING str; string_init(&str);
    int id;
	NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);
    
    err = mdb_build_upcol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".UpdateCol.memory"), &str);
	if (err != STATUS_OK) return nerr_pass(err);

    MDB_EXEC(db, NULL, "UPDATE memory SET %s WHERE id=%d;", NULL, str.buf, id);

    string_clear(&str);

    cache_delf(cd, PREFIX_MEMORY"%d", id);

    return STATUS_OK;
}

static NEOERR* aux_cmd_emailadd(struct aux_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    char sum[LEN_MD5], *content;
    NEOERR *err;
    mdb_conn *db = e->db;

    REQ_GET_PARAM_STR(q->hdfrcv, "content", content);
    
    mstr_md5_str(content, sum);
    hdf_set_value(q->hdfrcv, "checksum", sum);

    HDF *node = hdf_get_child(q->hdfrcv, "mto");

insert:
    if (node) hdf_set_value(q->hdfrcv, "to", hdf_obj_value(node));
    
    err = mdb_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.email"),
                          &str);
    if (err != STATUS_OK) return nerr_pass(err);
    
    MDB_EXEC(db, NULL, "INSERT INTO email %s", NULL, str.buf);
    string_clear(&str);

    if (node) {
        node = hdf_obj_next(node);
        if (node) goto insert;
    }

    return STATUS_OK;
}

static NEOERR* aux_cmd_inboxget(struct aux_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    int npp, nst;
    int mid, type = 0;
    NEOERR *err;

    struct cache *cd = e->cd;
    mdb_conn *db = e->db;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    REQ_FETCH_PARAM_INT(q->hdfrcv, "type", type);

    mdb_pagediv(q->hdfrcv, NULL, &count, &offset, NULL, q->hdfsnd);

    if (cache_getf(cd, &val, &vsize, PREFIX_INBOX"%d_%d_%d", mid, type, offset)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_PAGEDIV_SET_N(q->hdfsnd, db, "inbox", "type=%d AND statu=%d AND mid=%d",
                          NULL, type, INBOX_ST_NORMAL, mid);
        MDB_QUERY_RAW(db, "inbox", _COL_INBOX, "type=%d AND statu=%d AND mid=%d "
                      " ORDER BY intime DESC LIMIT %d OFFSET %d",
                      NULL, type, INBOX_ST_NORMAL, mid, count, offset);
        err = mdb_set_rows(q->hdfsnd, db, _COL_INBOX, "inbox",
                           NULL, MDB_FLAG_EMPTY_OK);
        if (err != STATUS_OK) return nerr_pass(err);

        /*
         * update fresh
         */
        HDF *node = hdf_get_child(q->hdfsnd, "inbox");
        int fresh, id;
        bool uped = false;
        while (node) {
            id = hdf_get_int_value(node, "id", 0);
            fresh = hdf_get_int_value(node, "fresh", 0);
            if (fresh && id) {
                uped = true;
                MDB_EXEC(db, NULL, "UPDATE inbox SET fresh=0 WHERE id=%d", NULL, id);
            }
            node = hdf_obj_next(node);
        }

        if (!uped) {
            CACHE_HDF(q->hdfsnd, INBOX_CC_SEC, PREFIX_INBOX"%d_%d_%d",
                      mid, type, offset);
        }
    }

    return STATUS_OK;
}

static NEOERR* aux_cmd_inboxadd(struct aux_entry *e, QueueEntry *q)
{
    STRING str; string_init(&str);
    struct cache *cd = e->cd;
    mdb_conn *db = e->db;
    int type = 0;
    NEOERR *err;

    HDF *node = hdf_get_child(q->hdfrcv, "mmid");

insert:
    if (node) hdf_set_value(q->hdfrcv, "mid", hdf_obj_value(node));

    REQ_FETCH_PARAM_INT(q->hdfrcv, "type", type);
    
    err = mdb_build_incol(q->hdfrcv,
                          hdf_get_obj(g_cfg, CONFIG_PATH".InsertCol.inbox"),
                          &str);
    if (err != STATUS_OK) return nerr_pass(err);
    
    MDB_EXEC(db, NULL, "INSERT INTO inbox %s", NULL, str.buf);
    string_clear(&str);

    cache_delf(cd, PREFIX_INBOX"%d_%d_0",
               hdf_get_int_value(q->hdfrcv, "mid", 0), type);

    if (node) {
        node = hdf_obj_next(node);
        if (node) goto insert;
    }

    return STATUS_OK;
}

static NEOERR* aux_cmd_inboxdel(struct aux_entry *e, QueueEntry *q)
{
    int id, mid, type;
    NEOERR *err;

    MEMBER_GET_PARAM_MID(q->hdfrcv, mid);
    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);
    REQ_GET_PARAM_INT(q->hdfrcv, "type", type);

    struct cache *cd = e->cd;
    mdb_conn *db = e->db;

    MDB_EXEC(db, NULL, "UPDATE inbox SET statu=%d WHERE id=%d "
             " AND mid=%d AND type=%d",
             NULL, INBOX_ST_DEL, id, mid, type);
    
    cache_delf(cd, PREFIX_INBOX"%d_%d_0", mid, type);
    
    return STATUS_OK;
}

static void aux_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct aux_entry *e = (struct aux_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct aux_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_CMT_GET:
        err = aux_cmd_cmtget(e, q);
        break;
    case REQ_CMD_CMT_ADD:
        err = aux_cmd_cmtadd(e, q);
        break;
    case REQ_CMD_CMT_DEL:
        err = aux_cmd_cmtdel(e, q);
        break;
    case REQ_CMD_MEMORY_GET:
        err = aux_cmd_memoryget(e, q);
        break;
    case REQ_CMD_MEMORY_ADD:
        err = aux_cmd_memoryadd(e, q);
        break;
    case REQ_CMD_MEMORY_MOD:
        err = aux_cmd_memorymod(e, q);
        break;
    case REQ_CMD_AUX_EMAIL_ADD:
        err = aux_cmd_emailadd(e, q);
        break;
    case REQ_CMD_AUX_INBOX_GET:
        err = aux_cmd_inboxget(e, q);
        break;
    case REQ_CMD_AUX_INBOX_ADD:
        err = aux_cmd_inboxadd(e, q);
        break;
    case REQ_CMD_AUX_INBOX_DEL:
        err = aux_cmd_inboxdel(e, q);
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

static void aux_stop_driver(EventEntry *entry)
{
    struct aux_entry *e = (struct aux_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
}



static EventEntry* aux_init_driver(void)
{
    struct aux_entry *e = calloc(1, sizeof(struct aux_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = aux_process_driver;
    e->base.stop_driver = aux_stop_driver;
    //mevent_add_timer(&e->base.timers, 60, true, hint_timer_up_term);

    char *s = hdf_get_value(g_cfg, CONFIG_PATH".dbsn", NULL);
    err = mdb_init(&e->db, s);
    JUMP_NOK(err, error);
    
    e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
    if (e->cd == NULL) {
        wlog("init cache failure");
        goto error;
    }

    err = mdb_exec(e->db, NULL, "SELECT id from memory ORDER BY id DESC LIMIT 1", NULL);
    JUMP_NOK(err, error);
    err = mdb_get(e->db, "i", &m_memory_maxid);
    if (nerr_handle(&err, NERR_NOT_FOUND)) {
        mtc_err("table memory empty");
        wlog("table memory empty");
    }
    JUMP_NOK(err, error);
    
    return (EventEntry*)e;
    
error:
    if (e->base.name) free(e->base.name);
    if (e->db) mdb_destroy(e->db);
    if (e->cd) cache_free(e->cd);
    free(e);
    return NULL;
}

struct event_driver aux_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = aux_init_driver,
};
