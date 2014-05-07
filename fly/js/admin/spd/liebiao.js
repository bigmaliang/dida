; var bmoon = bmoon || {};
bmoon.spdliebiao = {
    version: '1.0',

    init: function() {
        var o = bmoon.spdliebiao;
        if (o.inited) return o;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.spdliebiao.init();

        var href = location.href;

        if (href.match(/.*\/[0-9]+\.html$/)) {
            o.parseNode(href.match(/.*\/([0-9]+)\.html$/)[1]);
        } else if (href.match(/.*liebiao.com\/pinche\//)){
            o.parseList();
        }

        setTimeout(function() {window.location.reload();}, 10*60*1000);
    },

    bindClick: function() {
        var o = bmoon.spdliebiao.init();
    },

    parseNode: function(id) {
        var o = bmoon.spdliebiao.init();

        var ori = 'liebiao', city = '', phone = '', contact = '', size = 0, model = '',
        dad = 0, repeat = 0, saddr = '', eaddr = '', sdate = '', stime = '',
        attach = '', nick = id, seat = 4, fee = 0, marks = '';

        var s = $('tr', '.lb_pp'), x;

        // PART 1
        // city, phone, contact
        // size, model
        city = $('.top_a').html() || '';

        $.each(s, function(i, ele){
            var c = $(':first', ele),
            t = c.html(),
            v = c.next().html();

            if (t.match(/联系电话/)) {
                x = $('img', c.next()).attr('src');
                phone = bmoon.utl.clotheHTML(x);
            } else if (t.match(/是否有车/)) {
                dad = v.match(/有车/) ? 1: 0;
            } else if (t.match(/出发地/)) {
                saddr = $.trim(v);
            } else if (t.match(/到达地/)) {
                eaddr = $.trim(v);
            } else if (t.match(/途径地/)) {
                marks = $.trim(v);
            }
        });
        

        // PART 2
        // dad
        // repeat
        // saddr, eaddr
        // sdate, stime
        sdate = $('.postdate').html() || '';
        

        // PART 3
        // attach, nick
        // seat, fee, marks
        attach = $('.messagebody') &&
            $.trim($('.messagebody').html().replace('<b>信息详细内容</b><br>', ''));
        if (attach.match(/每天/)) repeat = 1;
        nick = '列表网友';
        

        console.log('city ' + city);
        console.log('phone ' + phone);
        console.log('contact ' + contact);
        console.log('size ' + size);
        console.log('model ' + model);

        console.log('dad ' + dad);
        console.log('repeat ' + repeat);
        console.log('saddr ' + saddr);
        console.log('eaddr ' + eaddr);
        console.log('sdate ' + sdate);
        console.log('stime ' + stime);

        console.log('attach ' + attach);
        console.log('nick ' + nick);
        console.log('seat ' + seat);
        console.log('fee ' + fee);
        console.log('marks ' + marks);

        $.getJSON(g_site_admin + 'json/spd/do?JsonCallback=?', {
            _op: 'add',

            plan: JSON.stringify({
                mid: 0,
                phone: bmoon.utl.clotheHTML(phone),
                contact: bmoon.utl.clotheHTML(contact),
                ori: ori,
                oid: id,
                ourl: location.href,
                dad: dad,
                nick: nick,
                fee: fee,
                saddr: saddr,
                eaddr: eaddr,
                marks: marks, // convert
                city: city, // need convert to cityid
                repeat: repeat,
                sdate: sdate,
                stime: stime,
                attach: attach
            }),
            
            _type_object: 'plan',
            
        }, function(mydata) {
            if (mydata.success == 1) {
                window.opener = null;
                window.close();
            } else {
                console.log(mydata.errmsg);
                console.log(mydata.errtrace);
            }
        });
    },
    
    parseList: function() {
        var o = bmoon.spdliebiao.init();

        var
        ids = [], urls = {},
        objs = $('a', '.article_list'),
        reg = /liebiao.com\/pinche\/([0-9]+)\.html$/;

        $.each(objs, function(i, obj) {
            if ($(obj).attr('href').match(reg)) {
                var id = $(obj).attr('href').match(reg)[1];
                ids.push(id);
                urls[id] = $(obj).attr('href');
            }
        });

        $.getJSON(g_site_admin + 'json/spd/pre?JsonCallback=?',
                 {
                     ori: 'liebiao',
                     oids: ids
                 }, function(data) {
                     if (data.success == 1) {
                         if (bmoon.utl.type(data.oids) == 'Object') {
                             $.each(data.oids, function(key, val) {
                                 console.log(val);
                                 setTimeout(function() {window.open(urls[val]);},
                                            Math.random()*50*1000);
                             });
                         } else {
                             console.log('dida ALL DONE');
                         }
                     } else {
                         alert(data.errmsg || '操作失败');
                     }
                 })
    }
};

$(document).ready(bmoon.spdliebiao.onready);
