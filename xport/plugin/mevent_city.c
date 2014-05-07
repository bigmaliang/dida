#include "mevent_plugin.h"
#include "mevent_city.h"

#define PLUGIN_NAME    "city"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

static unsigned char *ips = NULL;
static unsigned int ipbgn, ipend;
static iconv_t cv = NULL;

struct city_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct city_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct city_stats st;
};

/*
 * ip2place part
 */
static char* gb2utf8(char *s)
{
    if (!s || !*s) return NULL;

    if (!cv || cv == (iconv_t)-1) cv = iconv_open("UTF-8", "GB2312");
    if (cv == (iconv_t)-1) {
        mtc_err("init conv error %s", strerror(errno));
        return NULL;
    }

    size_t len = strlen(s), ulen;
    ulen = len*2;
    char *utf8 = calloc(1, ulen);
    char *us = utf8;
    
    if (iconv (cv, &s, &len, &utf8, &ulen) == -1) {
        mtc_err("conv error");
        free(us);
        return NULL;
    }
    
    //iconv_close(cv);
    return us;
}

static unsigned int b2int(unsigned char *p, int count)
{
    int i;
    unsigned int ret;

    if(count < 1 || count > 4) 
        return 0;
    
    ret = p[0];
    for (i = 0; i < count; i++)
        ret |= ((unsigned int)p[i])<<(8*i);
    
    return ret;
}

static unsigned char* readarea(int offset)
{
    if (!ips) return NULL;
    
    unsigned char *p = ips + offset;
    unsigned char mode = *p;
    
    if (mode == REDIRECT_MODE_1 || mode == REDIRECT_MODE_2) {
        offset = b2int(p+1, 3);
        if (offset == 0) {
            return NULL;
        }
    }
    return ips + offset;
}

static void ip_place(int offset, char **c, char **a)
{
    if (!ips) return;
    
    unsigned char *p = ips + offset + 4;
    unsigned char mode = *p;

    if (mode == REDIRECT_MODE_1) {
        offset = b2int(p+1, 3);
        p = ips + offset;
        mode = *p;
        if (mode == REDIRECT_MODE_2) {
            *c = (char*)(ips + b2int(p+1, 3));
            *a = (char*)readarea(offset+4);
        } else {
            *c = (char*)(ips + offset);
            *a = (char*)readarea(offset + strlen(*c) + 1);
        }
    } else if (mode == REDIRECT_MODE_2) {
        offset = b2int(p+1, 3);
        *c = (char*)(ips + offset);
        *a = (char*)readarea(offset+4+4);
    } else {
        *c = (char*)(ips + offset + 4);
        *a = (char*)(readarea(offset + 4 + strlen(*c) + 1));
    }
    *c = gb2utf8(*c);
    *a = gb2utf8(*a);
}

static int ip_offset(unsigned int ip)
{
    unsigned int ipb, ipe;
    unsigned int M, L, R, record_count;

    record_count = (ipend - ipbgn)/7+1;
    /* search for right range */
    L = 0;
    R = record_count - 1;
    while (L < R-1) {
        M = (L + R) / 2;
        ipb = b2int(ips + ipbgn + M*7, 4);
        ipe = b2int(ips + b2int(ips + ipbgn + M*7 + 4, 3), 4);

        if (ip == ipb) {
            L = M;
            break;
        }
        if (ip > ipb)
            L = M;
        else
            R = M;
    }

    ipb = b2int(ips + ipbgn + L*7, 4);
    ipe = b2int(ips + b2int(ips + ipbgn + L*7 + 4, 3), 4);

    /* version infomation, the last item */
    if((ip & 0xffffff00) == 0xffffff00) {
        ipb = b2int(ips + ipbgn + R*7, 4);
        ipe = b2int(ips + b2int(ips + ipbgn + R*7 + 4, 3), 4);
    }
    
    if (ipb <= ip && ip <= ipe)
        return b2int(ips + ipbgn + L*7 + 4, 3);
    else
        return -1;
}

static bool ip2place(HDF *hdf, char *ip, char *key)
{
    if (!hdf || !ip) return false;
    
    unsigned int dip, offset;
    char *c, *a;

    dip= inet_addr(ip);
    
    if (dip != INADDR_NONE) {
        dip = ntohl(dip);
        offset = ip_offset(dip);
        if (offset >= 0) {
            ip_place(offset, &c, &a);
            if (c && a) {
                if (key) {
                    hdf_set_valuef(hdf, "%s.ip=%s", key, ip);
                    hdf_set_valuef(hdf, "%s.c=%s", key, c);
                    hdf_set_valuef(hdf, "%s.a=%s", key, a);
                } else {
                    hdf_set_valuef(hdf, "ip=%s", ip);
                    hdf_set_valuef(hdf, "c=%s", c);
                    hdf_set_valuef(hdf, "a=%s", a);
                }
                free(c); c = NULL;
                free(a); a = NULL;
                return true;
            }
        }
    }
    return false;
}

