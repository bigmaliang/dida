#ifndef __OBORE_H__
#define __OBORE_H__
#include "mheads.h"

/*
 * bore 随便看看
 */

__BEGIN_DECLS

/*
 * Description: 获取单条随便看看
 * Request: id - 单条看看的id号
 * Returns: {memory: 'xxxx'}
 */
NEOERR* bore_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 添加随便看看页面
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of omember.h
 * Returns: None
 */
NEOERR* bore_new_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 修改随便看看页面
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of member.h
 *          id* - 单条看看id号
 * Returns: {memory: 'xxxx'}
 */
NEOERR* bore_op_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 提交修改随便看看
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of member.h
 *          id* - 单条看看id号
 *          title - 看看标题
 *          hint - 看看副标题
 *          content - 看看内容
 *          statu - 看看状态，see table_aux.sql
 * Returns: None
 */
NEOERR* bore_op_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 提交添加随便看看
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of member.h
 *          title* - 看看标题
 *          content* - 看看内容，仅支持 markdown 语法，用户输入的 html 字符将被转义
 *                     如 <p> 将被存为&lt;p&gt;
 *          hint - 看看副标题
 * Returns: None
 */
NEOERR* bore_op_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 预览看看
 * Request: s* - markdown string
 * Returns: 经过markdown语法转换后的 html 页面
 */
NEOERR* bore_preview_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OBORE_H__ */
