#include "mheads.h"
#include "lheads.h"
#include "oplan.h"

static char *plan_rand_keyword()
{
    int tt = hdf_get_int_value(g_cfg, "Config.Keywords.plannum", 0);

    int cnt = neo_rand(tt);

    return hdf_get_valuef(g_cfg, "Config.Keywords.plan.%d", cnt);
}

NEOERR* plan_of_recent(HDF *hdf, HASH *dbh, HASH *evth)
{
    mevent_t *evt = hash_lookup(evth, "plan");

    MCS_NOT_NULLB(hdf, evt);

    hdf_copy(evt->hdfsnd, NULL, hdf);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_RECENT, FLAGS_SYNC);

    hdf_copy(hdf, PRE_OUTPUT".plans", evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* plan_match_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan");
    
    if (!cgi || !cgi->hdf || !evt) return nerr_raise(NERR_ASSERT, "paramter null");

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_MATCH, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    hdf_set_attr(cgi->hdf, PRE_OUTPUT".plans", "type", "array");

    /*
     * trace search event
     */
    char *s;
    HDF_FETCH_STR(evt->hdfrcv, "_ntt", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.ei_one", s);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".scityid", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.ei_two", s);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".ecityid", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.ei_three", s);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".saddr", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_one", s);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".eaddr", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_two", s);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".date", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_three", s);

    HDF *node = hdf_get_obj(cgi->hdf, PRE_RESERVE".event");
    
    return nerr_pass(trace_event(node, evth, ses, TRACE_TYPE_PLAN_SEARCH));
}

NEOERR* plan_leave_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt;
    char *mname = NULL, *email = NULL;
    HDF *plan;
    int id, expect;
    NEOERR *err;
    
    if (!cgi || !cgi->hdf) return nerr_raise(NERR_ASSERT, "paramter null");

    HDF_GET_OBJ(cgi->hdf, PRE_QUERY".plan", plan);
    
    evt = hash_lookup(evth, "plan");
    if (!evt) return nerr_raise(NERR_ASSERT, "plan null");

    hdf_copy(evt->hdfsnd, NULL, plan);

    err = member_check_login_data_get(cgi, dbh, evth, ses);
	if (err == STATUS_OK) {
        mname = hdf_get_value(cgi->hdf, PRE_RESERVE".mname", NULL);
        hdf_set_value(evt->hdfsnd, "mname", mname);
    } else {
        hdf_set_value(evt->hdfsnd, "mid", "0");
        nerr_ignore(&err);
        mname = NULL;
    }

    /*
     * add plan
     */
    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_ADD, FLAGS_SYNC);
    id = hdf_get_int_value(evt->hdfrcv, "id", 0);
    if (id <= 0) return nerr_raise(NERR_ASSERT, "添加路线失败");
    hdf_set_int_value(plan, "id", id);

    /*
     * add subscribe
     */
    expect = hdf_get_int_value(plan, "subscribe", FFT_EXPECT_NONE);
    if (expect != FFT_EXPECT_NONE) {
        if (!mname) return nerr_raise(LERR_NOTLOGIN, "not login");

        evt = hash_lookup(evth, "fft");
        MCS_NOT_NULLA(evt);

        /* TODO need to do this way??? */
        if (expect & FFT_EXPECT_PHONE) {
            hdf_copy(evt->hdfsnd, NULL, plan);
            hdf_set_value(evt->hdfsnd, "mname", mname);
            hdf_set_int_value(evt->hdfsnd, "pid", id);
            
            hdf_set_int_value(evt->hdfsnd, "addrtype", FFT_EXPECT_PHONE);
            MEVENT_TRIGGER(evt, NULL, REQ_CMD_FFT_EXPECT_ADD, FLAGS_NONE);
        }
        if (expect & FFT_EXPECT_EMAIL) {
            /* use email which user supplied on /plan/leave */
            HDF_GET_STR(plan, "contact", email);
            
            hdf_copy(evt->hdfsnd, NULL, plan);
            hdf_set_value(evt->hdfsnd, "mname", email);
            hdf_set_int_value(evt->hdfsnd, "pid", id);
            
            hdf_set_int_value(evt->hdfsnd, "addrtype", FFT_EXPECT_EMAIL);
            MEVENT_TRIGGER(evt, NULL, REQ_CMD_FFT_EXPECT_ADD, FLAGS_NONE);
        }
    }

    /*
     * meet expect
     */
    evt = hash_lookup(evth, "fft");
    MCS_NOT_NULLA(evt);

    hdf_copy(evt->hdfsnd, NULL, plan);
    hdf_set_int_value(evt->hdfsnd, "id", id);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_FFT_EXPECT_MATCH, FLAGS_SYNC);

    /*
     * these man need notify them immediately
     */
    HDF *obj = hdf_get_obj(evt->hdfrcv, "expects");
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

        expect += 100;
    }
    
    /*
     * trace leave event
     */
    /*
     * subscribe + meet ? 100 : 0
     */
    hdf_set_int_value(cgi->hdf, PRE_RESERVE".event.ei_one", expect);

    char *s;
    HDF_FETCH_STR(plan, "scityid", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.ei_two", s);
    HDF_FETCH_STR(plan, "ecityid", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.ei_three", s);

    HDF_FETCH_STR(plan, "saddr", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_one", s);
    HDF_FETCH_STR(plan, "eaddr", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_two", s);
    HDF_FETCH_STR(plan, "sdate", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_three", s);

    HDF *node = hdf_get_obj(cgi->hdf, PRE_RESERVE".event");
    
    return nerr_pass(trace_event(node, evth, ses, TRACE_TYPE_PLAN_LEAVE));
}

NEOERR* plan_info_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan");

    MCS_NOT_NULLB(cgi, evt);

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_GET, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT".plan", evt->hdfrcv);

    hdf_set_valuef(cgi->hdf, PRE_CFG_LAYOUT".keywords=%s到%s拼车,拼车去%s,%s",
                   hdf_get_value(evt->hdfrcv, "saddr", "地球"),
                   hdf_get_value(evt->hdfrcv, "eaddr", "火星"),
                   hdf_get_value(evt->hdfrcv, "eaddr", "火星"),
                   plan_rand_keyword());
    
    hdf_set_valuef(cgi->hdf, PRE_CFG_LAYOUT".title=%s至%s顺风车",
                   hdf_get_value(evt->hdfrcv, "saddr", "地球"),
                   hdf_get_value(evt->hdfrcv, "eaddr", "火星"));

    hdf_set_valuef(cgi->hdf, PRE_CFG_LAYOUT".description=%s %s至%s顺风车拼车",
                   hdf_get_value(evt->hdfrcv, "sdate", "每天"),
                   hdf_get_value(evt->hdfrcv, "saddr", "地球"),
                   hdf_get_value(evt->hdfrcv, "eaddr", "火星"));
    
    return STATUS_OK;
}

NEOERR* plan_pic_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan");
    HDF *node;
    char *s = NULL, *defs = NULL;
    NEOERR *err;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".defs", defs);

    node = hdf_get_child(cgi->hdf, PRE_QUERY".type");
    if (!node) HDF_GET_STR(cgi->hdf, PRE_QUERY".type", s);
    
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_PRIV_GET, FLAGS_SYNC);

    if (s) goto getval;
    while (node) {
        s = hdf_obj_value(node);
    getval:
        /*
         * turn &amp; into & in url format
         */
        mstr_html_unescape(evt->hdfrcv, s);
        s = hdf_get_value(evt->hdfrcv, s, NULL);
        if (s && *s) {
            if (!strncmp(s, "http:", 5)) {
                hdf_set_value(cgi->hdf, PRE_OUTPUT".302", s);
                return STATUS_OK;
            }
            break;
        }
        
        node = hdf_obj_next(node);
    }

    if (!s || !*s) {
        if (!defs) s = SITE_DOMAIN;
        else if (!strcmp(defs, "segv")) return STATUS_OK;
        else s = defs;
    }
    
    err = mimg_create_from_string(s,
                                  hdf_get_value(g_cfg,
                                                "Config.font.plan.path",
                                                "/usr/share/ttf/Times.ttf"),
                                  atof(hdf_get_value(g_cfg,
                                                     "Config.font.plan.size",
                                                     "14.")),
                                  &ses->data);
    if (err != STATUS_OK) return nerr_pass(err);
    
    return STATUS_OK;
}

