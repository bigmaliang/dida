; var bmoon = bmoon || {};
bmoon.index = {
    version: '1.0',

    // {"province":"","city":"长沙市","district":"开福区","street":"","streetNumber":"","business":"开福寺"}
    _strFromPoi: function(d) {
        if (bmoon.utl.type(d) == 'Object')
            return d.province + d.city + d.district + d.street + d.streetNumber + d.business;
    },
    
    _formInfoTitle: function(x) {
        if (x != 'e') return '<b>线路起点</b>';
        else return '<b>线路终点</b>';
    },

    _formInfoContent: function(result) {
        var s = result.address,
        pois = result.surroundingPois;

        if (pois && pois.length > 0) {
            s += '<br /><b>周边信息</b>'
            for (var i = 0; i < pois.length; i++) {
                s += '<br />' + pois[i].title;
            }
        }

        return s;
    },
    
    init: function() {
        var o = bmoon.index;
        if (o.inited) return o;

        o.e_hour = $('#hour');
        o.e_min = $('#min');
        o.e_dad = $('#dad');
        o.e_dateinput = $('#date-input');
        o.e_datehint = $('#date-hint');

        o.e_saddr = $('#saddr');
        o.e_eaddr = $('#eaddr');
        o.e_km = $('#km');
        o.e_maphint = $('#map-hint');

        o.e_submit = $('#submit');

        o.e_mc_noresult = $('#mc-noresult');
        o.e_mc_result = $('#mc-result');
        o.e_mc_nav = $('#mc-nav');
        o.e_mc_prev = $('#mc-prev');
        o.e_mc_next = $('#mc-next');
        o.e_mc_continue = $('#mc-continue');
        o.e_mc_nick = $('#mc-nick');
        o.e_mc_saddr = $('#mc-saddr');
        o.e_mc_eaddr = $('#mc-eaddr');
        o.e_mc_planurl = $('#mc-planurl');
        o.e_mc_del = $('#mc-del');
        o.e_mc_from = $('#mc-from');
        o.e_mc_sdate = $('#mc-sdate');
        o.e_mc_stime = $('#mc-stime');
        o.e_mc_phone = $('#mc-phone');
        o.e_mc_contact = $('#mc-contact');
        o.e_mc_attach = $('#mc-attach');
        o.e_mc_num_nav = $('#mc-num-nav');

        o.e_mc_no_repeat = $('#mc-no-repeat');
        o.e_mc_no_wday = $('#mc-no-wday');
        o.e_mc_no_phone = $('#mc-no-phone');
        o.e_mc_no_contact = $('#mc-no-contact');
        o.e_mc_no_nick = $('#mc-no-nick');
        o.e_mc_no_attach = $('#mc-no-attach');
        o.e_mc_no_subscribes = $('input[name=subscribe]');
        o.e_mc_no_submit = $('#mc-no-submit');

        o.plan = {};

        o.inited = true;
        return o;
    },
    
    initMap: function(geo) {
        var o = bmoon.index.init();

        geo = geo || [28.188,113.033];
        o.g_lat = new BMap.Point(geo[1], geo[0]);
        o.g_map = new BMap.Map("map");
        o.g_map.enableScrollWheelZoom();
        o.g_map.centerAndZoom(o.g_lat, 13);

        o.g_sauto = new BMap.Autocomplete({
            location: o.g_map,
            input: 'saddr'});
        o.g_eauto = new BMap.Autocomplete({
            location: o.g_map,
            input: 'eaddr'});
        o.g_geocode = new BMap.Geocoder();

        o.g_smarker = new BMap.Marker();
        o.g_emarker = new BMap.Marker();
        o.g_smarker.enableDragging();
        o.g_emarker.enableDragging();
        o.g_direct = new BMap.Polyline();
        o.g_prect = new BMap.Polygon();
        o.g_infow = new BMap.InfoWindow('起点', {
            width: 250,
            height: 100
        });
        o.bindMapChange();
    },

    onready: function() {
        var o = bmoon.index.init();

        $.tools.dateinput.localize("zh", bmoon.dida.dateinputzh);
        o.calendar = o.e_dateinput.dateinput({
            format: 'yyyy-mm-dd',
            lang: 'zh',
            trigger: true,
            firstDay: 1,
            change: function(e, date) {
                o.e_datehint.html(this.getValue('dddd'))
            }
        });
        
        o.e_mc_del.tooltip({
            tipClass: 'tipsy tipsy-south',
            layout: '<div><div class="tipsy-inner"/></div>'
        });

        o.bindClick();
        o.setDefault();
        bmoon.utl.loadJS('http://api.map.baidu.com/api?v=1.3&callback=bmoon.index.onMapReady');
    },

    onMapReady: function() {
        var o = bmoon.index.init();

        if (bmoon.dida.c_city) {
            if (bmoon.dida.c_city.id != 0) {
                o.initMap(bmoon.dida.dbpoint2ll(bmoon.dida.c_city.geopos));
                o.city = bmoon.dida.c_city.s;
            } else o.initMap();
        } else {
            //$.getJSON('/json/city/ip', {ip: '118.145.22.78'}, function(data) {
            $.getJSON('/json/city/ip', null, function(data) {
                if (data.success == 1 && bmoon.utl.type(data.citys) == 'Array') {
                    bmoon.dida.setCityCookie(data.citys);
                    o.initMap(bmoon.dida.dbpoint2ll(data.citys[0].geopos));
                } else o.initMap();
            });
        }
    },

    bindClick: function() {
        var o = bmoon.index.init();

        o.calendar.bind("onShow onHide", function()  {
            $(this).parent().toggleClass("active"); 
        });

        o.e_mc_no_repeat.change(o.wdayChanged);
        o.e_submit.click(o.matchPlan);
        o.e_mc_no_submit.click(o.leavePlan);
        o.e_mc_prev.click(function() {o.rendPlan(o._pcur-1);});
        o.e_mc_next.click(function() {o.rendPlan(o._pcur+1);});
        o.e_mc_continue.click(function() {o.e_mc_noresult.toggleClass('hide');});
        o.e_mc_no_subscribes.click(function() {
            if ($(this).attr('checked') == 'checked') {
                if ($(this).val() == 1) {
                    if (!o.e_mc_no_phone.val().length) {
                        $.getJSON('/json/member/info', null, function(data) {
                            if (data.success == 1)
                                o.e_mc_no_phone.val(data.member.phone);
                        });
                    }
                }
                if (!bmoon.dida.loginCheck()) {
                    bmoon.dida.loginhint.html('订阅线路需要登录');
                    bmoon.dida.reloadAfterLogin = false;
                    bmoon.dida.loginoverlay.load();
                }
                if (!$('.' + $(this).attr('inpcheck')).inputval()) {
                    $(this).removeAttr('checked');
                    return;
                }
            }
        });
        o.e_mc_del.click(o.delPlan);
        o.e_saddr.bind('blur', o.checkAddrInput);
        o.e_eaddr.bind('blur', o.checkAddrInput);
    },

    setDefault: function() {
        var o = bmoon.index.init();

        o.e_mc_no_contact.val(bmoon.dida.c_mname);
        o.e_mc_no_nick.val(bmoon.dida.c_mnick);
    },

    checkAddrInput: function() {
        var o = bmoon.index.init();

        var me = $(this),
        id = me.attr('id');
        
        setTimeout(function() {
            var val = me.val(),
            ll = id == 'saddr' ? o.plan.sll : o.plan.ell,
            addr = id == 'saddr' ? o.e_saddr : o.e_eaddr;
            if (val.length && !ll) {
                addr.val('请在下拉框中选择地址');
                addr.mnblink({
                    blinks: 2,
                    callback: function() {addr.val('');}
                });
            }
        }, 1500);
    },

    wdayChanged: function() {
        var o = bmoon.index.init();

        if (o.e_mc_no_repeat.val() == 2) {
            o.e_mc_no_wday.removeClass('hide');
        } else {
            o.e_mc_no_wday.addClass('hide');
        }
    },

    matchPlan: function() {
        var o = bmoon.index.init();

        var p = $(this).parent(),
        plan = o.plan;

        o.e_mc_noresult.addClass('hide');
        o.e_mc_result.addClass('hide');
        o.e_mc_nav.addClass('hide');
        //o.g_prect.setMap(null);
        o.mplans = {};
        
        if (!plan.sll) {
            o.e_saddr.focus();
            return;
        }
        if (!plan.ell) {
            o.e_eaddr.focus();
            return;
        }

        plan.dad  = o.e_dad.val();
        //plan.date = o.e_dateinput.val();
        plan.date = $('#date-input').val();
        //o.dayow = xxx;
        plan.time = o.e_hour.val() + ':' + o.e_min.val() + ':00';
        plan.rect = '((' + plan.sll.join(',') + '),(' + plan.ell.join(',') + '))';
        plan.sgeo = '(' + plan.sll.join(',') + ')';
        plan.egeo = '(' + plan.ell.join(',') + ')';
        plan.km = bmoon.utl.earthDis(plan.sll, plan.ell);
        //plan.sll = [212.12, 232.33];
        //plan.ell = [212.12, 232.33];
        //plan.saddr = ... plan.scity = ..
        //plan.eaddr = ... plan.ecity = ..
        //plan.scityid = x plan.ecityid = x

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        
        $.getJSON('/json/plan/match', plan, function(data) {
            p.removeClass('loading');
            if (data.success == '1' && data._ntt > 0 &&
                bmoon.utl.type(data.plans) == 'Array') {
                p.addClass('success');

                o._pnum = data._ntt ? data._ntt: data.plans.length;
                o.mplans = data.plans;
                o.e_mc_num_nav.html('0 / ' + o._pnum);
                o.rendPlan(0);
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg || ' ' + '</span>').appendTo(p);
                if (data._ntt == 0) o.e_mc_noresult.removeClass('hide');
            }
        });
    },

    leavePlan: function() {
        var o = bmoon.index.init();
        
        if (!$('.VAL_LEAVE').inputval()) return;

        var p = $(this).parent(),
        plan = o.plan;

        plan.repeat = o.e_mc_no_repeat.val();
        if (plan.repeat == 2) {
            var os = $('input[name=wday]:checked'),
            days = [];
            $.each(os, function(i, o) {
                days.push($(o).val());
            });
            plan.sdate = days.join(',');
        } else plan.sdate = plan.date;
        if (o.e_dad.val() == 0) plan.dad = 1; // if i lookup man, i'm a car
        else plan.dad = 0;
        plan.stime = plan.time;
        plan.phone = o.e_mc_no_phone.val();
        plan.contact = o.e_mc_no_contact.val();
        plan.nick = o.e_mc_no_nick.val().length ? o.e_mc_no_nick.val(): '嘀嗒网友';
        plan.attach = o.e_mc_no_attach.val();
        plan.subscribe = 0;
        $.each($('input[name=subscribe]:checked'), function(i, o) {
            plan.subscribe += parseInt($(this).val());
        });

        var pdata = {
            _op: 'add',
            plan: JSON.stringify(o.plan),
            _type_object: 'plan',
        };

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/plan/leave', pdata, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                //o.e_mc_no_submit.attr('disabled', 'disabled');
                p.addClass('success');
                $('<span class="vres">提交成功，期待有人联系你:D</span>').appendTo(p);
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');
    },

    rendPlan: function(ncur) {
        var o = bmoon.index.init();

        var plan = o.mplans[ncur],
        geo = bmoon.dida.dbbox2ll(plan.rect);
        
        o.g_prect.setPath([
            new BMap.Point(geo[0][1], geo[0][0]),
            new BMap.Point(geo[1][1], geo[1][0])
        ]);
        
        //o.g_map.addOverlay(o.g_prect);
        o.rendMatch(plan, ncur);

        bmoon.dida.tracePageview({
            es_two: 'http://imdida.org/plan/info?id=' + plan.id,
            es_three: '线路详情'
        });
    },

    delPlan: function() {
        var o = bmoon.index.init();

        var plan = o.mplans[o._pcur],
        p = $(this).parent(),
        pdata = {
            _op: 'mod',
            id: plan.id,
            statu: bmoon.dida.planstatu.del
        };

        $.getJSON('/json/plan/mine', pdata, function(data) {
            if (data.success == '1') {
                noty({text: '感谢您举报垃圾线路，稍后该线路将不会被搜到。', type: 'success', theme: 'noty_theme_mitgux'});
            } else {
                noty({text: data.errmsg, type: 'error', theme: 'noty_theme_mitgux'});
            }
        });
    },

    rendMatch: function(plan, ncur) {
        var o = bmoon.index.init();

        var domains = bmoon.dida.odomain;
        o._pcur = ncur;
        if (ncur > 0) o.e_mc_prev.removeClass('hide');
        else o.e_mc_prev.addClass('hide');
        if (ncur < o._pnum-1) o.e_mc_next.removeClass('hide');
        else o.e_mc_next.addClass('hide');

        if (plan.mid == 0) {
            o.e_mc_nick.html(plan.nick);
        } else {
            o.e_mc_nick.html('<a target="_blank" href="/member/home?mid='+plan.mid+'">'+plan.nick+'</a>');
        }
        o.e_mc_attach.html(plan.attach);
        o.e_mc_num_nav.html(ncur+1 + ' / ' + o._pnum);
        o.e_mc_saddr.html(plan.saddr);
        o.e_mc_eaddr.html(plan.eaddr);
        if (plan.repeat == 1) {
            o.e_mc_sdate.html('每日');
        } else if (plan.repeat == 2) {
            o.e_mc_sdate.html('每周 ' + plan.sdate);
        } else o.e_mc_sdate.html(plan.sdate);
        o.e_mc_stime.html(plan.stime);
        o.e_mc_planurl.attr('href', '/plan/info?id='+plan.id);

        o.e_mc_nav.removeClass('hide');
        o.e_mc_result.removeClass('hide');

        if (bmoon.dida.c_mnick && plan.mid == 0) o.e_mc_del.removeClass('hide');
        else o.e_mc_del.addClass('hide');
        
        o.e_mc_from.empty();
        if (plan.ori > 0) {
            if (plan.ourl) o.e_mc_from.attr('href', plan.ourl);
            else o.e_mc_from.attr('href', domains[plan.ori].url);
            o.e_mc_from.attr('title', '数据来源：　' + domains[plan.ori].title);
            $('<img/>').attr('src', domains[plan.ori].img).appendTo(o.e_mc_from);
        }
        
        o.e_mc_phone.attr('src', '/image/plan/pic?defs=%20&type=phone&id=' +
                          plan.id);
        o.e_mc_contact.attr('src', '/image/plan/pic?defs=%20&type=contact&id=' +
                            plan.id);
    },

    // {"province":"","city":"长沙市","district":"开福区","street":"","streetNumber":"","business":"开福寺"}
    upPlan: function(x, data) {
        var o = bmoon.index.init();

        var p = o.plan,
        s = o._strFromPoi(data);

        if (x != 'e') p.saddr = s;
        else p.eaddr = s;

        if (data.point) {
            if (x != 'e') p.sll = [data.point.lat, data.point.lng];
            else {
                p.ell = [data.point.lat, data.point.lng];
                o.rendDirect();
            }
        } else {
            o.g_geocode.getPoint(s, function(point) {
                if (!point) return;
                if (x != 'e') p.sll = [point.lat, point.lng];
                else {
                    p.ell = [point.lat, point.lng];
                    o.rendDirect();
                }
            }, o.city);
        }

        bmoon.dida.getCityByPoi(data, function(city) {
            if (bmoon.utl.type(city) == 'Array') {
                if (x != 'e') p.scityid = city[0].id;
                else p.ecityid = city[0].id;
            }
        });
    },

    rendDirect: function() {
        var o = bmoon.index.init();

        if (!o.plan.sll || !o.plan.ell) return;

        var km = bmoon.utl.earthDis(o.plan.sll, o.plan.ell);

        o.e_km.html('约 '+ km +' 千米');

        if (km > 10 && km < 100) o.g_map.setZoom(12);
        else if (km > 100 && km < 300) o.g_map.setZoom(9);
        else if (km > 300 && km < 600) o.g_map.setZoom(7);
        else if (km > 600) o.g_map.setZoom(6);

        o.g_direct.setPath([
            new BMap.Point(o.plan.sll[1], o.plan.sll[0]),
            new BMap.Point(o.plan.ell[1], o.plan.ell[0])
        ]);
        o.g_map.addOverlay(o.g_direct);
    },

    bindMapChange: function() {
        var o = bmoon.index.init();

        o.g_sauto.addEventListener('onconfirm', function(res) {
            o.autoChanged('s', res);
        });
        o.g_eauto.addEventListener('onconfirm', function(res) {
            o.autoChanged('e', res);
        });
        
        o.g_smarker.addEventListener('dragend', function(e) {
            o.markDraged('s');
        });
        o.g_emarker.addEventListener('dragend', function(e) {
            o.markDraged('e');
        });
    },

    markDraged: function(x) {
        var o = bmoon.index.init();

        var marker = o.g_smarker,
        addr = o.e_saddr;
        if (x == 'e') {
            marker = o.g_emarker;
            addr = o.e_eaddr;
        }

        var pos = marker.getPosition();
        
        o.g_geocode.getLocation(pos, function(result) {
            if (result.surroundingPois && result.surroundingPois.length > 0) {
                var poi = result.surroundingPois[0];
                addr.val(poi.address + poi.title);
                result.addressComponents.point = poi.point;
            } else {
                addr.val(result.address);
                result.addressComponents.point = result.point;
            }
            o.upPlan(x, result.addressComponents);

            o.g_infow.setTitle(o._formInfoTitle(x));
            o.g_infow.setContent(o._formInfoContent(result));

            marker.openInfoWindow(o.g_infow);
            //setTimeout(function() {marker.closeInfoWindow();}, 3000);
        });
    },
    
    // res.item = {"index":2,"value":{"province":"","city":"长沙市","district":"开福区","street":"","streetNumber":"","business":"开福寺"}}}
    autoChanged: function(x, res) {
        var o = bmoon.index.init();

        var marker = o.g_smarker,
        p = o.plan,
        s = o._strFromPoi(res.item.value);

        if (x == 'e') {
            marker = o.g_emarker;
        } else {
            o.e_maphint.removeClass('hide');
        }

        o.g_geocode.getPoint(s, function(point) {
            if (!point) return;
            marker.setPosition(point);
            o.g_map.centerAndZoom(point, 13);
            o.g_map.addOverlay(marker);

            o.g_infow.setTitle(o._formInfoTitle(x));
            o.g_infow.setContent(o._strFromPoi(res.item.value));

            marker.openInfoWindow(o.g_infow);
            //setTimeout(function() {marker.closeInfoWindow();}, 2000);
        }, o.city);

        o.upPlan(x, res.item.value);
    }
};

$(document).ready(bmoon.index.onready);