static NEOERR* city_cmd_s(struct city_entry *e, QueueEntry *q)
{
	unsigned char *val = NULL; size_t vsize = 0;
    char *p = NULL, *c;
	NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    REQ_GET_PARAM_STR(q->hdfrcv, "c", c);
    REQ_FETCH_PARAM_STR(q->hdfrcv, "p", p);

    if (cache_getf(cd, &val, &vsize, PREFIX_CITY"%s.%s", p, c)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        if (p) {
            char tok[64] = {0};
            strncpy(tok, p, 64);
            if (!strstr(p, "省") && !strstr(p, "区"))
                snprintf(tok, sizeof(tok), "%s省", p);

            MDB_QUERY_RAW(db, "city", _COL_CITY, "s=$1", "s", tok);
            mdb_set_row(q->hdfsnd, db, _COL_CITY, "province", MDB_FLAG_NO_ERR);
        }

        MDB_QUERY_RAW(db, "city", _COL_CITY, "s=$1", "s", c);
        err = mdb_set_rows(q->hdfsnd, db, _COL_CITY, "citys", NULL, MDB_FLAG_Z);
        if (nerr_handle(&err, NERR_NOT_FOUND)) {
            if (strstr(c, "市")) c[strlen(c)-3] = '\0';
            MDB_QUERY_RAW(db, "city", _COL_CITY, "s=$1", "s", c);
            err = mdb_set_rows(q->hdfsnd, db, _COL_CITY, "citys",
                               NULL, MDB_FLAG_EMPTY_OK);
        }
        if (err != STATUS_OK) return nerr_pass(err);

        /*
         * get city's parents
         */
        int cnt = 0;
        char *pid = hdf_get_valuef(q->hdfsnd, "citys.0.pid");
        while (pid && atoi(pid) > 0) {
            MDB_QUERY_RAW(db, "city", _COL_CITY, "id=%d", NULL, atoi(pid));
            err = mdb_set_rows(q->hdfsnd, db, _COL_CITY, "citys", NULL, MDB_FLAG_Z);
            TRACE_NOK(err);
            pid = hdf_get_valuef(q->hdfsnd, "citys.%d.pid", ++cnt);
        }
        
        /*
         * 北京市 has no subcities
         */
        /*
        int id = hdf_get_int_value(q->hdfsnd, "city.id", 0);
        MDB_QUERY_RAW(db, "city", _COL_CITY, "pid=%d", NULL, id);
        err = mdb_set_rows(q->hdfsnd, db, _COL_CITY, "subcities",
        NULL, MDB_FLAG_EMPTY_OK);
        if (err != STATUS_OK) return nerr_pass(err);
        */
        
        CACHE_HDF(q->hdfsnd, CITY_CC_SEC, PREFIX_CITY"%s.%s", p, c);
    }
    
    return STATUS_OK;
}

static NEOERR* city_cmd_ip(struct city_entry *e, QueueEntry *q)
{
	unsigned char *val = NULL; size_t vsize = 0;
    char *ip, *c, *a, *s;
	NEOERR *err;

    struct cache *cd = e->cd;
    
    REQ_GET_PARAM_STR(q->hdfrcv, "ip", ip);

    if (cache_getf(cd, &val, &vsize, PREFIX_CITY"%s", ip)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        if (!ip2place(q->hdfsnd, ip, NULL))
            return nerr_raise(NERR_ASSERT, "%s not ip", ip);
        c = hdf_get_value(q->hdfsnd, "c", NULL);
        a = hdf_get_value(q->hdfsnd, "a", NULL);
        s = strstr(c, "省");
        if (!s) s = strstr(c, "区");
        if (s) {
            s += 3;
            char tok[64] = {0};
            strncpy(tok, c, s-c);
            hdf_set_value(q->hdfrcv, "p", tok);
            hdf_set_value(q->hdfrcv, "c", s);
        } else {
            mtc_err("%s doesn't contain 省/区(%s)", c, a);
            hdf_set_value(q->hdfrcv, "c", c);
        }
        err = city_cmd_s(e, q);
        if (err != STATUS_OK) return nerr_pass(err);

        CACHE_HDF(q->hdfsnd, CITY_CC_SEC, PREFIX_CITY"%s", ip);
    }
    
    return STATUS_OK;
}

