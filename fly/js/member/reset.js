; var bmoon = bmoon || {};
bmoon.memberreset = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.memberreset;
        if (o.inited) return o;

        o.mname = $('#mname');
        o.submit = $('#submit');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.memberreset.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.memberreset.init();

        //o.mname.blur(o.memberCheck);
        o.submit.click(o.memberReset);
    },

    memberCheck: function() {
        var o = bmoon.memberreset.init();
        
        var
        mname = o.mname.val(),
        p = $(this).parent();
        
        if (mname.length > 0) {
            $(".vres", p).remove();
            p.removeClass('success').removeClass('error').addClass('loading');
            $.getJSON('/json/member/exist', {mname: mname}, function(data) {
                p.removeClass('loading');
                if (data.success == 1) {
                    if (data.exist == 0) {
                        $('<span class="vres">'+ data.msg +'</span>').appendTo(p);
                        p.addClass('error');
                    } else {
                        p.addClass('success');
                    }
                } else {
                    p.addClass('error');
                    $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
                }
            });
        }
    },

    memberReset: function() {
        var o = bmoon.memberreset.init();

        if (!$('.VAL_RESETMEMBER').inputval()) return;

        var mname = o.mname.val(),
        p = $(this).parent();

        $(".vres", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.getJSON('/json/member/reset', {mname: mname}, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                $('<span class="vres">邮件一分钟送达，请依邮件内容完成密码重置。</span>').appendTo(p);
            } else {
                p.addClass('error');
                $('<span class="vres">'+ data.errmsg +'</span>').appendTo(p);
            }
        });
    }
};

$(document).ready(bmoon.memberreset.onready);
