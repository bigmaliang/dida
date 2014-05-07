; var bmoon = bmoon || {};
bmoon.bore = {
    version: '1.0',

    // { "id": "1", "type": "0", "oid": "7", "pid": "0", "ip": "127.0.0.1", "addr": "CZ88.NET", "author": "ru_PGFOV", "content": "hey, man\n\n  Baby\n", "intime": "2010-10-22 16:53:42", "uptime": "2010-10-22 16:53:42" }
    _nodeComment: function(cmt) {
        var r = $('#comment-'+cmt.id),
        html = [
            '<div class="comment-item" id="comment-', cmt.id, '">',
                '<div class="info">',
                    '<span class="addr">',
                        '来自 ', cmt.addr, ' 的 ',
                    '</span> ',
                    '<span class="author">', cmt.author, '</span> ',
                    '<span class="date">', cmt.intime, '</span> ',
                '</div>',
                '<div class="content">',
                    '<p class="pre">', bmoon.utl.stripHTML(cmt.content), '</p>',
                '</div>',
            '</div>'
        ].join('');

        if (cmt.id == 0) r = [];
        if (!r.length) {
            r = $(html);
        }
        return r;
    },
    
    init: function() {
        var o = bmoon.bore;
        if (o.inited) return o;

        o.commentnum = 0;
        o.commentget = 0;

        o.author = bmoon.dida.c_mnick;
        if (!o.author) o.author = '网友';

        o.e_comment = $('#comment');
        o.e_comment_get = $('#comment-get');
        o.e_comment_add = $('#comment-add');
        o.e_comment_content = $('#comment-content');
        o.e_get_comment = $('#get-comment');
        o.e_add_comment = $('#add-comment');
        
        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.bore.init();

        o.bindClick();
        bmoon.utl.after(o.getComment, 'mgd.id !== undefined', 10);
    },

    bindClick: function() {
        var o = bmoon.bore.init();

        o.e_get_comment.click(o.getComment);
        o.e_add_comment.click(o.addComment);
    },

    getComment: function() {
        var o = bmoon.bore.init();

        o.e_comment_num = $('#comment-num-'+mgd.id);
        
        $.getJSON('/json/comment', {
            ids: '0:'+mgd.id,
            _nst: o.commentget
        }, function(data) {
            if (data.success == 1 && bmoon.utl.type(data[0]) == 'Object') {
                o.commentnum = data[0][mgd.id]._ntt;
                o.e_comment_num.text(o.commentnum);
                
                if (bmoon.utl.type(data[0][mgd.id].cmts) == 'Object') {
                    $.each(data[0][mgd.id].cmts, function(k, v){
                        if (v.pid == 0)
                            o._nodeComment(v).appendTo(o.e_comment);
                        else
                            o._nodeComment(v).appendTo($('#comment-'+v.pid));
                        o.commentget++;
                    });
                    if (o.commentget < o.commentnum) {
                        o.e_comment_get.removeClass('hide');
                    } else {
                        o.e_comment_get.addClass('hide');
                    }
                }
            }
        });
    },

    addComment: function() {
        var o = bmoon.bore.init();

        if (!$('.VAL_NEWCOMMENT').inputval()) return;

        var content = o.e_comment_content.val(),
        p = $(this).parent();

        $(".error", p).remove();
        p.removeClass('success').removeClass('error').addClass('loading');

        $.post('/json/comment',
               {
                   _op: 'add', type: 0, oid: mgd.id, otitle: mgd.title, pid: 0,
                   author: o.author,
                   content: content
               },
               function(data) {
                   p.removeClass('loading');
                   
                   if (data.success == 1) {
                       p.addClass('success');
                       o.commentnum++;
                       o.e_comment_num.text(o.commentnum);
                       o.e_comment_content.val('');
                       o._nodeComment({
                           id: 0,
                           addr: '火星',
                           author: '你',
                           intime: '刚刚',
                           content: content
                       }).prependTo(o.e_comment).fadeIn('slow');
                   } else {
                       p.addClass('error');
                       $('<span class="text-error">'+ data.errmsg +'</span>').appendTo(p);
                   }
               },
               'json');
    }
};

$(document).ready(bmoon.bore.onready);
