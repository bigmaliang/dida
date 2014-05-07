; var bmoon = bmoon || {};
bmoon.memberregist = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.memberregist;
        if (o.inited) return o;

        o.mnick = $('#mnick');
        o.mname = $('#mname');
        o.msn = $('#msn');
        o.submit = $('#submit');

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.memberregist.init();

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.memberregist.init();

        o.mname.blur(o.memberCheck);
        o.submit.click(o.memberAdd);
    },

    memberCheck: function() {
        var o = bmoon.memberregist.init();
        
        var mname = o.mname.val(),
        p = $(this).parent();
        
        if (mname.length > 0) {
            $('.vres', p).remove();
            p.removeClass('success').removeClass('error').addClass('loading');
            $.getJSON('/json/member/exist', {mname: mname}, function(data) {
                p.removeClass('loading');
                if (data.success == 1) {
                    if (data.exist == 1) {
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

    memberAdd: function() {
        var o = bmoon.memberregist.init();

        if (!$('.VAL_NEWAPP').inputval()) return;

        o.msn.attr('value', $.md5($.md5(o.msn.val())));

        var mnick = o.mnick.val(),
        mname = o.mname.val(),
        msn = o.msn.val(),
        p = $(this).parent();

        $('.vres', p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        $.post('/json/member/new', {_op: 'add', mnick: mnick, mname: mname, msn: msn}, function(data) {
            p.removeClass('loading');
            if (data.success != 1 || !data.mnick) {
                p.addClass('error');
                noty({text: data.errmsg, type: 'error', theme: 'noty_theme_mitgux'});
            } else {
                p.addClass('success');
                noty({text: '注册成功，且已完成登录。', type: 'success', theme: 'noty_theme_mitgux'});
                setTimeout(function() {
                    //$('#done').removeClass('hide');
                    bmoon.dida.loginCheck();
                }, 1000);
            }
        }, 'json');
    }
};

$(document).ready(bmoon.memberregist.onready);
