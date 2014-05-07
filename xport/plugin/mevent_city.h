#ifndef __MEVENT_CITY_H__
#define __MEVENT_CITY_H__

#define PREFIX_CITY   "City"
#define PREFIX_PLACE  "Place"
#define CITY_CC_SEC   60000

#define _COL_CITY "id, grade, pid, s, geopos"

#define REDIRECT_MODE_1	 0x01
#define	REDIRECT_MODE_2	 0x02
#define	IP_RECORD_LENGTH 7

enum {
    REQ_CMD_CITY_BY_IP = 1001,
    REQ_CMD_CITY_BY_ID,
    REQ_CMD_CITY_BY_S,
    REQ_CMD_PLACE_GET
};

#endif    /* __MEVENT_CITY_H__ */
