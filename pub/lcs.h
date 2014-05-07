#ifndef __LCS_H__
#define __LCS_H__
#include "mheads.h"

__BEGIN_DECLS

/*
 * 获取请求中参数
 */
#define HDF_GET_OBJ(hdf, key, ret)                              \
    do {                                                        \
        if (!hdf_get_obj(hdf, key)) {                           \
            return nerr_raise(LERR_USERINPUT, "need %s", key);  \
        }                                                       \
        ret = hdf_get_obj(hdf, key);                            \
    } while (0)

#define HDF_GET_INT(hdf, key, ret)                              \
    do {                                                        \
        if (!hdf_get_value(hdf, key, NULL)) {                   \
            return nerr_raise(LERR_USERINPUT, "need %s", key);  \
        }                                                       \
        ret = hdf_get_int_value(hdf, key, 0);                   \
    } while (0)

#define HDF_GET_ULONG(hdf, key, ret)                            \
    do {                                                        \
        if (!hdf_get_value(hdf, key, NULL)) {                   \
            return nerr_raise(LERR_USERINPUT, "need %s", key);  \
        }                                                       \
        ret = strtoul(hdf_get_value(hdf, key, NULL), NULL, 10); \
    } while (0)

#define HDF_GET_STR(hdf, key, ret)                              \
    do {                                                        \
        ret = hdf_get_value(hdf, key, NULL);                    \
        if (!ret || *ret == '\0') {                             \
            return nerr_raise(LERR_USERINPUT, "need %s", key);  \
        }                                                       \
    } while (0)

#define HDF_GET_STR_IDENT(hdf, key, ret)                            \
    do {                                                            \
        ret = hdf_get_value(hdf, key, NULL);                        \
        if (!ret) {                                                 \
            return nerr_raise(LERR_NOTLOGIN, "login need %s", key); \
        }                                                           \
    } while (0)


#define HDF_FETCH_INT(hdf, key, ret)                \
    do {                                            \
        ret = hdf_get_int_value(hdf, key, 0);       \
    } while (0)

#define HDF_FETCH_ULONG(hdf, key, ret)                              \
    do {                                                            \
        ret = strtoul(hdf_get_value(hdf, key, NULL), NULL, 10);     \
    } while (0)

#define HDF_FETCH_STR(hdf, key, ret)            \
    do {                                        \
        ret = hdf_get_value(hdf, key, NULL);    \
    } while (0)


__END_DECLS
#endif    /* __LCS_H__ */
