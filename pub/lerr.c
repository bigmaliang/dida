#include "mheads.h"
#include "lheads.h"

/* LERR_XXX start from 25 */

/*
 * static error (layout.html used)
 */
int LERR_NOTLOGIN  = 0;            /* 25 */
int LERR_LOGINPSW  = 0;            /* 26 */
int LERR_LIMIT     = 0;            /* 27 */
int LERR_MISS_DATA = 0;            /* 28 */
int LERR_MISS_TPL  = 0;            /* 29 */
int LERR_ATTACK    = 0;            /* 30 */

/*
 * mevent plugin error
 */
int LERR_MEMBERED = 0;
int LERR_CARED = 0;
int LERR_MEMBER_NEXIST = 0;
int LERR_PLAN_NEXIST = 0;

/*
 * app error
 */
int LERR_USERINPUT = 0;
int LERR_IMGPROE = 0;


static int lerrInited = 0;

NEOERR* lerr_init()
{
    NEOERR *err;

    if (lerrInited == 0) {
        err = nerr_init();
        if (err != STATUS_OK) return nerr_pass(err);

        err = merr_init((MeventLog)mtc_msg);
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_NOTLOGIN, "请登录后操作");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_LOGINPSW, "密码错误");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_LIMIT, "用户无权限");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_MISS_DATA, "请求的资源不存在");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_MISS_TPL, "找不到渲染模板(忘记了/json ?)");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_ATTACK, "太过频繁，请稍后请求！");
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_MEMBERED, "邮箱已被注册！");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_CARED, "一个帐户只能拥有一辆车！");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_MEMBER_NEXIST, "用户不存在");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_PLAN_NEXIST, "路线不存在");
        if (err != STATUS_OK) return nerr_pass(err);

        err = nerr_register(&LERR_USERINPUT, "输入参数错误");
        if (err != STATUS_OK) return nerr_pass(err);
        err = nerr_register(&LERR_IMGPROE, "处理图片失败");
        if (err != STATUS_OK) return nerr_pass(err);

        lerrInited = 1;
    }

    return STATUS_OK;
}

void lerr_opfinish_json(NEOERR *err, HDF *hdf)
{
    if (err == STATUS_OK) {
        hdf_set_value(hdf, PRE_SUCCESS, "1");
        return;
    }
    
    hdf_remove_tree(hdf, PRE_SUCCESS);
    
    NEOERR *neede = mcs_err_valid(err);
    /* set PRE_ERRXXX with the most recently err */
    hdf_set_int_value(hdf, PRE_ERRCODE, neede->error);
    if (!hdf_get_obj(hdf, PRE_ERRMSG)) {
        hdf_set_valuef(hdf, "%s=%s:%d %s",
                       PRE_ERRMSG, neede->file, neede->lineno, neede->desc);
    }

    STRING str; string_init(&str);
    nerr_error_traceback(err, &str);
    mtc_err("%s", str.buf);
    hdf_set_value(hdf, PRE_ERRTRACE, str.buf);
    nerr_ignore(&err);
    string_clear(&str);
}
