#ifndef __OINBOX_H__
#define __OINBOX_H__
#include "mheads.h"

__BEGIN_DECLS

/*
 * Description: 发送多个站内信（内部使用，非接口）
 * Input: datanode* - 数据hdf，递归该node的所有子node, 所有有 mid key 的将设为收件人地址
 *                    同时支持以下 template 子 node (详见mtemplate.h):
 *                     PRE_DATASET  :data used by template file on cs_rend()
 *                     PRE_VALUESET :copy to out direct
 *                     PRE_VALUEREP :replace valueset after valueset copied
 *        inboxtype* - 站内信类型字符串，支持所有tpl/config/inbox/xxx.hdf 下的子node key
 * Returns: None
 */
NEOERR* inbox_multi_add(HDF *datanode, HASH *evth, char *inboxtype);

/*
 * Description: 查看用户系统通知
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 * Returns: None
 */
NEOERR* inbox_system_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 删除指定通知
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 *          id* - 通知id
 *          type* - 通知类型
 * Returns: None
 */
NEOERR* inbox_data_del(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 查看用户私人信箱
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 * Returns: None
 */
NEOERR* inbox_private_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 查看用户发件箱
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 * Returns: None
 */
NEOERR* inbox_sent_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OINBOX_H__ */
