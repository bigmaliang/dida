; var bmoon = bmoon || {};
bmoon.planinfo = {
    version: '1.0',

    init: function() {
        var o = bmoon.planinfo;
        if (o.inited) return o;

        o.e_plan_from = $('#plan-from');
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.planinfo.init();

        o.bindClick();
        //bmoon.utl.after(o.rendFromPic, 'mgd._done != undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.planinfo.init();

    },

    rendFromPic: function() {
        var o = bmoon.planinfo.init();

        var domains = bmoon.dida.odomain;
        if (mgd.ori > 0) {
            if (mgd.ourl) o.e_plan_from.attr('href', mgd.ourl);
            else o.e_plan_from.attr('href', domains[mgd.ori].url);
            o.e_plan_from.attr('title', '数据来源：　' + domains[mgd.ori].title);
            $('<img/>').attr('src', domains[mgd.ori].img).appendTo(o.e_plan_from);
        }
    }
};

$(document).ready(bmoon.planinfo.onready);
