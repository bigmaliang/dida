; var bmoon = bmoon || {};
bmoon.planmine = {
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
    
    _strSdate: function(p) {
        if (p.repeat == bmoon.dida.planrepeat.none)
            return p.sdate;
        else if (p.repeat == bmoon.dida.planrepeat.day)
            return '每日';
        else if (p.repeat == bmoon.dida.planrepeat.week)
            return '每周';
    },

    _planFromBody: function(body) {
        var plan = {
            id: body.attr('rel'),
            saddr: $('span[name="saddr"]', body).html(),
            eaddr: $('span[name="eaddr"]', body).html(),
            sdate: $('span[name="sdate"]', body).html(),
            stime: $('span[name="stime"]', body).html(),
            phone: $('span[name="phone"]', body).html(),
            contact: $('span[name="contact"]', body).html(),
            attach: $('span[name="attach"]', body).html()
        };

        return plan;
    },
    
    _planFromBodyRW: function(body) {
        var plan = {
            id: body.attr('rel'),
            saddr: $('span[name="saddr"]', body).html(),
            eaddr: $('span[name="eaddr"]', body).html(),
            sdate: $('input[name="sdate"]', body).val(),
            stime: $('input[name="stime"]', body).val(),
            phone: $('input[name="phone"]', body).val(),
            contact: $('input[name="contact"]', body).val(),
            attach: $('textarea[name="attach"]', body).val()
        };

        return plan;
    },
    
    _strPlanRead: function(p) {
        return [
            '<dt>路线：</dt>',
            '<dd>',
                '<span name="saddr">', p.saddr, '</span>', ' - ',
                '<span name="eaddr">', p.eaddr, '</span>',
            '</dd>',

            '<dt>出发时间：</dt>',
            '<dd>',
                '<span name="sdate">', p.sdate, '</span>',
                '&nbsp;<span name="stime">', p.stime, '</span>',
            '</dd>',

            '<dt>联系方式：</dt>',
            '<dd>',
                '<span name="phone">', p.phone, '</span>',
                '&nbsp;<span name="contact">', p.contact, '</span>',
            '</dd>',

            '<dt>留言：</dt>',
            '<dd>',
                '<pre><span name="attach">', p.attach, '</span></pre>',
            '</dd>'
        ].join('');
    },

    _strPlanReadWrite: function(p) {
        return [
            '<dt>路线：</dt>',
            '<dd>',
                '<span name="saddr">', p.saddr, '</span>', ' - ',
                '<span name="eaddr">', p.eaddr, '</span>',
                '<span class="slight">暂不支持修改路线</span>',
            '</dd>',

            '<dt>出发时间：</dt>',
            '<dd>',
                '<input name="sdate" type="text" id="saddr-', p.id, '" value="', p.sdate, '" />',
                ' &nbsp; <input name="stime" type="text" id="stime-', p.id, '" value="', p.stime, '" />',
            '</dd>',

            '<dt>联系方式：</dt>',
            '<dd>',
                '<input name="phone" type="text" id="phone-', p.id, '" value="', p.phone, '" />',
                ' &nbsp; <input name="contact" type="text" id="contact-', p.id, '" value="', p.contact, '" />',
            '</dd>',

            '<dt>留言：</dt>',
            '<dd>',
                '<textarea name="attach">', p.attach, '</textarea>',
            '</dd>'
        ].join('');
    },

    _strPlanLi: function(p) {
        var o = bmoon.planmine.init();

        return [
            '<li>',
                '<h3>', p.nick, ' <span class="slight">刚刚创建</span>', '</h3>',
                '<div class="body">',
                    '<dl class="inp">', o._strPlanRead(p), '</dl>',
                    '<div class="clearer"></div>',
                '</div>',
            '</li>'
        ].join('');
    },

    init: function() {
        var o = bmoon.planmine;
        if (o.inited) return o;

        o.e_tips = $('input, a', '#plan-list .op');
        o.e_subsets = $('#plan-list .op input[name="set-sub"]');
        o.e_pause = $('#plan-list .op a[name="pause"]');
        o.e_modify = $('#plan-list .op a[name="modify"]');
        o.e_cancel = $('#plan-list .op a[name="cancel"]');
        o.e_rebirth = $('#plan-list .op a[name="rebirth"]');

        o.e_plan_count = $('#plan-count');
        o.e_plan_list = $('#plan-list');
        o.e_plan_create = $('#plan-create');
        o.e_plan_new = $('#plan-new');

        o.e_pnew_datehint = $('#pnew-datehint');
        o.e_pnew_repeat = $('#pnew-repeat');
        o.e_pnew_sdate = $('#pnew-sdate');
        o.e_pnew_hour = $('#pnew-hour');
        o.e_pnew_min = $('#pnew-min');
        o.e_pnew_dad = $('#pnew-dad');
        o.e_pnew_wday = $('#pnew-wday');
        o.e_pnew_saddr = $('#pnew-saddr');
        o.e_pnew_eaddr = $('#pnew-eaddr');
        o.e_pnew_nick = $('#pnew-nick');
        o.e_pnew_phone = $('#pnew-phone');
        o.e_pnew_contact = $('#pnew-contact');
        o.e_pnew_km = $('#pnew-km');
        o.e_pnew_attach = $('#pnew-attach');
        o.e_pnew_subscribes = $('input[name=subscribe]', o.e_plan_new);
        o.e_pnew_submit = $('#pnew-submit');

        o.e_plan_new.data('_postData', {});

        o.e_nav = $('#plan-nav');

        o.inited = true;
        return o;
    },

    initMap: function(geo) {
        var o = bmoon.planmine.init();

        geo = geo || [28.188,113.033];
        o.g_lat = new BMap.Point(geo[1], geo[0]);
        o.g_map = new BMap.Map("map");
        o.g_map.enableScrollWheelZoom();
        o.g_map.centerAndZoom(o.g_lat, 13);

        o.g_sauto = new BMap.Autocomplete({
            location: o.g_map,
            input: 'pnew-saddr'});
        o.g_eauto = new BMap.Autocomplete({
            location: o.g_map,
            input: 'pnew-eaddr'});

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

        o.map_inited = true;
    },

    onready: function() {
        var o = bmoon.planmine.init();

        $.tools.dateinput.localize("zh", bmoon.dida.dateinputzh);
        o.calendar = o.e_pnew_sdate.dateinput({
            format: 'yyyy-mm-dd',
            lang: 'zh',
            trigger: true,
            firstDay: 1,
            change: function(e, date) {
                o.e_pnew_datehint.html(this.getValue('dddd'))
            }
        });
        
        o.e_tips.tooltip({
            tipClass: 'tipsy tipsy-south',
            layout: '<div><div class="tipsy-inner"/></div>'
        });

        o.bindClick();

        bmoon.utl.after(o.rendNav, 'mgd.ntt != undefined', 10);
    },

    onMapReady: function() {
        var o = bmoon.planmine.init();

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
        var o = bmoon.planmine.init();

        o.calendar.bind("onShow onHide", function()  {
	        $(this).parent().toggleClass("active"); 
        });
        
        o.e_subsets.click(o.setPlanSub);
        o.e_pause.click(o.pausePlan);
        o.e_modify.click(o.modifyPlan);
        o.e_cancel.click(o.cancelPlan);
        o.e_rebirth.click(o.rebirthPlan);

        o.e_plan_create.click(o.showPlanNew);
        o.e_pnew_repeat.change(o.wdayChanged);
        o.e_pnew_submit.click(o.leavePlan);
        o.e_pnew_subscribes.click(function() {
            if ($(this).attr('checked') == 'checked') {
                if (!$('.' + $(this).attr('inpcheck')).inputval()) {
                    $(this).removeAttr('checked');
                    return;
                }
            }
        });
        o.e_pnew_saddr.bind('blur', o.checkAddrInput);
        o.e_pnew_eaddr.bind('blur', o.checkAddrInput);
    },

    rendNav: function() {
        var o = bmoon.planmine.init();

        if (mgd.ntt > mgd.npp) {
            o.e_nav.mnnav({
                ntt: mgd.ntt,
                npg: mgd.npg,
                npp: mgd.npp,
                url: '/plan/mine'
            });
        }
    },

    bindMapChange: function() {
        var o = bmoon.planmine.init();

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

    checkAddrInput: function() {
        var o = bmoon.planmine.init();

        var me = $(this),
        id = me.attr('id'),
        p = o.e_plan_new.data('_postData');
        
        setTimeout(function() {
            var val = me.val(),
            ll = id == 'pnew-saddr' ? p.sll : p.ell,
            addr = id == 'pnew-saddr' ? o.e_pnew_saddr : o.e_pnew_eaddr;
            if (val.length && !ll) {
                addr.val('请在下拉框中选择地址');
                addr.mnblink({
                    blinks: 2,
                    callback: function() {addr.val('');}
                });
            }
        }, 1500);
    },

    setPlanSub: function() {
        var o = bmoon.planmine.init();

        var id = $(this).attr('rel'),
        name = $(this).attr('name'),
        sub = bmoon.dida.plansub[name],
        checkop = $(this).attr('checked') == 'checked' ? 1: 0,
        p = $(this).parent().parent();

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.getJSON('/json/plan/mine', {
            _op: 'mod',
            id: id,
            subscribe: sub,
            checkop: checkop
        }, function(data) {
            p.removeClass('loading');
            if (data.success == '1') {
                p.addClass('success');
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        });
    },

    pausePlan: function() {
        var o = bmoon.planmine.init();

        var id = $(this).attr('rel'),
        me = $(this),
        echeck = $('input[type="checkbox"]', me.parent()),
        erebirth = $('a[name="rebirth"]', me.parent()),
        emodify = $('a[name="modify"]', me.parent()),
        ecancel = $('a[name="cancel"]', me.parent()),
        p = me.parent().parent(),
        body = $('div.body dl.inp', p);

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.getJSON('/json/plan/mine', {
            _op: 'mod',
            id: id,
            statu: bmoon.dida.planstatu.pause
        }, function(data) {
            p.removeClass('loading');
            if (data.success == '1') {
                if (body.data('_edit')) {
                    body.html(o._strPlanRead(o._planFromBodyRW(body)));
                }
                p.addClass('success').addClass('paused');
                me.addClass('hide');
                echeck.attr('disabled', 'disabled');
                emodify.addClass('hide');
                erebirth.removeClass('hide');
                ecancel.addClass('hide');
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        });
    },

    modifyPlan: function() {
        var o = bmoon.planmine.init();

        var me = $(this),
        ecancel = $('a[name="cancel"]', me.parent()),
        pp = me.parent().parent(),
        body = $('div.body dl.inp', pp),
        plan = o._planFromBody(body);

        body.data('_originHtml', body.html());
        body.html(o._strPlanReadWrite(plan));
        body.data('_postData', {id: me.attr('rel')});
        body.data('_edit', true);

        me.html('<span>保存</span>').addClass('disabled').unbind('click');
        ecancel.removeClass('hide');

        $('input, textarea', body).change(function() {
            body.data('_postData')[$(this).attr('name')] = $(this).val();
            me.removeClass('disabled');
            me.unbind('click').click(o.savePlan);
        });
    },

    cancelPlan: function() {
        var o = bmoon.planmine.init();

        var me = $(this),
        emodify = $('a[name="modify"]', me.parent()),
        pp = me.parent().parent(),
        body = $('div.body dl.inp', pp);

        body.html(body.data('_originHtml'));
        body.data('_edit', false);

        me.addClass('hide');
        emodify.html('<span>修改</span>').removeClass('disabled').unbind('click').click(o.modifyPlan);
    },

    savePlan: function() {
        var o = bmoon.planmine.init();

        var me = $(this),
        ecancel = $('a[name="cancel"]', me.parent()),
        p = me.parent().parent(),
        body = $('div.body dl.inp', p);

        var pdata = body.data('_postData');
        pdata._op = 'mod';

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/plan/mine', pdata, function(data) {
            p.removeClass('loading');
            if (data.success == '1') {
                p.addClass('success');
                body.html(o._strPlanRead(o._planFromBodyRW(body)));
                me.html('<span>修改</span>').unbind('click').click(o.modifyPlan);
                ecancel.addClass('hide');
                body.data('_edit', false);
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');

    },

    rebirthPlan: function() {
        var o = bmoon.planmine.init();

        var id = $(this).attr('rel'),
        p = $(this).parent().parent(),
        me = $(this),
        echeck = $('input[type="checkbox"]', me.parent()),
        epause = $('a[name="pause"]', me.parent()),
        emodify = $('a[name="modify"]', me.parent());

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.getJSON('/json/plan/mine', {
            _op: 'mod',
            id: id,
            statu: bmoon.dida.planstatu.fresh
        }, function(data) {
            p.removeClass('loading');
            if (data.success == '1') {
                p.addClass('success').removeClass('paused');
                me.addClass('hide');
                echeck.removeAttr('disabled');
                epause.removeClass('hide');
                emodify.html('<span>修改</span>').removeClass('hide');
                emodify.removeClass('disabled').unbind('click').click(o.modifyPlan);
            } else {
                p.addClass('error');
                $('<span class="vres">' + data.errmsg + '</span>').appendTo(p);
            }
        });
    },

    showPlanNew: function() {
        var o = bmoon.planmine.init();

        !o.map_inited && bmoon.utl.loadJS('http://api.map.baidu.com/api?v=1.3&callback=bmoon.planmine.onMapReady');
        
        o.e_plan_new.toggleClass('hide');
    },

    wdayChanged: function() {
        var o = bmoon.planmine.init();

        var v = o.e_pnew_repeat.val();
        if (v == 0) {
            o.e_pnew_wday.addClass('hide');
            $('#pnew-sdate').removeAttr('disabled').removeClass('hide');
            $('a.caltrigger').removeClass('hide');
            o.e_pnew_datehint.removeClass('hide');
        } else if (v == 1) {
            o.e_pnew_wday.addClass('hide');
            $('#pnew-sdate').attr('disabled', 'disabled').addClass('hide');
            $('a.caltrigger').addClass('hide');
            o.e_pnew_datehint.addClass('hide');
        } else if (v == 2) {
            o.e_pnew_wday.removeClass('hide');
            $('#pnew-sdate').attr('disabled', 'disabled').addClass('hide');
            $('a.caltrigger').addClass('hide');
            o.e_pnew_datehint.addClass('hide');
        }
    },
    
    markDraged: function(x) {
        var o = bmoon.planmine.init();

        var marker = o.g_smarker,
        addr = o.e_pnew_saddr;
        if (x == 'e') {
            marker = o.g_emarker;
            addr = o.e_pnew_eaddr;
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
        var o = bmoon.planmine.init();

        var marker = o.g_smarker,
        s = o._strFromPoi(res.item.value);

        if (x == 'e') {
            marker = o.g_emarker;
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
    },

    // {"province":"","city":"长沙市","district":"开福区","street":"","streetNumber":"","business":"开福寺"}
    upPlan: function(x, data) {
        var o = bmoon.planmine.init();

        var p = o.e_plan_new.data('_postData'),
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
        var o = bmoon.planmine.init();

        var plan = o.e_plan_new.data('_postData');
        
        if (!plan.sll || !plan.ell) return;

        var km = bmoon.utl.earthDis(plan.sll, plan.ell);

        o.e_pnew_km.html('约 '+ km +' 千米');

        if (km > 10 && km < 100) o.g_map.setZoom(12);
        else if (km > 100 && km < 300) o.g_map.setZoom(9);
        else if (km > 300 && km < 600) o.g_map.setZoom(7);
        else if (km > 600) o.g_map.setZoom(6);

        o.g_direct.setPath([
            new BMap.Point(plan.sll[1], plan.sll[0]),
            new BMap.Point(plan.ell[1], plan.ell[0])
        ]);
        o.g_map.addOverlay(o.g_direct);
    },

    leavePlan: function() {
        var o = bmoon.planmine.init();

        if (!$('.VAL_LEAVE').inputval()) return;

        var plan = o.e_plan_new.data('_postData'),
        p = $(this).parent();

        $.each($('input, select, textarea', o.e_plan_new), function(i, o) {
            var name = $(this).attr('name'),
            val = $(this).val();

            if (name && name.length > 1 && val.length > 0) {
                plan[name] = val;
            }
        });

        if (plan.repeat == 2) {
            var os = $('input[name=wday]:checked'),
            days = [];
            $.each(os, function(i, o) {
                days.push($(o).val());
            });
            plan.sdate = days.join(',');
        }
        plan.subscribe = 0;
        $.each($('input[name=subscribe]:checked', o.e_plan_new), function(i, o) {
            plan.subscribe += parseInt($(this).val());
        });
        plan.stime = o.e_pnew_hour.val() + ':' + o.e_pnew_min.val() + ':00';
        plan.rect = '((' + plan.sll.join(',') + '),(' + plan.ell.join(',') + '))';
        plan.sgeo = '(' + plan.sll.join(',') + ')';
        plan.egeo = '(' + plan.ell.join(',') + ')';
        plan.km = bmoon.utl.earthDis(plan.sll, plan.ell);

        var pdata = {
            _op: 'add',
            plan: JSON.stringify(plan),
            _type_object: 'plan'
        };
        
        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/plan/leave', pdata, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                //o.e_pnew_submit.attr('disabled', 'disabled');
                p.addClass('success');
                var newp = $(o._strPlanLi(plan)).prependTo(o.e_plan_list);
                o.e_plan_count.html(parseInt(o.e_plan_count.html())+1);
                noty({text: '成功添加路线。', type: 'success', theme: 'noty_theme_mitgux'});
            } else {
                p.addClass('error');
                noty({text: data.errmsg, type: 'error', theme: 'noty_theme_mitgux'});
            }
        }, 'json');
    }
};

$(document).ready(bmoon.planmine.onready);
