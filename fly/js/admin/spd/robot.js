; var bmoon = bmoon || {};
bmoon.spdrobot = {
    version: '1.0',

    init: function() {
        var o = bmoon.spdrobot;
        if (o.inited) return o;

        o.e_count = $('#count');
        o.e_res = $('#res');
        
        o.g_geocode = new BMap.Geocoder();
        o.g_map = new BMap.Map('map');
        o.g_map.enableScrollWheelZoom();
        o.g_smarker = new BMap.Marker();
        o.g_emarker = new BMap.Marker();
        o.g_direct = new BMap.Polyline();

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.spdrobot.init();

        o.bindClick();
        o.getPlans();
    },

    bindClick: function() {
        var o = bmoon.spdrobot.init();

    },

    outClear: function() {
        var o = bmoon.spdrobot.init();

        o.e_res.empty();
    },

    outPut: function(msg) {
        var o = bmoon.spdrobot.init();

        $('<pre>' + msg + '</pre>').prependTo(o.e_res);
    },

    getPlans: function() {
        var o = bmoon.spdrobot.init();

        o.outClear();
        $.getJSON('/json/spd/post/robot', null, function(data) {
            if (data.success == 1 && bmoon.utl.type(data.plans) == 'Array') {
                o.plans = data.plans;
                o.cur_plan = 0;
                o.e_count.html(o.plans.length)
                o.parsePlan();
            } else {
                o.outPut(data.errmsg);
                o.outPut(data.errtrace);
                setTimeout(o.getPlans, 60*1000);
            }
        });
    },

    // produce saddr, eaddr, scityid, ecityid, rect, km accord saddr, eaddr
    parsePlan: function() {
        var o = bmoon.spdrobot.init();

        if (o.cur_plan == o.plans.length) {
            o.outPut('完了，休息会儿！');
            setTimeout(o.getPlans, 60*1000);
            return;
        }

        var plan = o.plans[o.cur_plan++];
        setTimeout(o.parsePlan, 5*1000);

        o.outPut('解析 ' + plan.id + ':' + plan.saddr + ' - ' + plan.eaddr + ' ...');

        o.g_geocode.getPoint(plan.saddr, function(point) {
            if (point) {
                o.g_smarker.setPosition(point);
                o.g_map.addOverlay(o.g_smarker);
                o.g_map.centerAndZoom(point, 13);
                
                plan.sll = [point.lat, point.lng];
                o.g_geocode.getLocation(point, function(result) {
                    bmoon.dida.getCityByPoi(result.addressComponents, function(city) {
                        if (bmoon.utl.type(city) == 'Array') {
                            plan.scityid = city[0].id;
                            o.g_geocode.getPoint(plan.eaddr, function(point) {
                                if (point) {
                                    o.g_emarker.setPosition(point);
                                    o.g_map.addOverlay(o.g_emarker);
                                    o.g_map.centerAndZoom(point, 13);
                                    plan.ell = [point.lat, point.lng];

                                    var km = bmoon.utl.earthDis(plan.sll, plan.ell);
                                    if (km > 10 && km < 100) o.g_map.setZoom(12);
                                    else if (km > 100 && km < 300) o.g_map.setZoom(9);
                                    else if (km > 300 && km < 600) o.g_map.setZoom(7);
                                    else if (km > 600) o.g_map.setZoom(6);

                                    o.outPut(km + '千米');

                                    o.g_direct.setPath([
                                        new BMap.Point(plan.sll[1], plan.sll[0]),
                                        new BMap.Point(plan.ell[1], plan.ell[0])
                                    ]);
                                    o.g_map.addOverlay(o.g_direct);
                                    
                                    o.g_geocode.getLocation(point, function(result) {
                                        bmoon.dida.getCityByPoi(result.addressComponents, function(city) {
                                            if (bmoon.utl.type(city) == 'Array') {
                                                plan.ecityid = city[0].id;

                                                if (plan.scityid && plan.ecityid) {
                                                    plan.rect = '((' + plan.sll.join(',') + '),(' +
                                                        plan.ell.join(',') + '))';
                                                    plan.sgeo = '(' + plan.sll.join(',') +')';
                                                    plan.egeo = '(' + plan.ell.join(',') +')';
                                                    plan.km = bmoon.utl.earthDis(plan.sll, plan.ell);

                                                    if (plan.repeat > 0 && plan.km > 120.0) {
                                                        o.outPut('长途拼车，却重复。');
                                                        o.parsePlanErr(plan);
                                                    }

                                                    var nplan = {
                                                        id: plan.id,
                                                        scityid: plan.scityid,
                                                        ecityid: plan.ecityid,
                                                        rect: plan.rect,
                                                        sgeo: plan.sgeo,
                                                        egeo: plan.egeo,
                                                        km: plan.km
                                                    },
                                                    pdata = {
                                                        _op: 'mod',
                                                        plan: JSON.stringify(nplan),
                                                        _type_object: 'plan'
                                                    };
                                                    $.post('/json/spd/post/robot', pdata, function(data) {
                                                        if (data.success == 1) o.outPut('完成');
                                                        else o.outPut(data.errmsg);
                                                    }, 'json');
                                                }
                                            } else {
                                                o.outPut('解析起点有错 ' + city);
                                                o.parsePlanErr(plan);
                                            }
                                        });
                                    });
                                } else {
                                    o.outPut('解析终点失败');
                                    o.parsePlanErr(plan);
                                }
                            }, plan.city);
                        } else {
                            o.outPut('解析起点有错 ' + city);
                            o.parsePlanErr(plan);
                        }
                    });
                });
            } else {
                o.outPut('解析起点失败');
                o.parsePlanErr(plan);
            }
            
        }, plan.city);
    },

    parsePlanErr: function(plan) {
        var o = bmoon.spdrobot.init();

        var nplan = {
            id: plan.id,
            statu: 11
        },
        pdata = {
            _op: 'mod',
            plan: JSON.stringify(nplan),
            _type_object: 'plan'
        };
        $.post('/json/spd/post/robot', pdata, function(data) {
            if (data.success == 1) o.outPut('处理有错，完成置位');
            else o.outPut(data.errmsg);
        }, 'json');
    }
};

$(document).ready(bmoon.spdrobot.onready);
