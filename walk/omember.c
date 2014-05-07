#include "mheads.h"
#include "lheads.h"
#include "omember.h"

static void member_after_login(CGI *cgi, HASH *evth,
                               char *mname, char *mnick, char *mid)
{
    char tm[LEN_TM_GMT], *p, mmsn[LEN_CK];
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "member");
    if (!evt) return;

    memset(mmsn, 0x0, sizeof(mmsn));
    mstr_rand_string(mmsn, sizeof(mmsn));
    mutil_getdatetime_gmt(tm, sizeof(tm), "%A, %d-%b-%Y %T GMT", time(NULL) + ONE_WEEK);

    /*
     * set cookie 
     */
    cgi_cookie_set(cgi, "mname", mname, NULL, SITE_DOMAIN, tm, 1, 0);
    cgi_cookie_set(cgi, "mnick", mnick, NULL, SITE_DOMAIN, tm, 1, 0);
    neos_url_escape(mnick, &p, NULL);
    cgi_cookie_set(cgi, "mnick_esc", p, NULL, SITE_DOMAIN, tm, 1, 0);
    free(p);

    //cgi_url_escape(mmsn, &p);
    cgi_cookie_set(cgi, "mmsn", mmsn, NULL, SITE_DOMAIN, tm, 1, 0);
    cgi_cookie_set(cgi, "mid",  mid,  NULL, SITE_DOMAIN, tm, 1, 0);

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_set_value(evt->hdfsnd, "mmsn", mmsn);
    MEVENT_TRIGGER_NRET(evt, mname, REQ_CMD_MEMBER_UP, FLAGS_NONE);

    hdf_set_copy(cgi->hdf, PRE_OUTPUT".mnick", PRE_QUERY".mnick");
    hdf_set_copy(cgi->hdf, PRE_OUTPUT".mname", PRE_QUERY".mname");
    hdf_set_value(cgi->hdf, PRE_OUTPUT".mmsn", mmsn);
    hdf_set_value(cgi->hdf, PRE_OUTPUT".mid", mid);
}

NEOERR* member_info_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname;
    NEOERR *err;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    mname = hdf_get_value(cgi->hdf, PRE_QUERY".mname", NULL);
    if (!mname) {
        MEMBER_CHECK_LOGIN();
        hdf_set_value(evt->hdfsnd, "mname", mname);
        MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMBER_PRIV_GET, FLAGS_SYNC);
    } else {
        hdf_set_value(evt->hdfsnd, "mname", mname);
        MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMBER_GET, FLAGS_SYNC);
    }

    hdf_copy(cgi->hdf, PRE_OUTPUT".member", evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* member_pic_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    HDF *node;
    char *s = NULL, *defs = NULL, *path, *size;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".defs", defs);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".fpath", path);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".fsize", size);

    node = hdf_get_child(cgi->hdf, PRE_QUERY".type");
    if (!node) HDF_GET_STR(cgi->hdf, PRE_QUERY".type", s);
    
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMBER_PRIV_GET, FLAGS_SYNC);

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
        
        if (node) node = hdf_obj_next(node);
    }

    if (!s || !*s) {
        if (!defs) s = SITE_DOMAIN;
        else if (!strcmp(defs, "segv")) return STATUS_OK;
        else s = defs;
    }
    
    if (!path) path = hdf_get_value(g_cfg,
                                    "Config.font.member.path",
                                    "/usr/share/ttf/Times.ttf");
    
    if (!size) size = hdf_get_value(g_cfg, "Config.font.member.size", "14.");
    
    return nerr_pass(mimg_create_from_string(s, path, atof(size), &ses->data));
}

