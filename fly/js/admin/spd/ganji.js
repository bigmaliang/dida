; var bmoon = bmoon || {};
bmoon.spdganji = {
    version: '1.0',

    init: function() {
        var o = bmoon.spdganji;
        if (o.inited) return o;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.spdganji.init();

        var href = location.href;

        o.area = href.match(/http:\/\/(.*).ganji.*/)[1];

        if (href.match(/www.ganji.com\/index.htm/)) {
            o.parseDir();
        } else if (href.match(/.*\/([0-9|_]+x)\.htm$/)) {
            o.parseNode(o.area + href.match(/.*\/([0-9|_]+x)\.htm$/)[1]);
        } else if (href.match(/.*ganji.com\/pincheshangxiaban\//)){
            o.parseList();
        }
    },

    bindClick: function() {
        var o = bmoon.spdganji.init();
    },

    parseNode: function(id) {
        var o = bmoon.spdganji.init();

        var
        sizes = ['小轿车', '商务车', '越野车', '面包车', '客车', '货车'], model = '',
        dad = 0, saddr = '', eaddr = '', marks = '', time = '', size = 0, fee = 0,
        repeat = 0, sdate = '', stime = '', attach = '', uname = '赶集网友';

        saddr = $.trim($('.body_left').text().match('起点：([^<\n]+)')[1]);
        
        eaddr = $.trim($('.body_left').text().match('终点：([^<\n]+)')[1]);

        dad = $('.body_left').html().match('我是车主') ? 1: 0;

        x = $('.body_left').html().match('联系人：([^<]+)');
        if (x) uname = $.trim(x[1]);

        x = $('.body_left').html().match(/途径地点:([^\<]+)/);
        marks = x && $.trim(x[1]) || '';
        
        x = $('.body_left').html().match(/[0-9]{4}-[0-9]{2}-[0-9]{2}/);
        time = x && x[0] || '2011-12-31 08:00:00';

        sdate = repeat == 2 ? '1,2,3,4,5': time.split(' ')[0];
        stime = time.split(' ')[1] ? time.split(' ')[1]: '';

        attach = $($('.mt20', '.body_left')[0]).html().replace(/\<[^\>]+\>/g, "");

        var phone = $('img', '.body_left')[0] && $($('img', '.body_left')[0]).attr('src') || '',
        contact = $('img', '.body_left')[1] && $($('img', '.body_left')[1]).attr('src') || '',
        city = $($('.city a', '#head')[0]).html();

        if (phone) phone = 'http://' + o.area + '.ganji.com' + phone;
        if (contact) contact = 'http://' + o.area + '.ganji.com' + contact;
        
        console.log('dad ' + dad);
        console.log('uname ' + uname);
        console.log('saddr ' + saddr);
        console.log('eaddr ' + eaddr);
        console.log('fee' + fee);
        console.log('marks ' + marks);
        console.log('size ' + sizes[size]);
        console.log('repeat ' + repeat);
        console.log('sdate ' + sdate);
        console.log('stime ' + stime);
        console.log('attach ' + attach);

        console.log('phone' + phone);
        console.log('contact' + contact);
        console.log('city' + city);

        var ori = 'ganji';
        
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
                nick: uname,
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
        var o = bmoon.spdganji.init();

        var ids = [], ids2 = [],
        urls = {},
        cnt = 0,
        objs = $('a', '#con_one_1'),
        reg = /.*\/pincheshangxiaban\/(.*)\.htm$/;

        $.each(objs, function(i, obj) {
            if ($(obj).attr('href').match(reg)) {
                var id = o.area + $(obj).attr('href').match(reg)[1];
                if (cnt++ < 30) {
                    ids.push(id);
                } else {
                    ids2.push(id);
                }
                urls[id] = 'http://' + o.area + '.ganji.com' + $(obj).attr('href');
            }
        });

        function callback(data) {
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
        }

        $.getJSON(g_site_admin + 'json/spd/pre?JsonCallback=?', {
            ori: 'ganji',
            oids: ids
        }, callback);

        $.getJSON(g_site_admin + 'json/spd/pre?JsonCallback=?', {
            ori: 'ganji',
            oids: ids2
        }, callback);
        
        setTimeout(function() {
            var pn = $.cookie('_dida_pn');

            if (!pn) pn = '1';
            pn = parseInt(pn) + 1;

            if (pn < 4) {
                $.cookie('_dida_pn', pn, {path: '/'});
                window.location = location.href.match(/.*ganji.com\/pincheshangxiaban\//)[0] + 'f' + (pn-1)*50;
            } else {
                //$.cookie('_dida_pn', 1, {path: '/'});
                //window.location = href.match(/.*ganji.com\/pincheshangxiaban\//)[0];
                window.opener = null;
                window.open('', '_self', '');
                window.close();
            }
        }, 60*1000);
    },

    parseDir: function() {
        var o = bmoon.spdganji.init();

        var cs = [
            'cs', 'bj', 'sh', 'gz', 'sz', 'wh', 'nj', 'tj', 'hz',
            'cd', 'cq', 'cc', 'dl', 'dg', 'fz', 'foshan', 'gy',
            'gl', 'hrb', 'hf', 'hn', 'jn', 'km', 'lz',
            'xz', 'nj', 'nb', 'nn', 'nc', 'qd', 'sy', 'sjz',
            'su', 'ty', 'wx', 'xj', 'wei', 'xa', 'xm', 'xn',
            'yc', 'yichang', 'yantai', 'zz', 'zhuhai'
        ],
        pos = 0;

        function get() {
            for (var i = 0; i < 1 && pos < cs.length; pos++) {
                var href = 'http://' + cs[pos] + '.ganji.com/pincheshangxiaban/';
                window.open(href);
                i++;
            }

            setTimeout(get, 5*60*1000);
        }

        get();
    }
};

$(document).ready(bmoon.spdganji.onready);
