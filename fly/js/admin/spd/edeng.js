; var bmoon = bmoon || {};
bmoon.spdedeng = {
    version: '1.0',

    init: function() {
        var o = bmoon.spdedeng;
        if (o.inited) return o;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.spdedeng.init();

        var href = location.href;

        if (href.match(/.*\/([0-9|_]+)\.html$/)) {
            o.parseNode(href.match(/.*\/([0-9|_]+)\.html$/)[1]);
        } else if (href.match(/.*edeng.cn\/13\/pinche\//)){
            o.parseList();
        }

        setTimeout(function() {window.location.reload();}, 10*60*1000);
    },

    bindClick: function() {
        var o = bmoon.spdedeng.init();
    },

    parseNode: function(id) {
        var o = bmoon.spdedeng.init();

        var ori = 'edeng', city = '', phone = '', contact = '', size = 0, model = '',
        dad = 0, repeat = 0, saddr = '', eaddr = '', sdate = '', stime = '',
        attach = '', nick = id, seat = 4, fee = 0, marks = '';

        var s = $('#center_left').html(), x;

        // PART 1
        // city, phone, contact
        // size, model
        x = s.match(/起点：.*>(.*)</);
        city = x && x[1] || '';
        phone = $('.contactphonefc a').text() || '';

        x = $('.properties2 .email').text();
        contact = x && x.match(/([a-zA-Z0-9_\.\-\+])+\@(([a-zA-Z0-9\-])+\.)+([a-zA-Z0-9]{2,4})+/)[0] || '';

        x = $('.properties3').html().match(/车型：.*>(.*)</);
        model = x && x[1] || '';


        // PART 2
        // dad
        // repeat
        // saddr, eaddr
        // sdate, stime
        dad = s.match(/我是车主/) ? 1: 0;

        x = $('.properties3').html().match(/出发日：.*>(.*)</);
        x = x && x[1] || '';
        var xx = [
            ['其他','0'],
            ['每天', '0'],
            ['工作日', '1,2,3,4,5'],
            ['周末节假日', '6,7']
        ];
        if (x) {
            for (var i = 0; i < xx.length; i++) {
                if (xx[i][0].match(x)) {
                    repeat = i < 2 ? i: 2;
                    sdate = xx[i][1];
                }
            }
        }
        
        x = s.match(/起点：.*>(.*)</);
        saddr = x && x[1] || '';

        x = s.match(/终点：.*>(.*)</);
        eaddr = x && x[1] || '';
        
        x = $('.properties3').html().match(/出发时间：.*>(.*)</);
        x = x && x[1] || '2011-12-31 08:00:00';
        if (repeat == 0) sdate = x.split(' ')[0];
        stime = x.split(' ')[1] ? x.split(' ')[1]: '08:00:00';
        

        // PART 3
        // attach, nick
        // seat, fee, marks
        attach = $('#center_left .edcontent p').html().replace(/\<[^\>]+\>/g, "");
        nick = $('#center_left .username a').html();

        x = $('.properties3').html().match(/座位数：.*>(.*)</);
        seat = x && parseInt(x[1]) || '';

        x = $('.properties3').html().match(/价格：.*>(.*)</);
        fee = x && parseInt(x[1]) || '';

        x = s.match(/途经地点：.*>(.*)</);
        marks = x && x[1] || '';


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
        var o = bmoon.spdedeng.init();

        var
        ids = [], urls = {},
        objs = $('a', '.house_infor1'),
        reg = /.*\/jiedaoxinxi\/(.*)\.html$/;

        $.each(objs, function(i, obj) {
            if ($(obj).attr('href').match(reg)) {
                var id = $(obj).attr('href').match(reg)[1];
                ids.push(id);
                urls[id] = $(obj).attr('href');
            }
        });

        $.getJSON(g_site_admin + 'json/spd/pre?JsonCallback=?',
                 {
                     ori: 'edeng',
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

$(document).ready(bmoon.spdedeng.onready);
