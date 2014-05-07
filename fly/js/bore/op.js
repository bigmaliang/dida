; var bmoon = bmoon || {};
bmoon.boreop = {
    version: '1.0',
    
    init: function() {
        var o = bmoon.boreop;
        if (o.inited) return o;
        o.inited = true;

        o.imagenum = 0;
        
        o.e_title = $('#title');
        o.e_hint = $('#hint');
        o.e_content = $('#content');
        o.e_submit = $('#submit');
        o.e_submit_del = $('#submit-del');
        o.e_submit_rev = $('#submit-rev');
        
        return o;
    },
    
    onready: function() {
        var o = bmoon.boreop.init();

        o.e_content.markItUp(myMarkdownSettings);
        o.e_content.uploader({
            fileField: '#mkd-input-file',
            url: '/json/zero/image',
            dataName: 'upfile',
            extraData: {_op: 'add'},
            allowedExtension: 'jpeg, bmp, png, gif',
	        afterUpload: function (data) {
                data = jQuery.parseJSON(data);
                if (data.success == 1) {
                    o.imagenum++;
                    var s = '\n' +
                        '[![图片' + o.imagenum+ '](' + data.imageurl_zoom + ' "' + data.imagename+ '")]'+
                        '(' + data.imageurl + ' "' + '点击查看原图")' +
                        '\n';
                    $.markItUp({replaceWith: s});
                } else alert(data.errmsg || '上传图片失败');
	        },
            error: function(msg) {
                alert(msg);
            }
        });

        o.bindClick();
    },

    bindClick: function() {
        var o = bmoon.boreop.init();

        o.e_submit.click(o.boreOp);
        o.e_submit_del.click(function() {o.boreDel(1);});
        o.e_submit_rev.click(function() {o.boreDel(0);});
    },

    boreOp: function() {
        var o = bmoon.boreop.init();
        
        if (!$('.VAL_NEWBORE').inputval()) return;

        var title = o.e_title.val(),
        hint = o.e_hint.val(),
        content = o.e_content.val(),
        p = $(this).parent();

        $(".error", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');
        
        $.post('/json/bore/op', {
            _op:mgd.op? mgd.op: 'add',
            id: mgd.id,
            title: title,
            hint: hint,
            content: content
        }, function(data) {
            p.removeClass('loading');
            if (data.success == 1) {
                p.addClass('success');
                o.e_title.val("");
                o.e_hint.val("");
                o.e_content.val("");
            } else {
                p.addClass('error');
                $('<span class="text-error">'+ data.errmsg +'</span>').appendTo(p);
            }
        }, 'json');
    },

    boreDel: function(statu) {
        var o = bmoon.boreop.init();

        // p = $(this).parent();
        // just work on $('...').click(o.boreDel)
        // don't work on $('...').click(function() {o.boreDel(x)})
        // why ?
        var p = $('#submit').parent();

        if (confirm('确认操作？')) {
            $(".error", p).remove();
            p.removeClass('success').removeClass('error').addClass('loading');
            
            $.getJSON('/json/bore/op',
                      {_op: 'mod', id: mgd.id, statu: statu},
                      function(data) {
                          p.removeClass('loading');
                          if (data.success == 1) {
                              p.addClass('success');
                          } else {
                              p.addClass('error');
                              $('<span class="text-error">'+ data.errmsg +'</span>').appendTo(p);
                          }
                      });
        }
    }
};

$(document).ready(bmoon.boreop.onready);
