#include "mheads.h"
#include "lheads.h"
#include "oemail.h"

NEOERR* email_multi_add(HDF *datanode, HASH *evth, char *emailtype)
{
    mevent_t *evt;
    char *mname;
    HDF *child;
    int cnt = 0;
    NEOERR *err;
    
    MCS_NOT_NULLB(datanode, evth);

    evt = hash_lookup(evth, "aux");
    MCS_NOT_NULLA(evt);

    child = hdf_obj_child(datanode);
    while (child) {
        mname = hdf_get_value(child, "mname", NULL);
        if (mname) {
            /*
             * use cnt instead of mname as hdf key because mname contain '.'
             */
            hdf_set_valuef(evt->hdfsnd, "mto.%d=%s", cnt++, mname);
        }

        child = hdf_obj_next(child);
    }

    if (!hdf_get_obj(evt->hdfsnd, "mto"))
        return nerr_raise(NERR_ASSERT, "请设置收信用户名");

    err = mtpl_InConfigRend_get(evt->hdfsnd, datanode, "email", emailtype, g_datah);
	if (err != STATUS_OK) return nerr_pass(err);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_AUX_EMAIL_ADD, FLAGS_NONE);

    return STATUS_OK;
}

NEOERR* email_add(HDF *datanode, HASH *evth, char *emailtype, char *mname)
{
    mevent_t *evt;
    NEOERR *err;
    
    MCS_NOT_NULLC(datanode, evth, mname);

    evt = hash_lookup(evth, "aux");
    MCS_NOT_NULLA(evt);

    err = mtpl_InConfigRend_get(evt->hdfsnd, datanode, "email", emailtype, g_datah);
	if (err != STATUS_OK) return nerr_pass(err);

    hdf_set_value(evt->hdfsnd, "to", mname);

    MEVENT_TRIGGER(evt, NULL, REQ_CMD_AUX_EMAIL_ADD, FLAGS_NONE);

    return STATUS_OK;
}
