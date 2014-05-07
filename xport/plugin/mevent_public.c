#include "mevent_plugin.h"
#include "mevent_public.h"
#include "mevent_plan.h"

#define PLUGIN_NAME    "public"
#define CONFIG_PATH    PRE_PLUGIN"."PLUGIN_NAME

struct public_stats {
    unsigned long msg_total;
    unsigned long msg_unrec;
    unsigned long msg_badparam;
    unsigned long msg_stats;

    unsigned long proc_suc;
    unsigned long proc_fai;
};

struct public_entry {
    EventEntry base;
    mdb_conn *db;
    Cache *cd;
    struct public_stats st;
};

time_t m_thatsec = 0;

static int plan_compare(const void *a, const void *b)
{
    HDF *ha = *(HDF**)a, *hb = *(HDF**)b;
    unsigned int seca = mcs_get_uint_value(ha, "epochsec", 0);
    unsigned int secb = mcs_get_uint_value(hb, "epochsec", 0);

    return abs(seca - m_thatsec) - abs(secb - m_thatsec);
}

static void plan_prepare_time(HDF *node, char *date, struct tm *todaystm, float km)
{
    if (!node || !date || !todaystm) return;

    char datetime[LEN_TM] = {0}, *stime, *sdate;
    time_t tm;

    /*
     * epochsec seted, so, return
     */
    if (hdf_get_value(node, "epochsec", NULL)) return;
    
    stime = hdf_get_value(node, "stime", "08:00:00");
    sdate = hdf_get_value(node, "sdate", "2011-11-11");
    
    int repeat = hdf_get_int_value(node, "repeat", PLAN_RPT_NONE);
    if (repeat == PLAN_RPT_DAY) {
        /*
         * use date as datepart
         */
        snprintf(datetime, LEN_TM, "%s %s", date, stime);
    } else if (repeat == PLAN_RPT_WEEK) {
        /*
         * use the nearest date as datepart
         */
        int minday = 7, thatday;
        /* 1,2,3,4,5 */
        if (sdate) {
            int today = todaystm->tm_wday + 1;
            ULIST *list;
            
            string_array_split(&list, sdate, ",", 100);
            ITERATE_MLIST(list) {
                thatday = atoi(list->items[t_rsv_i]);
                if (abs(minday) > abs(thatday - today))
                    minday = thatday - today;
            }
            uListDestroy(&list, ULIST_FREE);
        }
        tm = m_thatsec + (minday*60*60*24);
        struct tm *stm = localtime(&tm);
        char s[LEN_DT];
        strftime(s, LEN_DT, "%Y-%m-%d", stm);
        snprintf(datetime, LEN_TM, "%s %s", s, stime);
    } else {
        /*
         * use sdate as datepart
         */
        snprintf(datetime, LEN_TM, "%s %s", sdate, stime);
    }

    struct tm thatdaystm;
    strptime(datetime, "%Y-%m-%d %H:%M:%S", &thatdaystm);
    tm = mktime(&thatdaystm);

    if (km != 0.0) {
        float mkm, delta;
        mkm = mcs_get_float_value(node, "km", 0.0);
        delta = abs(km - mkm);

        tm = m_thatsec + (tm - m_thatsec) * ((delta / km) + 0.01);
    }

    hdf_set_value(node, "datetime", datetime);
    mcs_set_uint_value(node, "epochsec", tm);
}

time_t pub_plan_get_abssec(char *pdate, char *ptime)
{
    char datetime[LEN_TM] = {0};
    struct tm thatdaystm;
    
    if (!pdate || !ptime) return 0;

    snprintf(datetime, LEN_TM, "%s %s", pdate, ptime);
    strptime(datetime, "%Y-%m-%d %H:%M:%S", &thatdaystm);

    return mktime(&thatdaystm);
}

time_t pub_plan_get_relsec(HDF *node, time_t thatsec)
{
    if (!node) return thatsec;

    time_t todaysec = mcs_get_uint_value(node, "epochsec", 0);

    return abs(todaysec - thatsec);
}

NEOERR* pub_plan_sort_by_time(HDF *node, float km, time_t thatsec, char *pdate)
{
    struct tm *todaystm;
    time_t todaysec;

    MCS_NOT_NULLB(node, pdate);

    todaysec = time(NULL);
    todaystm = localtime(&todaysec);

    m_thatsec = thatsec;

    HDF *obj = hdf_obj_child(node);
    while (obj) {
        plan_prepare_time(obj, pdate, todaystm, km);

        obj = hdf_obj_next(obj);
    }

    /*
     * sort plan
     */
    return hdf_sort_obj(node, plan_compare);
}

static void public_process_driver(EventEntry *entry, QueueEntry *q)
{
    struct public_entry *e = (struct public_entry*)entry;
    NEOERR *err;
    int ret;
    
    struct public_stats *st = &(e->st);

    st->msg_total++;
    
    mtc_dbg("process cmd %u", q->operation);
    switch (q->operation) {
        CASE_SYS_CMD(q->operation, q, e->cd, err);
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

static void public_stop_driver(EventEntry *entry)
{
    //struct public_entry *e = (struct public_entry*)entry;

    /*
     * e->base.name, e->base will free by mevent_stop_driver() 
     */
}



static EventEntry* public_init_driver(void)
{
    struct public_entry *e = calloc(1, sizeof(struct public_entry));
    if (e == NULL) return NULL;

    e->base.name = (unsigned char*)strdup(PLUGIN_NAME);
    e->base.ksize = strlen(PLUGIN_NAME);
    e->base.process_driver = public_process_driver;
    e->base.stop_driver = public_stop_driver;
    //mevent_add_timer(&e->base.timers, 60, true, hint_timer_up_term);

    return (EventEntry*)e;
}

struct event_driver public_driver = {
    .name = (unsigned char*)PLUGIN_NAME,
    .init_driver = public_init_driver,
};
