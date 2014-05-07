#ifndef __OCOMMENT_H__
#define __OCOMMENT_H__
#include "mheads.h"

/*
 * comment 评论接口
 */

__BEGIN_DECLS

/*
 * Description: 获取评论
 * Request: ids* - 由,分隔的评论key组成，其中评论key为type:id，type 详见 table_aux.sql
 *                 如 0:988 0:988,0:6849 1:4553,0:223 均为有效ids
 * Returns: {"type1": {"ida": {....}, "idb": {....}}, "type2": {"idc": {....}}}
 *          返回对象中，type 为 key， 该 type 下的所有评论再以 id 为 key 区分，如：
 *          { "0": { "998": { "_ntt": "0", "_nst": "0" } }, "success": "1" }
 *          { "0": { "223": { "_ntt": "0", "_nst": "0" } }, "1": { "4553": { "_ntt": "0", "_nst": "0" } }, "success": "1" }
 */
NEOERR* comment_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 添加评论
 * Request: type* - 评论类型， 详见 table_aux.sql
 *          oid* - 评论对象id
 *          pid - 若是对评论回复，pid为回复对象的id。最上层留言，pid默认为0
 *          ip - 发布评论者ip，接口会从标准 HTTP 头中获取
 *          author - 发布评论者昵称
 *          contact - 发布评论者联系方式
 *          content - 评论内容
 * Returns: None
 */
NEOERR* comment_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 修改评论（暂空）
 * Request:
 * Returns: None
 */
NEOERR* comment_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 删除评论
 * Request: ADMIN* - see MEMBER_CHECK_ADMIN() of omember.h
 *          id* - 评论id
 * Returns: None
 */
NEOERR* comment_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OCOMMENT_H__ */