NEOERR* member_edit_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "member");
    char *mname, *msn, *omsn, *rlink;
    NEOERR *err;

    MEMBER_CHECK_LOGIN();
    
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".msn", msn);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".omsn", omsn);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".rlink", rlink);
    if (msn && *msn) {
        if (omsn && *omsn) {
            /*
             * modify msn through msn
             */
            hdf_set_value(evt->hdfsnd, "mname", mname);
            hdf_set_value(evt->hdfsnd, "msn", omsn);
            MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_CHECK_MSN, FLAGS_SYNC);
        } else if (rlink && *rlink) {
            /*
             * modify msn through rlink
             */
            hdf_set_value(evt->hdfsnd, "mname", mname);
            MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_GETRLINK, FLAGS_SYNC);

            char *s = hdf_get_value(evt->hdfrcv, "rlink", NULL);
            if (!s || strcmp(s, rlink))
                return nerr_raise(LERR_USERINPUT, "验证码错误");
        } else return nerr_raise(LERR_USERINPUT, "修改密码需要提供旧密码");
    } else {
        /*
         * modify base information
         */
        hdf_set_value(evt->hdfsnd, "mname", mname);
        hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

        MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_UP, FLAGS_NONE);
    }
    
    return STATUS_OK;
}

NEOERR* member_car_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);
    
    MEMBER_CHECK_LOGIN();

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, mname, REQ_CMD_CAR_UP, FLAGS_NONE);
    
    return STATUS_OK;
}

NEOERR* member_exist_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".mname", mname);

    hdf_set_value(evt->hdfsnd, "mname", mname);

    mevent_trigger(evt, mname, REQ_CMD_MEMBER_GET, FLAGS_SYNC);

    if (PROCESS_OK(evt->errcode)) {
        hdf_set_value(cgi->hdf, PRE_OUTPUT".exist", "1");
        hdf_set_value(cgi->hdf, PRE_OUTPUT".msg", "用户已被注册");
    } else if (evt->errcode == LERR_MEMBER_NEXIST) {
        hdf_set_value(cgi->hdf, PRE_OUTPUT".exist", "0");
        hdf_set_value(cgi->hdf, PRE_OUTPUT".msg", "用户还未注册");
    } else {
        return nerr_raise(evt->errcode, "get member %s info failure %d",
                          evt->ename, evt->errcode);
    }

    return STATUS_OK;
}

NEOERR* member_new_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mnick, *mname, *mid;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".mnick", mnick);
    HDF_GET_STR(cgi->hdf, PRE_QUERY".mname", mname);

    LEGAL_CHECK_NICK(mnick);
    LEGAL_CHECK_NAME(mname);

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_ADD, FLAGS_SYNC);

    mid = hdf_get_value(evt->hdfrcv, "mid", NULL);

    member_after_login(cgi, evth, mname, mnick, mid);

    char *s;
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mnick", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_one", s);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mname", s);
    hdf_set_value(cgi->hdf, PRE_RESERVE".event.es_two", s);

    HDF *node = hdf_get_obj(cgi->hdf, PRE_RESERVE".event");
    
    return nerr_pass(trace_event(node, evth, ses, TRACE_TYPE_MEMBER_REG));
}

NEOERR* member_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname, *msn;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_GET_STR(cgi->hdf, PRE_QUERY".mname", mname); 
    HDF_GET_STR(cgi->hdf, PRE_QUERY".msn", msn);

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_set_value(evt->hdfsnd, "msn", msn);

    MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_CHECK_MSN, FLAGS_SYNC);

    char *mnick = hdf_get_value(evt->hdfrcv, "mnick", "嘀嗒");
    char *mid = hdf_get_value(evt->hdfrcv, "mid", NULL);
    member_after_login(cgi, evth, mname, mnick, mid);

    return STATUS_OK;
}

NEOERR* member_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname;
    NEOERR *err;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    MEMBER_CHECK_LOGIN();

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_set_value(evt->hdfsnd, "mmsn", "0");

    MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_UP, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* member_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname, *mmsn;
    
    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mname", mname);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mmsn", mmsn);
    if (!mname || !mmsn) {
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".mname", mname);
        HDF_GET_STR_IDENT(cgi->hdf, PRE_COOKIE".mmsn", mmsn);
    }
    hdf_set_value(cgi->hdf, PRE_RESERVE".mname", mname);

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_set_value(evt->hdfsnd, "mmsn", mmsn);

    MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_CHECK_MMSN, FLAGS_SYNC);
    
    hdf_copy(cgi->hdf, PRE_OUTPUT".member", evt->hdfrcv);
    
    return STATUS_OK;
}

