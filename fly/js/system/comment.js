; var bmoon = bmoon || {};
bmoon.systemcomment = {
    version: '1.0',

    init: function() {
        var o = bmoon.systemcomment;
        if (o.inited) return o;

        o.e_dateinput = $('.datep');
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.systemcomment.init();

        $.tools.dateinput.localize("zh",  bmoon.dida.dateinputzh);
        o.calendar = o.e_dateinput.dateinput({
            format: 'yyyy-mm-dd',
            lang: 'zh',
            trigger: true,
            firstDay: 1
        });

        //bmoon.utl.after(o.rendPageview, 'mgd._done != undefined', 10);

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.systemcomment.init();

    }
};

$(document).ready(bmoon.systemcomment.onready);
