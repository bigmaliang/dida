#include "mheads.h"
#include "lheads.h"
#include "ozero.h"

void* lutil_get_data_handler(void *lib, CGI *cgi, session_t *ses)
{
    char *hname, *tp;
    void *res;

    hname = ses->dataer;
    res = dlsym(lib, hname);
    if ((tp = dlerror()) != NULL) {
        mtc_err("%s", tp);
        return NULL;
    } else
        mtc_info("%s found for data handler", hname);
    return res;
}

NEOERR* lutil_fetch_count(HDF *hdf, mdb_conn *conn, char *table, char *cond)
{
    if (!table || !cond) return nerr_raise(NERR_ASSERT, "paramter null");

    char *buf;
    size_t datalen;
    int count = 0;
    NEOERR *err;
    
    buf = mmc_getf(&datalen, 0, PRE_MMC_COUNT".%s.%s", table, cond);
    if (buf == NULL) {
        err = mdb_exec(conn, NULL, "SELECT count(*) FROM %s WHERE %s;",
                       NULL, table, cond);
        if (err != STATUS_OK) return nerr_pass(err);

        err = mdb_get(conn, "s", &buf);
        if (err != STATUS_OK) return nerr_pass(err);

        count = atoi(buf);
        mmc_storef(MMC_OP_SET, (void*)buf, 0, ONE_HOUR, 0,
                   PRE_MMC_COUNT".%s.%s", table, cond);
    } else {
        count = atoi(buf);
    }

    hdf_set_int_value(hdf, PRE_OUTPUT".ttnum", count);

    return STATUS_OK;
}

NEOERR* lutil_fetch_countf(HDF *hdf, mdb_conn *conn, char *table, char *cfmt, ...)
{
    char cond[LEN_SM];
    va_list ap;

    va_start(ap, cfmt);
    vsnprintf(cond, sizeof(cond), cfmt, ap);
    va_end(ap);

    return nerr_pass(lutil_fetch_count(hdf, conn, table, cond));
}

bool lutil_client_attack(HDF *hdf, session_t *ses, char *cname)
{
    if (!hdf || !ses || !ses->dataer) return false;

    char *limit = hdf_get_valuef(g_cfg, PRE_CFG_REQLIMIT".%s", ses->dataer);
    if (!limit || atoi(limit) <= 0) return false;

    return mutil_client_attack(hdf, ses->dataer, cname, atoi(limit), ONE_MINUTE);
}
