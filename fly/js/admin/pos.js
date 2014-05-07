; var bmoon = bmoon || {};
bmoon.pos = {
    version: '1.0',

    init: function() {
        var o = bmoon.pos;
        if (o.inited) return o;

        o.res = $('#res');

        o.ggeocode = new google.maps.Geocoder();

        o.curr_city = 0;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.pos.init();

        o.bindClick();

        bmoon.utl.after(o.getPos, 'mgd._done != undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.pos.init();

    },

    getPos: function() {
        var o = bmoon.pos.init();

        var i = o.curr_city;
        if (i == mgd.cities.length-1) return;
        //for (var i = 0; i < mgd.cities.length; i++) {
            var c = mgd.cities[i].s, id = mgd.cities[i].id;
            o.ggeocode.geocode({address: c}, function(results, status) {
                if (status == google.maps.GeocoderStatus.OK) {
                    $.post('/json/pos', {
                        _op: 'mod',
                        s: c,
                        id: id,
                        latlng: '(' + results[0].geometry.location.lat() +
                            ', ' + results[0].geometry.location.lng() + ')'
                    }, function(data) {;});
                } else {
                    console.log(c + 'error' + status);
                }
            });
        //}
        o.curr_city++;
        setTimeout(o.getPos, 2000);
    }
};

$(document).ready(bmoon.pos.onready);
