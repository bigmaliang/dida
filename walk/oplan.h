#ifndef __OPLAN_H__
#define __OPLAN_H__
#include "mheads.h"

/*
 * plan 路线
 */

__BEGIN_DECLS

/*
 * Description: 获取最近几天入库的正常路线（内部使用，非接口）
 * Request: days* - 最近的天数
 * Returns: None
 */
NEOERR* plan_of_recent(HDF *hdf, HASH *dbh, HASH *evth);

/*
 * Description: 根据时间和地点，获取匹配路线
 *              该接口将会根据参数和配置进行以下几步路线匹配：
 *              1, 根据scityid, ecityid 快速查找，若总结果数在10~20之间，goto 4
 *              2, 根据rect, sgeo, km 做较精确的查找，若有结果，goto 4
 *              3, 根据rect, sgeo, km 做较模糊的查找，goto 4
 *              4, 若请求中有date, time 参数，将按时间过滤结果，并返回
 * Request: dad* - 0 找车, 1 找人 详见 table_merry.sql
 *
 *          scityid - 起点城市id，/json/city/s 可以根据字符串返回id，详见city_s_data_get()
 *          ecityid - 终点城市id，/json/city/s 可以根据字符串返回id，详见city_s_data_get()
 *          rect - 起点、终点经纬度坐标，格式为 ((startlat, startlng), (endlat, endlng))
 *          sgeo - 起点坐标，因rect里没有限定起终点顺序，故需要传该参数确定由哪里出发
 *          km - 线路总距离，可通过bmoon.js 的 bmoon.utl.earthDis() 函数计算
 *            一般情况下，以上5个参数都建议带上，可以更加快速、准确的匹配路线。
 *            如不能全部提供，至少需要提供
 *            (scityid* + ecityid*) 返回两个城市id间的匹配线路，或者，
 *            (rect* + sgeo* + km*) 返回坐标匹配的线路（更精确）
 *
 *          date - 用户出发的日期
 *          time - 用户出发的时间
 *            以上两个参数不做必传限制，但为了获得更好的匹配效果，建议传上。
 *
 *          _nmax - 匹配结果数限制，默认为 35，参考 server.hdf geoMax 配置项
 *          dayaround - 匹配出发日期前后多少天的结果，该参数无默认值，上传该参数将会忽略_nmax参数值
 * Returns: {success: '1', _ntt: '6', plans: [{id: '4', ...}, {id: '77', ...}, ...]}
 *          为避免联系方式滥用，返回结果中没有线路的 phone, contact 项，
 *          需要通过/image/plan/pic接口查看
 */
NEOERR* plan_match_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 保存用户路线，可同时订阅该路线的匹配路线
 *              该接口工作逻辑：
 *              1, 保存plan
 *              2, 用户需要订阅时，插入订阅记录到 expect
 *              3, 遍历 expect，看该线路是否匹配了别人的 expect
 *              4, 若有匹配，插入匹配记录到 meet
 *              5, 若有匹配，且需要马上告知订阅者，则给订阅者发送站内消息、邮件、或者短信
 * Request: plan* - 路线对象，js 中请使用 JSON.stringify()以下对象
 *                  phone* - 联系电话
 *                  contact - 其他联系方式 IM/Email
 *                  dad* - 0 找车, 1 找人 详见 table_merry.sql
 *                  nick - 称呼（张先生:D）
 *                  repeat* - 路线需求重复 0 一次 1 每天 2 每周 详见 table_merry.sql
 *                  attach - 用户留言
 *                  scityid* - 起点城市id，详见 getCityByPoi() of bmoon.dida.js
 *                  ecityid* - 终点城市id，详见 getCityByPoi() of bmoon.dids.js
 *                  saddr - 起点文字
 *                  eaddr - 终点文字
 *                  sgeo* - 起点地理经纬度坐标
 *                  egeo* - 终点地理经纬度坐标
 *                  marks - 途径地点名
 *                  rect* - 起、终点坐标((slat, slng), (elat, elng))
 *                  route - 途径地点坐标((),(),())
 *                  km* - 路线里程，可通过bmoon.js 的 bmoon.utl.earthDis() 函数计算
 *                  sdate* - 出发日期，
 *                           若repeat为0，格式必须为 YYYY-MM-DD
 *                           若repeat为2，则为逗号分割的阿拉伯数字(1-7)，如6,7表示周末
 *                  stime* - 出发时间，格式必须为 HH:MM:SS
 *                  subscribe - 是否订阅该线路 1 手机订阅 2 邮件订阅 3手机、和邮件订阅
 *          _type_object* - 传值 'plan'，用来告诉后台把plan当作对象使用
 *          LOGIN - 如果用户已登录，该线路将会以该用户名义保存（可以暂停、删除操作等）
 *                  如果用户需要订阅匹配线路，则需要用户已登录
 * Returns: None
 */
NEOERR* plan_leave_data_add(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 查看单条线路的详细信息（除phone, contact属性）
 * Request: id* - 路线id
 * Returns: None
 */
NEOERR* plan_info_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 以图片形式查看路线的联系方式
 * Request: id* - 路线id
 *          type* - 联系方式类型，可以为以下值
 *                    phone 查看路线电话
 *                    contact 查看路线其他联系方式，如IM/Email
 *                    type=phone&type=contact 在路线电话为空时返回contact
 *          defs - 路线联系方式为空时，返回的图片字符串内容，默认为本站域名
 *                 defs为 segv 且联系方式为空时不输出图片，仅输出 {"success": "1"}
 * Returns: None
 */
NEOERR* plan_pic_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 列出用户的线路信息
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 * Returns: 每页5个结果
 */
NEOERR* plan_mine_data_get(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

/*
 * Description: 修改用户的单条线路信息(普通用户可以修改爬虫入库的数据)
 * Request: LOGIN* - see MEMBER_CHECK_LOGIN() of omember.h
 *          id* - 线路id
 *          subscribe - 修改订阅信息，subscribe 为待修改的位，如 email 项为 0x2
 *                      需要同时传 checkop 参数，以指定加上、还是去掉此sub
 * Returns: 修改 plan 表的同时修改 expect 表
 */
NEOERR* plan_mine_data_mod(CGI *cgi, HASH *dbh, HASH *evth, session_t *ses);

__END_DECLS
#endif /* __OPLAN_H__ */
