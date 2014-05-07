#ifndef __OPOS_H__
#define __OPOS_H__
#include "mheads.h"

/*
 * pos 城市地理位置
 */

__BEGIN_DECLS

/*
 * Description: 查看 city 表中哪些城市地理坐标 geopos 为空（管理后台使用）
 * Request: None
 * Returns: None
 */
NEOERR* pos_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
/*
 * Description: 修改 city 表的 geopos 字段 （管理后台使用）
 * Request: id* - cityid
 *          latlng* - 地理坐标
 * Returns: None
 */
NEOERR* pos_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 通过 ip 获取用户城市信息，精确到市
 * Request: ip - 指定ip, 不传时默认从 HTTP 头中取当前客户端ip
 * Returns: None
 */
NEOERR* city_ip_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 通过字符串获取其城市信息
 * Request: c* - 待查询字符串，该字符串必须为 city 表中的 s 精确匹配
 *               可通过 bmoon.dida.getCityByGeoresult() 函数获取c, 使用方法请参考 index.js
 * Returns: 自行参考 /json/city/s?c=%E5%BC%80%E7%A6%8F%E5%8C%BA
 */
NEOERR* city_s_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 通过id获取城市信息
 * Request: id* - 待查city id
 * Returns: None
 */
NEOERR* city_id_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OPOS_H__ */
