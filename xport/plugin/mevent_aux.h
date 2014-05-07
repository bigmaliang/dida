#ifndef __MEVENT_AUX_H__
#define __MEVENT_AUX_H__

#define PREFIX_COMMENT  "Comment"
#define PREFIX_MEMORY   "Memory"
#define PREFIX_INBOX    "Inbox"
#define CMT_CC_SEC      60
#define MEMORY_CC_SEC   0
#define INBOX_CC_SEC    600

#define _COL_MEMORY "id, statu, title, hint, content, previd, nextid, " \
    " to_char(intime, 'YYYY-MM-DD') as intime, "                        \
    " to_char(uptime, 'YYYY-MM-DD') as uptime "

#define _COL_CMT "id, type, oid, otitle, pid, ip, addr, author, content, " \
    " to_char(intime, 'YYYY-MM-DD HH24:MI:SS') as intime, "           \
    " to_char(uptime, 'YYYY-MM-DD HH24:MI:SS') as uptime "

#define _COL_INBOX "id, type, mid, fresh, fmid, fnick, message, " \
    " to_char(intime, 'YYYY-MM-DD HH24:MI:SS') as intime "

enum {
    MEMORY_ST_OK = 0,
    MEMORY_ST_DEL
};

enum {
    CMT_ST_NORMAL = 0,
    CMT_ST_DEL
};

enum {
    INBOX_ST_NORMAL = 0,
    INBOX_ST_DEL
};

enum {
    CMT_TYPE_MEMORY = 0
};

enum {
    INBOX_TYPE_SYSTEM = 0,
    INBOX_TYPE_PRIVATE
};

enum {
    REQ_CMD_CMT_GET = 1001,
    REQ_CMD_CMT_ADD,
    REQ_CMD_CMT_DEL,
    REQ_CMD_MEMORY_GET,
    REQ_CMD_MEMORY_ADD,
    REQ_CMD_MEMORY_MOD,
    REQ_CMD_AUX_EMAIL_ADD = 2001,
    REQ_CMD_AUX_INBOX_GET,
    REQ_CMD_AUX_INBOX_ADD,
    REQ_CMD_AUX_INBOX_DEL
};

#endif    /* __MEVENT_AUX_H__ */
