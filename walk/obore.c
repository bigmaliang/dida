#include "mheads.h"
#include "lheads.h"
#include "obore.h"

#define SET_BORE_ACTION(out) hdf_set_value(out, PRE_WALK_SACTION".0", "actions_1");

NEOERR* bore_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt_member = hash_lookup(evth, "member");
    mevent_t *evt = hash_lookup(evth, "aux");
	NEOERR *err;

    MCS_NOT_NULLB(cgi->hdf, evt);

    err = member_check_login_data_get(cgi, dbh, evth, ses);
    if (err == STATUS_OK) {
        if (hdf_get_int_value(evt_member->hdfrcv, "verify", -1) >= MEMBER_VF_ADMIN)
            SET_BORE_ACTION(cgi->hdf);
    }
    nerr_ignore(&err);

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMORY_GET, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT".memory", evt->hdfrcv);

    /*
     * get recent plan for spider
     */
    evt = hash_lookup(evth, "plan");
    MCS_NOT_NULLA(evt);

    hdf_set_value(evt->hdfsnd, "limit", "50");

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_PLAN_RECENT, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT".plans", evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* bore_new_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    char *mname;
	NEOERR *err;
    
    MEMBER_CHECK_ADMIN();
    SET_BORE_ACTION(cgi->hdf);

    return STATUS_OK;
}

NEOERR* bore_op_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "aux");
    char *mname;
	NEOERR *err;
    
    MEMBER_CHECK_ADMIN();
    SET_BORE_ACTION(cgi->hdf);

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMORY_GET, FLAGS_SYNC);

    hdf_copy(cgi->hdf, PRE_OUTPUT".memory", evt->hdfrcv);

    return STATUS_OK;
}

NEOERR* bore_op_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "aux");
    char *mname;
	NEOERR *err;

    MEMBER_CHECK_ADMIN();

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMORY_MOD, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* bore_op_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{
    mevent_t *evt = hash_lookup(evth, "aux");
    char *mname;
	NEOERR *err;

    MEMBER_CHECK_ADMIN();

    hdf_copy(evt->hdfsnd, NULL, hdf_get_obj(cgi->hdf, PRE_QUERY));
    mstr_script_escape(evt->hdfsnd, "title");
    mstr_script_escape(evt->hdfsnd, "hint");
    mstr_script_escape(evt->hdfsnd, "content");

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_MEMORY_ADD, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* bore_preview_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses)
{

    MCS_NOT_NULLA(cgi->hdf);

    hdf_set_copy(cgi->hdf, PRE_OUTPUT".s", PRE_QUERY".s");

    mstr_script_escape(cgi->hdf, PRE_OUTPUT".s");
    
    return STATUS_OK;
}
