; var bmoon = bmoon || {};
bmoon.systemplan = {
    version: '1.0',

    init: function() {
        var o = bmoon.systemplan;
        if (o.inited) return o;

        o.e_delplan = $('#plan-list a.del');
        o.e_statu = $('select[name="statu"]');
        o.e_repeat = $('select[name="repeat"]');
        o.e_dateinput = $('.datep');
        o.e_nav = $('#plan-nav');
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.systemplan.init();

        o.bindClick();

        bmoon.utl.after(o.rendNav, 'mgd.ntt != undefined', 10);

        $.tools.dateinput.localize("zh",  bmoon.dida.dateinputzh);
        o.calendar = o.e_dateinput.dateinput({
            format: 'yyyy-mm-dd',
            lang: 'zh',
            trigger: true,
            firstDay: 1
        });

        var s = bmoon.utl.getQueryString('statu');
        if (s.length) o.e_statu.val(s);
        s = bmoon.utl.getQueryString('repeat');
        if (s.length) o.e_repeat.val(s);
        
    },

    bindClick: function() {
        var o = bmoon.systemplan.init();

        o.e_delplan.click(o.delPlan);
    },

    delPlan: function() {
        var o = bmoon.systemplan.init();

        var id = $(this).attr('rel'),
        pp = $(this).parent().parent(),
        pdata = {
            _op: 'mod',
            id: id,
            statu: bmoon.dida.planstatu.del
        };
        
        $.getJSON('/json/plan/mine', pdata, function(data) {
            if (data.success == '1') {
                pp.remove();
            } else {
                alert(data.errmsg || '操作失败');
            }
        });
    },
    
    rendNav: function() {
        var o = bmoon.systemplan.init();

        var param = bmoon.utl.urlparam.replace(/_npg=[0-9]+&?/, '')
            .replace(/_npp=[0-9]+&?/, '');
        if (param == '?') param = '';
        if (param.match(/^.*&$/)) param = param.substr(0, param.length - 1);
        
        if (mgd.ntt > mgd.npp) {
            o.e_nav.mnnav({
                ntt: mgd.ntt,
                npg: mgd.npg,
                npp: mgd.npp,
                url: bmoon.utl.urlclean + param
            });
        }
    }
};

$(document).ready(bmoon.systemplan.onready);
