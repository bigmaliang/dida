#ifndef __OTRACE_H__
#define __OTRACE_H__
#include "mheads.h"

/*
 * trace 轨迹系统
 */

__BEGIN_DECLS

/*
 * Description: 保存事件（内部使用，非接口）
 * Request: node* - 设置了该事件的 ei_xxx, es_xxx, et_xxx 事件内容
 *          type* - 事件类型，参考 mevent_trace.h
 * Returns: None
 */
NEOERR* trace_event(HDF *node, HASH *evth, session_t *ses, int type);

/*
 * Description: 保存系统操作轨迹（pv, uv, mv, comment, messge...）
 * Request: type* - 操作类型，参考 mevent_trace.h
 *          sender* - 操作发起者，登录用户邮箱，或者未登录用户代号
 *          provid* - 省／区 id， 参考 bmoon.dida.js 之 o.c_province
 *          cityid* - 最小地区城市 id， 参考 bmoon.dida.js 之 o.c_cicy
 *          browser* - 浏览器类型，参考 mevent_trace.h
 *          bversion* - 浏览器版本
 *          e[i/s/t]_[one/two/three] - 保留参数，不同操作请自行带上不同参数
 * Returns: None
 */
NEOERR* trace_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OTRACE_H__ */
