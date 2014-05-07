; var bmoon = bmoon || {};
bmoon.index = {
    version: '1.0',

    _formAddr: function(name, addrs, loc) {
        if (addrs) {
            return '<div><strong>' + name + '</strong><br />' +
                [(addrs[0] &&
                  addrs[0].short_name || ''),
                 (addrs[1] &&
                  addrs[1].short_name || ''),
                 (addrs[2] &&
                  addrs[2].short_name || '')
                ].join(' ') +
                '（' + loc.lat().toFixed(4) + ', ' + loc.lng().toFixed(4) + '）';
        } else return ' ';
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
        o.g_lat = new google.maps.LatLng(geo[0], geo[1]);
        o.g_map = new google.maps.Map($('#map')[0], {
            zoom: 10,
            center: o.g_lat,
            mapTypeId: google.maps.MapTypeId.ROADMAP,
            region: 'zh-CN'
        });

        o.g_sauto = new google.maps.places.Autocomplete($('#saddr')[0]);
        o.g_eauto = new google.maps.places.Autocomplete($('#eaddr')[0]);
        o.g_geocode = new google.maps.Geocoder();
        o.g_dirservice = new google.maps.DirectionsService();
        o.g_dirrender = new google.maps.DirectionsRenderer();
        o.g_prect = new google.maps.Rectangle(); // matched plan's rect
        o.g_infow = new google.maps.InfoWindow();
        o.g_smarker = new google.maps.Marker({
            map: o.g_map,
            draggable: true,
            title: '设置起始位置'
        });
        o.g_emarker = new google.maps.Marker({
            map: o.g_map,
            draggable: true,
            title: '设置终点位置'
        });

        o.g_sauto.bindTo('bounds', o.g_map);
        o.g_eauto.bindTo('bounds', o.g_map);
        o.g_dirrender.setMap(o.g_map);
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
        
        //$.getJSON('/json/city/ip', {ip: '118.145.22.78'}, function(data) {
        $.getJSON('/json/city/ip', null, function(data) {
            if (data.success == 1 && bmoon.utl.type(data.city) == 'Object') {
                o.initMap(bmoon.dida.dbpoint2ll(data.city.geopos));
            } else o.initMap();
        });

        o.bindClick();
        o.setDefault();
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
        o.e_mc_continue.click(function() {o.e_mc_noresult.toggle();});
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
                if (!$('.' + $(this).attr('inpcheck')).inputval()) {
                    $(this).removeAttr('checked');
                    return;
                }
                if (!bmoon.dida.loginCheck()) {
                    bmoon.dida.loginhint.html('订阅线路需要登录');
                    bmoon.dida.reloadAfterLogin = false;
                    bmoon.dida.loginoverlay.load();
                }
            }
        });
    },

    setDefault: function() {
        var o = bmoon.index.init();

        o.e_mc_no_contact.val(bmoon.dida.loginmname.val());
        o.e_mc_no_nick.val(bmoon.dida.mnick.text());
    },

    wdayChanged: function() {
        var o = bmoon.index.init();

        if (o.e_mc_no_repeat.val() == 2) {
            o.e_mc_no_wday.show();
        } else {
            o.e_mc_no_wday.hide();
        }
    },

    matchPlan: function() {
        var o = bmoon.index.init();

        var p = $(this).parent(),
        plan = o.plan;

        o.e_mc_noresult.fadeOut();
        o.e_mc_result.fadeOut();
        o.e_mc_nav.fadeOut();
        o.g_prect.setMap(null);
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
            if (data.success == '1' && bmoon.utl.type(data.plans) == 'Array') {
                p.addClass('success');

                o._pnum = data._ntt ? data._ntt: data.plans.length;
                o.mplans = data.plans;
                o.e_mc_num_nav.html('0 / ' + o._pnum);
                o.rendPlan(0);
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg + '</span>').appendTo(p);
                if (data.errcode == 35) o.e_mc_noresult.fadeIn();
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
            _type_plan: 'object'
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
        geo = bmoon.dida.dbbox2ll(plan.rect),
        bounds = new google.maps.LatLngBounds(
          new google.maps.LatLng(geo[0][0], geo[0][1]),
          new google.maps.LatLng(geo[1][0], geo[1][1])
        );

        o.g_prect.setOptions({
            strokeColor: "#008888",
            strokeOpacity: 0.8,
            strokeWeight: 2,
            fillColor: "#aaaaaa",
            fillOpacity: 0.35,
            bounds: bounds,
            map: o.g_map
        });

        o.rendMatch(plan, ncur);
    },

    rendMatch: function(plan, ncur) {
        var o = bmoon.index.init();

        var domains = bmoon.dida.odomain;
        o._pcur = ncur;
        if (ncur > 0) o.e_mc_prev.show();
        else o.e_mc_prev.hide();
        if (ncur < o._pnum-1) o.e_mc_next.show();
        else o.e_mc_next.hide();
        
        o.e_mc_nick.html(plan.nick);
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

        o.e_mc_nav.fadeIn();
        o.e_mc_result.fadeIn('slow');
        
        o.e_mc_from.empty();
        if (plan.ori > 0) {
            if (plan.ourl) o.e_mc_from.attr('href', plan.ourl);
            else o.e_mc_from.attr('href', domains[plan.ori].url);
            o.e_mc_from.attr('title', '数据来源：　' + domains[plan.ori].title);
            $('<img/>').attr('src', domains[plan.ori].img).appendTo(o.e_mc_from);
        }
        
        o.e_mc_phone.attr('src', '/image/plan/pic?defs=segv&type=phone&id=' +
                          plan.id);
        o.e_mc_contact.attr('src', '/image/plan/pic?defs=segv&type=contact&id=' +
                            plan.id);
    },

    // {address_components: [], formatted_address: "", geometry: {}...} gdata.js
    upPlan: function(x, data) {
        var o = bmoon.index.init();

        var p = o.plan;

        if (x != 'e') {
            p.sll = [data.geometry.location.lat(), data.geometry.location.lng()];
            p.saddr = data.formatted_address;
        } else {
            p.ell = [data.geometry.location.lat(), data.geometry.location.lng()];
            p.eaddr = data.formatted_address;
        }

        bmoon.dida.getCityByGeoresult(data, function(city) {
            if (bmoon.utl.type(city) == 'Object') {
                if (x != 'e') p.scityid = city.id;
                else p.ecityid = city.id;
            }
        });
    },

    rendDirect: function() {
        var o = bmoon.index.init();

        if (!o.plan.sll || !o.plan.ell) return;

        o.e_km.html('约 '+ bmoon.utl.earthDis(o.plan.sll, o.plan.ell) +' 千米');
        
        var opts = {
            origin: new google.maps.LatLng(o.plan.sll[0], o.plan.sll[1]),
            destination: new google.maps.LatLng(o.plan.ell[0], o.plan.ell[1]),
            travelMode: google.maps.TravelMode.DRIVING
        };
        
        o.g_dirservice.route(opts, function(result, status) {
            if (status == google.maps.DirectionsStatus.OK) {
                o.g_dirrender.setDirections(result);
            }
        });
    },

    bindMapChange: function() {
        var o = bmoon.index.init();

        google.maps.event.addListener(o.g_smarker, 'dragend', function() {
            o.markDraged('s');
        });
        google.maps.event.addListener(o.g_emarker, 'dragend', function() {
            o.markDraged('e');
        });

        google.maps.event.addListener(o.g_sauto, 'place_changed', function() {
            o.autoChanged('s');
        });
        google.maps.event.addListener(o.g_eauto, 'place_changed', function() {
            o.autoChanged('e');
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
        o.g_geocode.geocode({latLng: pos}, function(results, status) {
            if (status == google.maps.GeocoderStatus.OK) {
                addr.val(results[0].formatted_address);
                o.g_infow.setContent(o._formAddr(
                    results[0].address_components[0].short_name,
                    results[0].address_components,
                    results[0].geometry.location));
                o.g_infow.open(o.g_map, marker);
                setTimeout(function() {o.g_infow.close();}, 2000);

                o.upPlan(x, results[0]);
                o.rendDirect();
            }
        });
    },

    autoChanged: function(x) {
        var o = bmoon.index.init();
        
        var marker = o.g_smarker,
        auto = o.g_sauto;
        
        if (x == 'e') {
            marker = o.g_emarker;
            auto = o.g_eauto;
        }

        o.g_infow.close();
        var place = auto.getPlace();
        if (place.geometry.viewport) {
            o.g_map.fitBounds(place.geometry.viewport);
        } else {
            o.g_map.setCenter(place.geometry.location);
        }
        
        marker.setPosition(place.geometry.location);
        
        o.g_infow.setContent(o._formAddr(
            place.name,
            place.address_components,
            place.geometry.location));
        o.g_infow.open(o.g_map, marker);
        setTimeout(function() {o.g_infow.close();}, 2000);

        o.upPlan(x, place);
        o.rendDirect();
    }
};

$(document).ready(bmoon.index.onready);
