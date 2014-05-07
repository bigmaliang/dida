#ifndef __MEVENT_FFT_H__
#define __MEVENT_FFT_H__

#define PREFIX_FFT     "Fft"

#define _COL_EXPECT "id, mid, mname, pid, statu, addrtype, gotime, "   \
    " dad, sgeo, rect, km, repeat, sdate, stime, "              \
    " to_char(intime, 'YYYY-MM-DD') as intime "

enum {
    FFT_EXPECT_NONE = 0,
    FFT_EXPECT_PHONE = (1 << 0),
    FFT_EXPECT_EMAIL = (1 << 1),
    FFT_EXPECT_ALL = (0x7F << 7)
};

enum {
    FFT_GOTIME_IMMEDIATE = 0,
    FFT_GOTIME_DAY,
    FFT_GOTIME_WEEK
};

enum {
    FFT_EXPECT_STATU_OK = 0,
    FFT_EXPECT_STATU_PAUSE,
    FFT_EXPECT_STATU_DEL
};

enum {
    REQ_CMD_FFT_EXPECT_ADD = 1001,
    REQ_CMD_FFT_EXPECT_UP,
    REQ_CMD_FFT_EXPECT_MATCH
};

#endif    /* __MEVENT_FFT_H__ */
