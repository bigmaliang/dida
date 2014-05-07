#ifndef __OMEMBER_H__
#define __OMEMBER_H__
#include "mheads.h"

__BEGIN_DECLS

/*
 * Description: 验证用户登录（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 */
#define MEMBER_CHECK_LOGIN()                                        \
    do {                                                            \
        err = member_check_login_data_get(cgi, dbh, evth, ses);     \
        if (err != STATUS_OK) return nerr_pass(err);                \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);    \
    } while (0)

/*
 * Description: 验证用户登录，且是否为管理员（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 *         {errcode: 27, errmsg: '用户无权限'}
 */
#define MEMBER_CHECK_ADMIN()                                            \
    do {                                                                \
        err = member_check_login_data_get(cgi, dbh, evth, ses);         \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);        \
        mevent_t *_evt = hash_lookup(evth, "member");                   \
        MCS_NOT_NULLA(_evt);                                            \
        if (hdf_get_int_value(_evt->hdfrcv, "verify", -1) < MEMBER_VF_ADMIN) \
            return nerr_raise(LERR_LIMIT, "%s not admin", mname);       \
    } while (0)

/*
 * Description: 验证用户登录，且是否为老爸（内部使用，非接口）
 * Request: mname - 用户名，query或cookie自带均可
 *          mmsn - 用户临时口令，由member/login生成，query或cookie自带均可
 * Return: {errcode: 25, errmsg: '请登录后操作'}
 *         {errcode: 27, errmsg: '用户无权限'}
 */
#define MEMBER_CHECK_ROOT()                                             \
    do {                                                                \
        err = member_check_login_data_get(cgi, dbh, evth, ses);         \
        if (err != STATUS_OK) return nerr_pass(err);                    \
        HDF_GET_STR_IDENT(cgi->hdf, PRE_RESERVE".mname", mname);        \
        mevent_t *_evt = hash_lookup(evth, "member");                   \
        MCS_NOT_NULLA(_evt);                                            \
        if (hdf_get_int_value(_evt->hdfrcv, "verify", -1) < MEMBER_VF_ROOT) \
            return nerr_raise(LERR_LIMIT, "%s not admin", mname);       \
    } while (0)

/*
 * called after CHECK_XXX() immediately
 */
#define SET_DASHBOARD_ACTION(out)                                       \
    do {                                                                \
        hdf_set_value(out, PRE_WALK_SACTION".0", "actions_1");          \
        mevent_t *_evt = hash_lookup(evth, "member");                   \
        if (hdf_get_int_value(_evt->hdfrcv, "verify", -1) >= MEMBER_VF_ADMIN) \
            hdf_set_value(out, PRE_WALK_SACTION".1", "actions_2");      \
    }                                                                   \
    while (0)

/* TODO */
#if 0
#define LEGAL_CHECK_NICK(mnick)                                     \
    do {                                                            \
        if (!reg_search("^([a-zA-Z0-9_\.\-])+$", mnick)) {          \
            return nerr_raise(LERR_USERINPUT, "%s illegal", mnick); \
        }                                                           \
    } while(0)

#define LEGAL_CHECK_MNAME(mname)                                        \
    do {                                                                \
        if (!reg_search("^([a-zA-Z0-9_\.\-])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+$", mname)) { \
            return nerr_raise(LERR_USERINPUT, "%s illegal", mname);     \
        }                                                               \
    } while(0)
#else
#define LEGAL_CHECK_NICK(mnick)
#define LEGAL_CHECK_NAME(mname)
#endif

/*
 * Description: 查看用户详情
 * Request: mname - 用户邮箱，不传则默认查看自己详情，查看自己时需要登录
 * Returns: 查看别人详情时没有 phone, contact 等联系方式， 查看自己时则有
 */
NEOERR* member_info_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 查看用户联系方式，以图片形式返回
 * Request: mname* - 用户邮箱
 *          type* - 联系方式类型，可以为以下值
 *                    phone 查看用户电话
 *                    contact 查看用户其他联系方式，如IM/Email
 *                    type=phone&type=contact 在用户电话为空时返回contact
 *          defs - 用户联系方式为空时，返回的图片字符串内容，默认为本站域名，
 *                 defs为 segv 且联系方式为空时不输出图片，仅输出 {"success": "1"}
 *          fpath - 指定图片字体
 *          fsize - 指定图片大小
 * Returns: 图片，内容为需要的字符串
 */
NEOERR* member_pic_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户修改自己的信息
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 *          mnick - 用户昵称
 *          male - 性别
 *          phone - 用户电话
 *          contact - 其他联系方式 IM/Email
 *          msn - 用户密码，修改密码不能与修改基本信息同时操作，需要前密码，或者重置的rlink
 * Returns: None
 */
NEOERR* member_edit_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户修改自己的汽车信息
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 *          size - 汽车类型，参考 table_merry.sql
 *          model - 汽车型号，字符串
 *          mdate - 购买日期
 * Returns: None
 */
NEOERR* member_car_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 判断邮箱是否已被注册
 * Request: mname* - 需要判断的邮箱号
 * Returns: {success: "1", exist: '1', msg: "用户已被注册"}
 *          {success: "1", exist: '0', msg: "用户还未注册"}
 */
NEOERR* member_exist_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 注册新用户
 * Request: mname* - 用户邮箱
 *          mnick* - 用户昵称
 *          msn* - 用户密码
 *          male - 性别
 *          phone - 用户电话
 *          contact - 其他联系方式 IM/Email
 * Returns: None
 */
NEOERR* member_new_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户登录
 * Request: mname* - 用户邮箱
 *          msn* - 用户密码
 * Returns: {errcode: 26, errmsg: '密码错误'}
 *          {"mname": "imtestt@163.com", "mmsn": "AOMXKA...", "success": "1" }
 *          其中，mmsn 为用户的临时token，一周内有效，请保存该返回，check_login时会用到
 */
NEOERR* member_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户登出
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 * Returns: None
 */
NEOERR* member_logout_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 判断登录是否有效（根据mname和mmsn）
 * Request: mname* - 用户邮箱，cookie或query中自带均可
 *          mmsn* - 登录时返回的 mmsn，临时token，一周内有效，cooke或query中自带均可
 * Returns: {errcode: 25, errmsg: '请登录后操作'}
 *          登录用户的用户信息写到 cgi->hdf 的 Output.member 中
 */
NEOERR* member_check_login_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 申请重置用户密码，重置连接将发往用户邮箱
 * Request: mname* - 用户注册时填的邮箱
 * Returns: None
 */
NEOERR* member_reset_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户帐号信息页面（登录用户，或者通过重置邮件跳转）
 * Request: LOGIN - see MEMBER_CHECK_LOGIN() of omember.h
 *          mname - 用户邮箱
 *          rlink - 重置密码时，系统生成的验证码
 *          (LOGIN，或者mname,rlink 二选一必填)
 * Returns: None
 */
NEOERR* member_account_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 用户个人信息展示页
 * Request: mid[*] - 用户id，强烈建议使用mid参数来展示（包括本人主页），
 *                   在没有mid的情况下， 支持 LOGIN 检查，查看自己的主页。
 * Returns: None
 */
NEOERR* member_home_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OMEMBER_H__ */