NEOERR* member_reset_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt;
    char *mnick, *mname, *mname_esc = NULL;
    char rlink[LEN_CK];
    NEOERR *err;
    
    HDF_GET_STR(cgi->hdf, PRE_QUERY".mname", mname);
    mstr_rand_string(rlink, sizeof(rlink));
    
    evt = (mevent_t*)hash_lookup(evth, "member");
    MCS_NOT_NULLB(cgi->hdf, evt);

    hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_set_value(evt->hdfsnd, "rlink", rlink);

    MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_SETRLINK, FLAGS_SYNC);

    mnick = hdf_get_value(evt->hdfrcv, "mnick", NULL);
    neos_url_escape(mname, &mname_esc, NULL);

    hdf_set_value(cgi->hdf, PRE_DATASET".Layout.mnick", mnick);
    hdf_set_value(cgi->hdf, PRE_DATASET".Layout.mname_esc", mname_esc);
    hdf_set_value(cgi->hdf, PRE_DATASET".Layout.rlink", rlink);

    err = email_add(cgi->hdf, evth, "MemberReset", mname);
	if (err != STATUS_OK) return nerr_pass(err);

    SAFE_FREE(mname_esc);

    return STATUS_OK;
}

NEOERR* member_account_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = (mevent_t*)hash_lookup(evth, "member");
    char *mnick, *mname, *mid, *rlink;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);

    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".mname", mname);
    HDF_FETCH_STR(cgi->hdf, PRE_QUERY".rlink", rlink);

    if (mname && rlink) {
        hdf_set_value(evt->hdfsnd, "mname", mname);
        MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_GETRLINK, FLAGS_SYNC);

        mnick = hdf_get_value(evt->hdfrcv, "mnick", NULL);

        char *s = hdf_get_value(evt->hdfrcv, "rlink", NULL);
        if (!s || strcmp(s, rlink))
            return nerr_raise(LERR_USERINPUT, "验证码错误");

        hdf_set_value(evt->hdfsnd, "mname", mname);
        MEVENT_TRIGGER(evt, mname, REQ_CMD_MEMBER_GET, FLAGS_SYNC);
        hdf_copy(cgi->hdf, PRE_OUTPUT".member", evt->hdfrcv);

        mid = hdf_get_value(evt->hdfrcv, "mid", NULL);
        member_after_login(cgi, evth, mname, mnick, mid);

        hdf_set_value(cgi->hdf, PRE_OUTPUT".rlink", rlink);
    } else {
        MEMBER_CHECK_LOGIN();
    }

    SET_DASHBOARD_ACTION(cgi->hdf);
    
    return STATUS_OK;
}

NEOERR* member_home_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "member");
    char *mname = NULL;
    int mid;
    NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);
    
    HDF_FETCH_INT(cgi->hdf, PRE_QUERY".mid", mid);

    if (mid == 0) {
        MEMBER_CHECK_LOGIN();
    } else {
        hdf_set_int_value(evt->hdfsnd, "mid", mid);
        MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMBER_GET, FLAGS_SYNC);
        hdf_copy(cgi->hdf, PRE_OUTPUT".member", evt->hdfrcv);
    }

    evt = hash_lookup(evth, "plan");
    MCS_NOT_NULLA(evt);

    if (mname) hdf_set_value(evt->hdfsnd, "mname", mname);
    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    hdf_set_value(evt->hdfsnd, "_npp", "5");
    hdf_set_value(evt->hdfsnd, "_guest", "1");
    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_MINE, FLAGS_SYNC);
    hdf_copy(cgi->hdf, PRE_OUTPUT, evt->hdfrcv);
    
    return STATUS_OK;
}
