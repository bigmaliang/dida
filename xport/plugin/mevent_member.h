#ifndef __MEVENT_MEMBER_H__
#define __MEVENT_MEMBER_H__

#define PREFIX_MEMBER   "Member"
#define PREFIX_MEMBER_PRIV   "MemberPriv"
#define PREFIX_CAR      "Car"

#define MEMBER_CC_SEC   60
#define CAR_CC_SEC      60

#define _COL_MEMBER "mid, mname, mnick, "               \
    " male, verify, credit, cityid, "                   \
    " to_char(intime, 'YYYY-MM-DD') as intime"
#define _COL_MEMBER_ADMIN "mid, mname, mnick, msn, mmsn, male, phone, contact, " \
    " verify, credit, cityid, "                                         \
    " to_char(intime, 'YYYY-MM-DD') as intime"
#define _COL_CAR "carstatu, mid, size, model, mdate"
#define _COL_RESET " mname, rlink, to_char(intime, 'YYYY-MM-DD') as intime "

#define MEMBER_GET_PARAM_MID(hdf, mid)                  \
    do {                                                \
        if (!hdf_get_value(hdf, "mid", NULL)) {         \
            char *_mname = NULL;                        \
            REQ_GET_PARAM_STR(hdf, "mname", _mname);    \
            mid = hash_string_rev(_mname);              \
            hdf_set_int_value(hdf, "mid", mid);         \
        } else mid = hdf_get_int_value(hdf, "mid", 0);  \
    } while (0)
#define MEMBER_SET_PARAM_MID(hdf)                                   \
    do {                                                            \
        if (!hdf_get_value(hdf, "mid", NULL)) {                     \
            char *_mname = NULL;                                    \
            REQ_GET_PARAM_STR(hdf, "mname", _mname);                \
            hdf_set_int_value(hdf, "mid", hash_string_rev(_mname)); \
        }                                                           \
    } while (0)

enum {
    REQ_CMD_MEMBER_GET = 1001,
    REQ_CMD_MEMBER_PRIV_GET,
    REQ_CMD_MEMBER_CHECK_MMSN,
    REQ_CMD_MEMBER_CHECK_MSN,
    REQ_CMD_MEMBER_ADD,
    REQ_CMD_MEMBER_UP,
    REQ_CMD_MEMBER_GETRLINK,
    REQ_CMD_MEMBER_SETRLINK,
    REQ_CMD_CAR_GET,
    REQ_CMD_CAR_ADD,
    REQ_CMD_CAR_UP
};

enum {
    MEMBER_ST_FRESH = 0,
    MEMBER_ST_SPD_OK,
    MEMBER_ST_PAUSE,
    MEMBER_ST_DELETE,
    MEMBER_ST_SPD_FRESH = 10
};

/* verify */
enum {
    MEMBER_VF_FRESH = 0,
    MEMBER_VF_ID,
    MEMBER_VF_DRIVE,
    MEMBER_VF_ADMIN = 90,
    MEMBER_VF_ROOT = 100
};

enum {
    REP_ERR_NOTLOGIN = 25,      /* for member_check_mmsn */
    REP_ERR_LOGINPSW = 26,      /* for member_check_msn */
    REP_ERR_MEMBERED = 31,
    REP_ERR_CARED,
    REP_ERR_MEMBER_NEXIST
};

#endif    /* __MEVENT_MEMBER_H__ */