static NEOERR* city_cmd_id(struct city_entry *e, QueueEntry *q)
{
	unsigned char *val = NULL; size_t vsize = 0;
    int id;
	NEOERR *err;

    mdb_conn *db = e->db;
    struct cache *cd = e->cd;

    REQ_GET_PARAM_INT(q->hdfrcv, "id", id);

    if (cache_getf(cd, &val, &vsize, PREFIX_CITY"%d", id)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        MDB_QUERY_RAW(db, "city", _COL_CITY, "id=%d", NULL, id);
        err = mdb_set_rows(q->hdfsnd, db, _COL_CITY, "citys", NULL, MDB_FLAG_Z);
        if (err != STATUS_OK) return nerr_pass(err);

        /*
         * get city's parents
         */
        int cnt = 0;
        char *pid = hdf_get_valuef(q->hdfsnd, "citys.0.pid");
        while (pid && atoi(pid) > 0) {
            MDB_QUERY_RAW(db, "city", _COL_CITY, "id=%d", NULL, atoi(pid));
            err = mdb_set_rows(q->hdfsnd, db, _COL_CITY, "citys", NULL, MDB_FLAG_Z);
            TRACE_NOK(err);
            pid = hdf_get_valuef(q->hdfsnd, "citys.%d.pid", ++cnt);
        }
        
        CACHE_HDF(q->hdfsnd, CITY_CC_SEC, PREFIX_CITY"%d", id);
    }
    
    return STATUS_OK;
}

static NEOERR* place_cmd_get(struct city_entry *e, QueueEntry *q)
{
    unsigned char *val = NULL; size_t vsize = 0;
    int count = 0;
    char *ip, tok[64];
    
    REQ_GET_PARAM_STR(q->hdfrcv, "ip", ip);
    
    struct cache *cd = e->cd;

    if (cache_getf(cd, &val, &vsize, PREFIX_PLACE"%s", ip)) {
        unpack_hdf(val, vsize, &q->hdfsnd);
    } else {
        char *s = strdup(ip);
        char *dupip = s, *p = s;
        while (*p != '\0') {
            if (*p == ',') {
                *p = '\0';
                sprintf(tok, "%d", count);
                ip2place(q->hdfsnd, s, tok);
                count++;
                s = p+1;
            }
            p++;
        }
        sprintf(tok, "%d", count);
        ip2place(q->hdfsnd, s, tok);
        free(dupip);

        CACHE_HDF(q->hdfsnd, 0, PREFIX_PLACE"%s", ip);
    }
    
    return STATUS_OK;
}

static void city_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct city_entry *e = (struct city_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct city_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
    case REQ_CMD_CITY_BY_IP:
        err = city_cmd_ip(e, q);
        break;
    case REQ_CMD_CITY_BY_ID:
        err = city_cmd_id(e, q);
        break;
    case REQ_CMD_CITY_BY_S:
        err = city_cmd_s(e, q);
        break;
    case REQ_CMD_PLACE_GET:
        err = place_cmd_get(e, q);
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

static void city_stop_driver(EventEntry *entry)
{
    struct city_entry *e = (struct city_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
    mdb_destroy(e->db);
    cache_free(e->cd);
    if (ips) {
        free(ips);
        ips = NULL;
    }
}



static EventEntry* city_init_driver(void)
{
    struct city_entry *e = calloc(1, sizeof(struct city_entry));
    if (e == NULL) return NULL;
    NEOERR *err;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = city_process_driver;
    e->base.stop_driver = city_stop_driver;
    //mevent_add_timer(&e->base.timers, 60, true, hint_timer_up_term);

    char *s = hdf_get_value(g_cfg, CONFIG_PATH".dbsn", NULL);
    err = mdb_init(&e->db, s);
    JUMP_NOK(err, error);
    
    e->cd = cache_create(hdf_get_int_value(g_cfg, CONFIG_PATH".numobjs", 1024), 0);
    if (e->cd == NULL) {
        wlog("init cache failure");
        goto error;
    }

    s = hdf_get_value(g_cfg, CONFIG_PATH".ipfile", "QQWry.Dat");
    err = ne_load_file(s, (char**)&ips);
    JUMP_NOK(err, error);

    ipbgn = b2int(ips, 4);
    ipend = b2int(ips+4, 4);
    if (ipbgn < 0 || ipend < 0) {
        wlog("%s format error", s);
        goto error;
    }
    
    return (EventEntry*)e;
    
error:
    if (e->base.name) free(e->base.name);
    if (e->db) mdb_destroy(e->db);
    if (e->cd) cache_free(e->cd);
    if (ips) free(ips);
    free(e);
    return NULL;
}

struct event_driver city_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = city_init_driver,
};
