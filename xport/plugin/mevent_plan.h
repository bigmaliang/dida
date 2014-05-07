#ifndef __MEVENT_PLAN_H__
#define __MEVENT_PLAN_H__

#define PREFIX_PLAN      "Plan"
#define PREFIX_PLAN_PRIV "PlanPriv"
#define PREFIX_PLAN_MINE "PlanMine"
#define PREFIX_SPD       "Spd"
#define PREFIX_RECENT    "Recent"

#define PLAN_CC_SEC      60
#define PLAN_MINE_CC_SEC 600
#define SPD_CC_SEC       60000
#define RECENT_CC_SEC    60000

#define _COL_PLAN "id, mid, ori, ourl, dad, nick, statu, repeat, subscribe, seat, " \
    " fee, attach, cityid, scityid, ecityid, saddr, eaddr, marks, rect, route, " \
    " round(km, 1) as km, sdate, edate, stime, etime, estmin, "         \
    " to_char(intime, 'YYYY-MM-DD') as intime, "                        \
    " to_char(uptime, 'YYYY-MM-DD') as uptime "

#define _COL_PLAN_ADMIN "id, mid, phone, contact, ori, ourl, "          \
    " dad, nick, statu, repeat, subscribe, seat, "                      \
    " fee, attach, cityid, scityid, ecityid, saddr, eaddr, marks, rect, route, " \
    " round(km, 1) as km, sdate, edate, stime, etime, estmin, "         \
    " to_char(intime, 'YYYY-MM-DD') as intime, "                        \
    " to_char(uptime, 'YYYY-MM-DD') as uptime "

#define _COL_PLAN_C "p.id AS id, p.mid AS mid, "                        \
    " p.phone AS phone, p.contact AS contact, "                         \
    " p.ori AS ori, p.ourl AS ourl, "                                   \
    " p.dad AS dad, p.nick AS nick, p.statu AS statu, p.repeat AS repeat, " \
    " p.seat AS seat, p.fee AS fee, p.attach AS attach, "               \
    " p.cityid AS cityid, p.scityid AS scityid, p.ecityid AS ecityid, " \
    " p.saddr AS saddr, p.eaddr AS eaddr, p.marks AS marks, "           \
    " p.rect AS rect, p.route AS route, p.km AS km, "                   \
    " p.sdate AS sdate, p.edate AS edate, "                             \
    " p.stime AS stime, p.etime AS etime, p.estmin AS estmin, "         \
    " to_char(p.intime, 'YYYY-MM-DD') as intime, "                      \
    " to_char(p.uptime, 'YYYY-MM-DD') as uptime, "                      \
    " c.geopos AS geopos, c.s AS city "

enum {
    REQ_CMD_PLAN_GET = 1001,
    REQ_CMD_PLAN_PRIV_GET,
    REQ_CMD_PLAN_MATCH,
    REQ_CMD_PLAN_ADD,
    REQ_CMD_PLAN_UP,
    REQ_CMD_PLAN_MINE,
    REQ_CMD_PLAN_RECENT,
    REQ_CMD_SPD_PEEL
};

enum {
    PLAN_ST_FRESH = 0,
    PLAN_ST_SPD_OK,             /* man confirmed ok */
    PLAN_ST_SPD_RBT_OK,         /* machine confirmed ok */
    PLAN_ST_PAUSE,
    PLAN_ST_DELETE,
    PLAN_ST_SPD_FRESH = 10,
    PLAN_ST_SPD_RBTED           /* machine checked, but can't confirm */
};

enum {
    PLAN_DAD_MAN = 0,
    PLAN_DAD_CAR
};

enum {
    PLAN_RPT_NONE = 0,
    PLAN_RPT_DAY,
    PLAN_RPT_WEEK
};

enum {
    REP_ERR_PLAN_NEXIST = 34
};

#endif    /* __MEVENT_PLAN_H__ */
