; var bmoon = bmoon || {};
bmoon.inboxsystem = {
    version: '1.0',

    init: function() {
        var o = bmoon.inboxsystem;
        if (o.inited) return o;

        o.e_delinbox = $('#inbox .del a');
        o.e_nav = $('#inbox-nav');
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.inboxsystem.init();

        o.bindClick();

        bmoon.utl.after(o.rendNav, 'mgd.ntt != undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.inboxsystem.init();

        o.e_delinbox.click(o.delInbox);
    },

    delInbox: function() {
        var o = bmoon.inboxsystem.init();

        var id = $(this).attr('rel'),
        pp = $(this).parent().parent();

        $.post('/json/inbox', {_op: 'del', id: id, type: 0}, function(data) {
            if (data.success == '1') {
                pp.remove();
            } else {
                alert(data.errmsg || '操作失败');
            }
        }, 'json');
    },

    rendNav: function() {
        var o = bmoon.inboxsystem.init();

        if (mgd.ntt > mgd.npp) {
            o.e_nav.mnnav({
                ntt: mgd.ntt,
                npg: mgd.npg,
                npp: mgd.npp,
                url: '/inbox/system'
            });
        }
    }
};

$(document).ready(bmoon.inboxsystem.onready);
