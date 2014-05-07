; var bmoon = bmoon || {};
bmoon.spdpost = {
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
        var o = bmoon.spdpost;
        if (o.inited) return o;

        o.saddr = $('#saddr');
        o.eaddr = $('#eaddr');
        o.sdate = $('#sdate');
        o.stime = $('#stime');
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

        o.glat = new google.maps.LatLng(28.188, 113.033);
        o.gmap = new google.maps.Map($('#map')[0], {
            zoom: 10,
            center: o.glat,
            mapTypeId: google.maps.MapTypeId.ROADMAP,
            region: 'zh-CN'
        });

        o.gsauto = new google.maps.places.Autocomplete($('#saddr')[0]);
        o.geauto = new google.maps.places.Autocomplete($('#eaddr')[0]);
        o.ggeocode = new google.maps.Geocoder();
        o.gdirservice = new google.maps.DirectionsService();
        o.gdirrender = new google.maps.DirectionsRenderer();
        o.ginfow = new google.maps.InfoWindow();
        o.gsmarker = new google.maps.Marker({
            map: o.gmap,
            draggable: true,
            title: '设置起始位置'
        });
        o.gemarker = new google.maps.Marker({
            map: o.gmap,
            draggable: true,
            title: '设置终点位置'
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

        o.gsauto.bindTo('bounds', o.gmap);
        o.geauto.bindTo('bounds', o.gmap);
        o.gdirrender.setMap(o.gmap);

        o.bindChange();
        o.del.click(o.delPlan);
        o.next.click(o.getPlan);
        o.save.click(o.savePlan);
    },

    getPlan: function() {
        var o = bmoon.spdpost.init();

        if (mgd._ntt <= 0) return;
        
        var opt = o.plan.id ? {exceptid: o.plan.id} : null;

        $.getJSON('/json/spd/post/do', opt, function(data) {
            if (data.success == 1 && bmoon.utl.type(data.plan) == 'Object') {
                var p = data.plan;
                o.saddr.val(p.saddr);
                o.eaddr.val(p.eaddr);
                o.sdate.val(p.sdate);
                o.stime.val(p.stime);
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

        var p = o.plan;

        if (x != 'e') {
            p.sll = [data.geometry.location.lat(), data.geometry.location.lng()];
            p.saddr = data.formatted_address;
        } else {
            p.ell = [data.geometry.location.lat(), data.geometry.location.lng()];
            p.eaddr = data.formatted_address;
        }

        bmoon.dida.getCityByGeoresult(data, function(city) {
            if (bmoon.utl.type(city) == 'Array') {
                if (x != 'e') p.scityid = city[0].id;
                else p.ecityid = city[0].id;
            }
        });

        if (p.sll && p.ell) {
            p.km = bmoon.utl.earthDis(p.sll, p.ell);
        }
    },

    delPlan: function() {
        var o = bmoon.spdpost.init();

        if (!o.plan) return;

        var pdata = {
            _op: 'del',
            plan: JSON.stringify(o.plan),
            _type_plan: 'object'
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

        var phone = o.phone.val(),
        contact = o.contact.val(),
        p = $(this).parent();
        
        if (!o.plan.sll || !o.plan.ell) return;

        o.plan.sdate = o.sdate.val();
        o.plan.stime = o.stime.val();
        o.plan.rect = '((' + o.plan.sll.join(',') + '),(' +
            o.plan.ell.join(',') + '))';
        o.plan.sgeo = '(' + o.plan.sll.join(',') +')';
        o.plan.egeo = '(' + o.plan.ell.join(',') +')';
        if (phone.length || contact.length) {
            if (phone) o.plan.phone = phone;
            if (contact) o.plan.contact = contact;
        }

        var pdata = {
            _op: 'mod',
            plan: JSON.stringify(o.plan),
            _type_plan: 'object'
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

        var opts = {
            origin: new google.maps.LatLng(o.plan.sll[0], o.plan.sll[1]),
            destination: new google.maps.LatLng(o.plan.ell[0], o.plan.ell[1]),
            travelMode: google.maps.TravelMode.DRIVING
        };
        
        o.gdirservice.route(opts, function(result, status) {
            if (status == google.maps.DirectionsStatus.OK) {
                o.gdirrender.setDirections(result);
            }
        });
    },

    bindChange: function() {
        var o = bmoon.spdpost.init();

        google.maps.event.addListener(o.gsmarker, 'dragend', function() {
            o.markDraged('s');
        });
        google.maps.event.addListener(o.gemarker, 'dragend', function() {
            o.markDraged('e');
        });

        google.maps.event.addListener(o.gsauto, 'place_changed', function() {
            o.autoChanged('s');
        });
        google.maps.event.addListener(o.geauto, 'place_changed', function() {
            o.autoChanged('e');
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
        o.ggeocode.geocode({latLng: pos}, function(results, status) {
            if (status == google.maps.GeocoderStatus.OK) {
                addr.val(results[0].formatted_address);
                o.ginfow.setContent(o._formAddr(
                    results[0].address_components[0].short_name,
                    results[0].address_components,
                    results[0].geometry.location));
                o.ginfow.open(o.gmap, marker);

                o.upPlan(x, results[0]);
                o.rendDirect();
            }
        });
    },

    autoChanged: function(x) {
        var o = bmoon.spdpost.init();
        
        var marker = o.gsmarker,
        auto = o.gsauto;
        
        if (x == 'e') {
            marker = o.gemarker;
            auto = o.geauto;
        }

        o.ginfow.close();
        var place = auto.getPlace();
        if (place.geometry.viewport) {
            o.gmap.fitBounds(place.geometry.viewport);
        } else {
            o.gmap.setCenter(place.geometry.location);
        }
        
        marker.setPosition(place.geometry.location);
        
        o.ginfow.setContent(o._formAddr(
            place.name,
            place.address_components,
            place.geometry.location));
        o.ginfow.open(o.gmap, marker);

        o.upPlan(x, place);
        o.rendDirect();
    }
};

$(document).ready(bmoon.spdpost.onready);
