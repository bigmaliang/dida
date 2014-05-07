; var bmoon = bmoon || {};
bmoon.spdpost = {
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
        var o = bmoon.spdpost;
        if (o.inited) return o;

        o.planinfo = $('#planinfo');
        o.saddr = $('#saddr');
        o.eaddr = $('#eaddr');
        o.sdate = $('#sdate');
        o.stime = $('#stime');
        o.planid = $('#planid');
        o.attach = $('#attach');
        o.ourl = $('#ourl');
        o.km = $('#km');
        o.marks = $('#marks');
        o.count = $('#count');
        o.phone = $('#phone');
        o.phoneimg = $('#img-phone');
        o.contact = $('#contact');
        o.contactimg = $('#img-contact');
        o.del = $('#del');
        o.next = $('#next');
        o.save = $('#save');
        o.map = $('#map');

        o.plan = {};
        o.nplan = {};           // plan for save

        //o.glat = new google.maps.LatLng(28.188, 113.033);
        o.g_lat = new BMap.Point(113.033, 28.188);
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
        o.g_infow = new BMap.InfoWindow('起点', {
            width: 250,
            height: 100
        });

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.spdpost.init();

        o.bindClick();
        o.getPlan();
    },

    bindClick: function() {
        var o = bmoon.spdpost.init();

        o.bindChange();
        o.del.click(o.delPlan);
        o.next.click(o.getPlan);
        o.save.click(o.savePlan);

        $('input', o.planinfo).change(function() {
            o.nplan[$(this).attr('id')] = $(this).val();
        });
    },

    getPlan: function() {
        var o = bmoon.spdpost.init();

        if (mgd._ntt <= 0) return;
        
        var opt = o.plan.id ? {exceptid: o.plan.id} : null;

        $.getJSON('/json/spd/post/do', opt, function(data) {
            if (data.success == 1 && bmoon.utl.type(data.plan) == 'Object') {
                o.nplan = {};

                var p = data.plan;
                o.saddr.val(p.saddr);
                o.eaddr.val(p.eaddr);
                o.sdate.val(p.sdate);
                o.stime.val(p.stime);
                o.planid.html(p.id);
                o.attach.html(p.attach);
                o.ourl.attr('href', p.ourl);
                o.plan = data.plan;
                o.phoneimg.attr('src', bmoon.utl.clotheHTML(data.plan.phone));
                o.contactimg.attr('src', bmoon.utl.clotheHTML(data.plan.contact));

                o.count.text(--mgd._ntt);
            } else {
                alert(data.errmsg || '获取数据失败，请稍后再试！');
            }
        });
    },

    // {address_components: [], formatted_address: "", geometry: {}...} gdata.js
    upPlan: function(x, data) {
        var o = bmoon.spdpost.init();

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

    delPlan: function() {
        var o = bmoon.spdpost.init();

        if (!o.plan) return;

        var nplan = {
            id: o.plan.id
        },
        pdata = {
            _op: 'del',
            plan: JSON.stringify(nplan),
            _type_object: 'plan'
        },
        p = $(this).parent();
        
        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/spd/post/do', pdata, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                if (mgd._ntt > 0) o.getPlan();
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg + '</span>').appendTo(p);
            }
        }, 'json');
    },

    savePlan: function() {
        var o = bmoon.spdpost.init();

        var p = $(this).parent();
        
        if (!o.plan.sll || !o.plan.ell) return;

        o.nplan.scityid = o.plan.scityid;
        o.nplan.ecityid = o.plan.ecityid;
        o.nplan.rect = '((' + o.plan.sll.join(',') + '),(' +
            o.plan.ell.join(',') + '))';
        o.nplan.sgeo = '(' + o.plan.sll.join(',') +')';
        o.nplan.egeo = '(' + o.plan.ell.join(',') +')';
        o.nplan.id = o.plan.id;

        var pdata = {
            _op: 'mod',
            plan: JSON.stringify(o.nplan),
            _type_object: 'plan'
        };
        
        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/spd/post/do', pdata, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                if (mgd._ntt > 0) o.getPlan();
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
            }
        }, 'json');
    },

    rendDirect: function() {
        var o = bmoon.spdpost.init();

        if (!o.plan.sll || !o.plan.ell) return;

        var km = bmoon.utl.earthDis(o.plan.sll, o.plan.ell);

        o.km.html('约 '+ km +' 千米');
        o.nplan.km = km;

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

    bindChange: function() {
        var o = bmoon.spdpost.init();

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
        var o = bmoon.spdpost.init();

        var marker = o.gsmarker,
        addr = o.saddr;
        if (x == 'e') {
            marker = o.gemarker;
            addr = o.eaddr;
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

    autoChanged: function(x, res) {
        var o = bmoon.spdpost.init();
        
        var marker = o.g_smarker,
        p = o.plan,
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
    }
};

$(document).ready(bmoon.spdpost.onready);
