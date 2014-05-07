#ifndef __OZERO_H__
#define __OZERO_H__
#include "mheads.h"

/*
 * zero 占位符，为特殊功能预留
 */

__BEGIN_DECLS

NEOERR* zero_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);
NEOERR* zero_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 上传并缩放图片接口，使用方法请参考 fly/js/bore/op.js
 * Request: upfile* - POST 上传文件，表单name=upfile
 *          _upfile_data_type - 说明上传文件内容格式，参考 cmoon/README
 * Returns: None
 */
NEOERR* zero_image_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OZERO_H__ */
