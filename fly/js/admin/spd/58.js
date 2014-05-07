; var bmoon = bmoon || {};
bmoon.spd58 = {
    version: '1.0',

    init: function() {
        var o = bmoon.spd58;
        if (o.inited) return o;

        o.inited = true;
        return o;
    },
    
    onready: function() {
        var o = bmoon.spd58.init();

        var href = location.href;

        if (href.match(/www.58.com\/pinche\/changecity\//)) {
            o.parseDir();
        } else if (href.match(/.*\/([0-9]+)x\.shtml$/)) {
            o.parseNode(href.match(/.*\/([0-9]+)x\.shtml$/)[1]);
        } else if (href.match(/.*58.com\/pinche\//)) {
            o.parseList();
        }
    },

    bindClick: function() {
        var o = bmoon.spd58.init();
    },

    parseNode: function(id) {
        var o = bmoon.spd58.init();

        var
        sizes = ['小轿车', '商务车', '越野车', '面包车', '客车', '货车'],
        dad = 0, saddr = '', eaddr = '', marks = '', time = '', size = 0,
        repeat = 0, sdate = '', stime = '', attach = '';

        var x = $('.info').html().match(/[^>]+→[^<]+/);
        x = x && x[0].split('→');
        if (x) {
            saddr = $.trim(x[0]);
            eaddr = $.trim(x[1]);
        }

        dad = $('.headline').html().match('有车') ? 1: 0;

        x = $('.info').html().match(/途经：<\/i>([^\<]+)/);
        marks = x && $.trim(x[1]) || '';
        
        x = $('.info').html().match(/时间：<\/i>([^\<]+)/);
        time = x && $.trim(x[1]).replace("&nbsp;", " ") || '2011-12-31 08:00:00';

        x = $('.info').html().match(/类型：<\/i>([^\<]+)/);
        x = x && x[1] && x[1].split('\n');
        repeat = x && $.trim(x[0]) == '上下班拼车' ? 2: 0;
        x = x && $.trim(x[1]);
        if (x) {
            for (var i = 0; i < sizes.length; i++) {
                if (x.match(sizes[i])) {
                    size = i;
                    break;
                }
            }
        }

        sdate = repeat == 2 ? '1,2,3,4,5': time.split(' ')[0];
        stime = time.split(' ')[1] ? time.split(' ')[1]: time.split(' ')[0];

        attach = $('.maincon').html().replace(/\<[^\>]+\>/g, "");

        var
        uid = $('.user').html().match('uid: \'([0-9]+)\''),
        uname = $('.user').html().match('username:\'(.*)\''),
        phone = $('#t_phone').html(),
        contact = $('.user > .userinfo').html(),
        city = $('.user > .vuser').html().match('（归属地：(.*)）');
        
        uid = uid && uid[1] || '';
        uname = uname && uname[1] || '';
        if (!phone.match(/^[0-9]+$/)) {
            phone = phone.match('http://image.58.com/showphone[^\'"]+');
            phone = phone && phone[0] || '';
        }
        contact = contact.match('http://image.58.com/showphone[^\'"]+');
        contact = contact && contact[0] || '';
        city = city && city[1] || $('#topbar .bar_left h2').html() || '';

        var ori = '58';
        
        $.getJSON(g_site_admin + 'json/spd/do?JsonCallback=?',
                  {
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
        
        console.log('dad ' + dad);
        console.log('saddr ' + saddr);
        console.log('eaddr ' + eaddr);
        console.log('marks ' + marks);
        console.log('size ' + sizes[size]);
        console.log('repeat ' + repeat);
        console.log('sdate ' + sdate);
        console.log('stime ' + stime);
        console.log('attach ' + attach);

        console.log('uid' + uid);
        console.log('uname' + uname);
        console.log('phone' + phone);
        console.log('contact' + contact);
        console.log('city' + city);
    },
    
    parseList: function() {
        var o = bmoon.spd58.init();

        var ids = [], urls = {},
        objs = $('a.t', '#infolist'),
        reg = /.*\/pinche\/([0-9]+)x\.shtml$/;

        $.each(objs, function(i, obj) {
            if ($(obj).attr('href').match(reg)) {
                ids.push($(obj).attr('href').match(reg)[1]);
                urls[$(obj).attr('href').match(reg)[1]] = $(obj).attr('href');
            }
        });

        $.getJSON(g_site_admin + 'json/spd/pre?JsonCallback=?', {
            ori: '58',
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
                console.log(data.errmsg || '操作失败');
            }
        });
        
        setTimeout(function() {
            var pn = $.cookie('_dida_pn');

            if (!pn) pn = '1';
            pn = parseInt(pn) + 1;

            if (pn < 4) {
                $.cookie('_dida_pn', pn, {path: '/'});
                window.location = location.href.match(/.*58.com\/pinche\//)[0] + 'pn' + pn;
            } else {
                //$.cookie('_dida_pn', 1, {path: '/'});
                //window.location = href.match(/.*58.com\/pinche\//)[0];
                window.opener = null;
                window.open('', '_self', '');
                window.close();
            }
        }, 60*1000);
    },

    parseDir: function() {
        var o = bmoon.spd58.init();

        var cs = $('#clist a'),
        pos = 0,
        dcities = {
            "ankang":"安康", "am":"澳门", "aks":"阿克苏", "ale":"阿拉尔", "al":"阿里",
            "ab":"阿坝", "alsm":"阿拉善盟", "anshun":"安顺", "baoji":"宝鸡",
            "bc":"白城", "bozhou":"亳州", "bygl":"巴音郭楞", "changdu":"昌都",
            "chongzuo":"崇左", "diqing":"迪庆", "dx":"定西", "dxal":"大兴安岭",
            "dafeng":"大丰", "fcg":"防城港", "guyuan":"固原", "gt":"馆陶",
            "ganzi":"甘孜", "guoluo":"果洛", "gn":"甘南", "hlr":" 海拉尔",
            "ht":"和田", "huangnan":"黄南", "hx":"海西", "hainan":"海南",
            "haidong":"海东", "haibei":"海北", "heihe":"黑河", "hexian":"和县",
            "hq":"霍邱", "jinchang":"金昌", "jyg":"嘉峪关", "jq":"酒泉",
            "kzls":"克孜勒苏", "kl":"垦利", "linxia":"临夏", "linyixian":"临猗",
            "lincang":"临沧", "linzhi":"林芝", "ln":"陇南", "lb":"来宾",
            "lps":"六盘水", "mg":"明港", "nujiang":"怒江", "nq":"那曲", "pe":"普洱",
            "qinzhou":"钦州", "qdn":"黔东南", "qn":"黔南", "qxn":"黔西南",
            "qingxu":"清徐", "rkz":"日喀则", "sw":"汕尾", "sn":"山南", "sl":"商洛",
            "szs":"石嘴山", "snj":"神农架", "tw":"台湾", "tmsk":"图木舒克", "tc":"铜川",
            "tr":"铜仁", "taishan":"台山", "tongcheng":"桐城", "wfd":"瓦房店",
            "wjq":"五家渠", "wzs":"五指山", "wuyishan":"武夷山", "hk":"香港",
            "yuzhou":"禹州", "yanling":"鄢陵", "ys":"玉树", "yf":"云浮", "yxx":"永新",
            "zd":"正定", "zhangqiu":"章丘", "zx":"赵县", "zw":"中卫",
            "zhangye":"张掖", "zhangbei":"张北", "pld":"庄河"
        },
        zcities = {
            "bs":"保山", "betl":"博尔塔拉", "baishan":"白山", "by":"白银",
            "baise":"百色", "bh":"北海", "bijie":"毕节", "chenzhou":"郴州",
            "chifeng":"赤峰", "cx":"楚雄", "changji":"昌吉", "changge":"长葛",
            "cy":"朝阳", "ch":"巢湖", "chizhou":"池州", "chuzhou":"滁州",
            "chaozhou":"潮州", "dali":"大理", "dingzhou":"定州", "dh":"德宏",
            "ez":"鄂州", "fushun":"抚顺", "fy":"阜阳", "gg":"贵港", "ganzhou":"赣州",
            "ga":"广安", "hh":"怀化", "hc":"河池", "hami":"哈密", "honghe":"红河",
            "hegang":"鹤岗", "hn":"淮南", "heyuan":"河源", "hezhou":"贺州", "jy":"揭阳",
            "ja":"吉安", "jz":"晋中", "jinzhou":"锦州", "jdz":"景德镇", "jiyuan":"济源",
            "klmy":"克拉玛依", "kel":"库尔勒", "lvliang":"吕梁", "liaoyang":"辽阳",
            "liaoyuan":"辽源", "luohe":"漯河", "la":"六安", "lw":"莱芜", "mdj":"牡丹江",
            "mz":"梅州", "scnj":"内江", "np":"南平", "pl":"平凉", "panzhihua":"攀枝花",
            "pt":"莆田", "qqhr":"齐齐哈尔", "quzhou":"衢州", "qingyuan":"清远",
            "qj":"曲靖", "qingyang":"庆阳", "qth":"七台河", "qianjiang":"潜江",
            "st":"汕头", "sm":"三明", "sg":"韶关", "sq":"商丘", "shuyang":"沭阳",
            "suihua":"绥化", "sanya":"三亚", "sd":"顺德", "songyuan":"松原",
            "smx":"三门峡", "suizhou":"随州", "shuozhou":"朔州", "sr":"上饶",
            "sp":"四平", "sys":"双鸭山", "tlf":"吐鲁番", "tongliao":"通辽", "tm":"天门",
            "wuzhou":"梧州", "ws":"文山", "wuhai":"乌海", "wuzhong":"吴忠",
            "wlcb":"乌兰察布", "wuwei":"武威", "xiangtan":"湘潭", "xf":"襄阳",
            "xx":"新乡", "xt":"邢台", "xinzhou":"忻州", "xl":"锡林郭勒盟",
            "xianyang":"咸阳", "xy":"信阳", "xam":"兴安盟", "xinyu":"新余",
            "xiangxi":"湘西", "xianning":"咸宁", "yingtan":"鹰潭", "yx":"玉溪",
            "yk":"营口", "yl":"榆林", "yj":"阳江", "yangchun":"阳春", "yq":"阳泉",
            "ya":"雅安", "yich":"伊春", "zhuzhou":"株洲", "zq":"肇庆", "zj":"镇江",
            "zmd":"驻马店", "zjj":"张家界", "zy":"资阳", "zunyi":"遵义",
            "zhoushan":"舟山", "zc":"诸城", "zt":"昭通"
        };

        o.dcities = dcities;
        o.zcities = zcities;

        cs = ['bj', 'cd', 'cs', 'gz', 'hz', 'jn', 'sh', 'sz', 'su', 'tj', 'wh',
             'dg', 'dl', 'lz', 'ty', 'zz', 'xa', 'nj', 'nc', 'qd', 'km'];
        
        function get() {
            /*
            for (var i = 0; i < 5 && pos < cs.length; pos++) {
                var href = $(cs[pos]).attr('href'),
                c = href.match(/http:\/\/(.*).58.com/)[1];

                if (dcities[c] || zcities[c]) {
                    console.log(c + '暂不获取');
                } else {
                    window.open(href);
                    i++;
                }
            }
            */
            for (var i = 0; i < 1 && pos < cs.length; pos++) {
                var href = 'http://' + cs[pos] + '.58.com/pinche/';
                window.open(href);
                i++;
            }
            setTimeout(get, 5*60*1000);
        }

        get();
    }
};

$(document).ready(bmoon.spd58.onready);