NEOERR* plan_mine_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan");
    char *mname;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);

    MEMBER_CHECK_LOGIN();
    SET_DASHBOARD_ACTION(cgi->hdf);

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    hdf_set_value(evt->hdfsnd, "_npp", "5");

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_MINE, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    
    return STATUS_OK;
}

NEOERR* plan_mine_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "plan"), *fevt = hash_lookup(evth, "fft");
    char *mname;
    bool caomin = false;
    NEOERR *err;

    MCS_NOT_NULLC(cgi->hdf, evt, fevt);

    MEMBER_CHECK_LOGIN();

    mevent_t *tevt = hash_lookup(evth, "member");
    if (hdf_get_int_value(tevt->hdfrcv, "verify", -1) < MEMBER_VF_ADMIN) {
        caomin = true;
        hdf_set_value(evt->hdfsnd, "_caomin", "1");
    }

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    hdf_set_value(evt->hdfsnd, "mname", mname);

    int pid = hdf_get_int_value(cgi->hdf, PRE_QUERY".id", 0);
    int sub = hdf_get_int_value(cgi->hdf, PRE_QUERY".subscribe", 0);
    int op = hdf_get_int_value(cgi->hdf, PRE_QUERY".checkop", 0);

    hdf_set_value(fevt->hdfsnd, "mname", mname);
    hdf_set_int_value(fevt->hdfsnd, "pid", pid);
    if (caomin) hdf_set_value(fevt->hdfsnd, "_caomin", "1");

    if (sub && op == 1) {
        MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_UP, FLAGS_SYNC);

        /*
         * enable some subscribe, so, we need insert/update expect
         */
        hdf_copy(fevt->hdfsnd, NULL, evt->hdfrcv);
        hdf_set_int_value(fevt->hdfsnd, "addrtype", sub);
        MEVENT_TRIGGER(fevt, NULL, REQ_CMD_FFT_EXPECT_ADD, FLAGS_NONE);
    } else {
        MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_UP, FLAGS_NONE);
        
        /*
         * normarl modify, up plan async
         */
        if (sub) {
            /*
             * remove subscribe
             */
            hdf_copy(fevt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
            hdf_set_int_value(fevt->hdfsnd, "addrtype", sub);
            hdf_set_int_value(fevt->hdfsnd, "statu", FFT_EXPECT_STATU_DEL);
            MEVENT_TRIGGER(fevt, NULL, REQ_CMD_FFT_EXPECT_UP, FLAGS_NONE);
        }
    }

    char *s;
    hdf_set_int_value(cgi->hdf, PRE_RESERVE".event.ei_one", pid);
    hdf_set_int_value(cgi->hdf, PRE_RESERVE".event.ei_two", sub);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".statu", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.ei_three", s);

    HDF *node = hdf_get_obj(cgi->hdf, PRE_RESERVE".event");

    return nerr_pass(trace_event(node, evth, ses, TRACE_TYPE_PLAN_MODIFY));
}
