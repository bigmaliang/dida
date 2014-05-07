# -*- coding: utf-8 -*-
# last id 46134

from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor
from scrapy.selector import HtmlXPathSelector
from scrapy.item import Item
from dida.items import DidaItem
from scrapy.http import Request
import re
import pg

cities = {u'dongying': {'s': u'\u4e1c\u8425', 'id': 3705}, u'yinchuan': {'s': u'\u94f6\u5ddd', 'id': 6401}, u'ningxia': {'s': u'\u5b81\u590f', 'id': 30}, u'sichuan': {'s': u'\u56db\u5ddd', 'id': 23}, u'zhengzhou': {'s': u'\u90d1\u5dde', 'id': 4101}, u'anhui': {'s': u'\u5b89\u5fbd', 'id': 13}, u'linyi': {'s': u'\u4e34\u6c82', 'id': 3713}, u'rizhao': {'s': u'\u65e5\u7167', 'id': 3711}, u'xizang': {'s': u'\u897f\u85cf', 'id': 26}, u'hainan': {'s': u'\u6d77\u5357', 'id': 22}, u'neimenggu': {'s': u'\u5185\u8499\u53e4', 'id': 7}, u'nantongshi': {'s': u'\u5357\u901a\u5e02'}, u'zhejiang': {'s': u'\u6d59\u6c5f', 'id': 12}, u'zibo': {'s': u'\u6dc4\u535a', 'id': 3703}, u'huaian': {'s': u'\u6dee\u5b89', 'id': 3208}, u'heilongjiang': {'s': u'\u9ed1\u9f99\u6c5f', 'id': 10}, u'xinjiang': {'s': u'\u65b0\u7586', 'id': 31}, u'xiamen': {'s': u'\u53a6\u95e8', 'id': 3502}, u'sanya': {'s': u'\u4e09\u4e9a', 'id': 4602}, u'weihai': {'s': u'\u5a01\u6d77', 'id': 3710}, u'nanning': {'s': u'\u5357\u5b81', 'id': 4501}, u'huhehaote': {'s': u'\u547c\u548c\u6d69\u7279', 'id': 1501}, u'yangzhou': {'s': u'\u626c\u5dde', 'id': 3210}, u'foshan': {'s': u'\u4f5b\u5c71', 'id': 4406}, u'guiyang': {'s': u'\u8d35\u9633', 'id': 5201}, u'hangzhou': {'s': u'\u676d\u5dde', 'id': 3301}, u'fuzhou': {'s': u'\u798f\u5dde', 'id': 3501}, u'wuxi': {'s': u'\u65e0\u9521', 'id': 3202}, u'guizhou': {'s': u'\u8d35\u5dde', 'id': 24}, u'fujian': {'s': u'\u798f\u5efa', 'id': 14}, u'jingzhou': {'s': u'\u8346\u5dde', 'id': 4210}, u'huizhou': {'s': u'\u60e0\u5dde', 'id': 4413}, u'jinhua': {'s': u'\u91d1\u534e', 'id': 3307}, u'shaoxing': {'s': u'\u7ecd\u5174', 'id': 3306}, u'changsha': {'s': u'\u957f\u6c99', 'id': 4301}, u'jilin': {'s': u'\u5409\u6797', 'id': 2202}, u'jining': {'s': u'\u6d4e\u5b81', 'id': 3708}, u'wulumuqi': {'s': u'\u4e4c\u9c81\u6728\u9f50', 'id': 6501}, u'tianjin': {'s': u'\u5929\u6d25', 'id': 1201}, u'xingtai': {'s': u'\u90a2\u53f0', 'id': 1305}, u'dongguan': {'s': u'\u4e1c\u839e', 'id': 4419}, u'jiangsu': {'s': u'\u6c5f\u82cf', 'id': 11}, u'jiangmen': {'s': u'\u6c5f\u95e8', 'id': 4407}, u'qinhuangdao': {'s': u'\u79e6\u7687\u5c9b', 'id': 1303}, u'guangzhou': {'s': u'\u5e7f\u5dde', 'id': 4401}, u'hefei': {'s': u'\u5408\u80a5', 'id': 3401}, u'quanzhou': {'s': u'\u6cc9\u5dde', 'id': 3505}, u'wuhan': {'s': u'\u6b66\u6c49', 'id': 4201}, u'suzhou': {'s': u'\u82cf\u5dde', 'id': 3205}, u'hubei': {'s': u'\u6e56\u5317', 'id': 18}, u'luoyang': {'s': u'\u6d1b\u9633', 'id': 4103}, u'xuzhou': {'s': u'\u5f90\u5dde', 'id': 3203}, u'qingdao': {'s': u'\u9752\u5c9b', 'id': 3702}, u'chongqing': {'s': u'\u91cd\u5e86', 'id': 5001}, u'suqian': {'s': u'\u5bbf\u8fc1', 'id': 3213}, u'cangzhou': {'s': u'\u6ca7\u5dde', 'id': 1309}, u'shaanxi': {'s': u'\u9655\u897f', 'id': 27}, u'changzhou': {'s': u'\u5e38\u5dde', 'id': 3204}, u'hengshui': {'s': u'\u8861\u6c34', 'id': 1311}, u'shanghai': {'s': u'\u4e0a\u6d77', 'id': 3101}, u'weifang': {'s': u'\u6f4d\u574a', 'id': 3707}, u'chengdu': {'s': u'\u6210\u90fd', 'id': 5101}, u'guangxi': {'s': u'\u5e7f\u897f', 'id': 21}, u'xiangtan': {'s': u'\u6e58\u6f6d', 'id': 4303}, u'shenyang': {'s': u'\u6c88\u9633', 'id': 2101}, u'qinghai': {'s': u'\u9752\u6d77', 'id': 29}, u'nanjing': {'s': u'\u5357\u4eac', 'id': 3201}, u'zhuzhou': {'s': u'\u682a\u6d32', 'id': 4302}, u'langfang': {'s': u'\u5eca\u574a', 'id': 1310}, u'liaoning': {'s': u'\u8fbd\u5b81', 'id': 8}, u'taian': {'s': u'\u6cf0\u5b89', 'id': 3709}, u'guangdong': {'s': u'\u5e7f\u4e1c', 'id': 20}, u'zhenjiang': {'s': u'\u9547\u6c5f', 'id': 3211}, u'guilin': {'s': u'\u6842\u6797', 'id': 4503}, u'lianyungang': {'s': u'\u8fde\u4e91\u6e2f', 'id': 3207}, u'zhongshan': {'s': u'\u4e2d\u5c71', 'id': 4420}, u'gansu': {'s': u'\u7518\u8083', 'id': 28}, u'lanzhou': {'s': u'\u5170\u5dde', 'id': 6201}, u'hebei': {'s': u'\u6cb3\u5317', 'id': 5}, u'zhuhai': {'s': u'\u73e0\u6d77', 'id': 4404}, u'kunming': {'s': u'\u6606\u660e', 'id': 5301}, u'jiaxing': {'s': u'\u5609\u5174', 'id': 3304}, u'taizhou': {'s': u'\u53f0\u5dde', 'id': 3310}, u'dezhou': {'s': u'\u5fb7\u5dde', 'id': 3714}, u'wenzhou': {'s': u'\u6e29\u5dde', 'id': 3303}, u'shijiazhuang': {'s': u'\u77f3\u5bb6\u5e84', 'id': 1301}, u'liuzhou': {'s': u'\u67f3\u5dde', 'id': 4502}, u'ningbo': {'s': u'\u5b81\u6ce2', 'id': 3302}, u'yichang': {'s': u'\u5b9c\u660c', 'id': 4205}, u'jinan': {'s': u'\u6d4e\u5357', 'id': 3701}, u'beijing': {'s': u'\u5317\u4eac', 'id': 1101}, u'shandong': {'s': u'\u5c71\u4e1c', 'id': 16}, u'dalian': {'s': u'\u5927\u8fde', 'id': 2102}, u'hunan': {'s': u'\u6e56\u5357', 'id': 19}, u'xian': {'s': u'\u897f\u5b89', 'id': 6101}, u'shanxi': {'s': u'\u5c71\u897f', 'id': 6}, u'mianyang': {'s': u'\u7ef5\u9633', 'id': 5107}, u'nanyang': {'s': u'\u5357\u9633', 'id': 4113}, u'henan': {'s': u'\u6cb3\u5357', 'id': 17}, u'shenzhen': {'s': u'\u6df1\u5733', 'id': 4403}, u'baoding': {'s': u'\u4fdd\u5b9a', 'id': 1306}, u'xiangfan': {'s': u'\u8944\u6a0a', 'id': 4206}, u'yantai': {'s': u'\u70df\u53f0', 'id': 3706}, u'yancheng': {'s': u'\u76d0\u57ce', 'id': 3209}, u'haerbin': {'s': u'\u54c8\u5c14\u6ee8', 'id': 2301}, u'yunnan': {'s': u'\u4e91\u5357', 'id': 25}, u'changchun': {'s': u'\u957f\u6625', 'id': 2201}, u'baotou': {'s': u'\u5305\u5934', 'id': 1502}, u'nanchang': {'s': u'\u5357\u660c', 'id': 3601}, u'liaocheng': {'s': u'\u804a\u57ce', 'id': 3715}, u'haikou': {'s': u'\u6d77\u53e3', 'id': 4601}, u'daqing': {'s': u'\u5927\u5e86', 'id': 2306}, u'tangshan': {'s': u'\u5510\u5c71', 'id': 1302}, u'taiyuan': {'s': u'\u592a\u539f', 'id': 1401}, u'jiangxi': {'s': u'\u6c5f\u897f', 'id': 15}, u'anshan': {'s': u'\u978d\u5c71', 'id': 2103}}


class DidaSpider(CrawlSpider):
    name = "edeng"
    allowed_domains = ["edeng.cn"]
    start_urls = [
        "http://www.edeng.cn/13/pinche/"
    ]
    rules = (
        Rule(SgmlLinkExtractor(allow=('.*\.edeng\.cn\/jiedaoxinxi\/[0-9]+.html$', )), callback='parse_node'),
    )
    download_delay = 10
    con = pg.connect(dbname='merry', host='localhost', user='dida', passwd='loveu')

    def parse_node(self, response):
        self.log('this is node page %s' % response.url)

        hxs = HtmlXPathSelector(response)

        item = DidaItem()

        ori = 3
        oid = re.compile('.*jiedaoxinxi\/([0-9]+).html').match(response.url).group(1)

        # dad
        try:
            if hxs.select('//ul[@class="properties"]/li[1]/text()').extract()[0].find(u'车主'):
                item['dad'] = 1
            else:
                item['dad'] = 0
        except:
            print "**********" + oid + "dad error"
            return item

        # attach
        item['attach'] = ''
        try: item['attach'] = ''.join(hxs.select('//div[@class="edcontent"]/p/text() | //div[@class="edcontent"]/p//*/text()').extract()).strip()
        except: pass
        if len(item['attach']) <= 0:
            print "**********" + oid + "attach error"
            return item

        # saddr, eaddr
        try:
            item['saddr'] = hxs.select('//ul[@class="properties"]/li[2]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
            item['eaddr'] = hxs.select('//ul[@class="properties"]/li[3]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
        except:
            print "**********" + oid + "saddr, eaddr error"
            return item
    
        # sdate, stime
        xlist = []
        try:
            xstr = hxs.select('//ul[@class="properties3"]/li[1]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
            xlist = xstr.split()
            if len(xlist) < 2:
                xstr = hxs.select('//ul[@class="properties3"]/li[2]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
                xlist = xstr.split()
        except: pass
        if len(xlist) < 2:
            print "**********" + oid + "time error"
            return item
        item['sdate'] = xlist[0]
        item['stime'] = xlist[1]

        # repeat, sdate, stime
        try:
            xstr = hxs.select('//ul[@class="properties"]/li[4]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
            if xstr.find(u'其他') >= 0:
                item['repeat'] = 0
            elif xstr.find(u'每天') >= 0:
                item['repeat'] = 1
            elif xstr.find(u'周末') >= 0 or xstr.find(u'节假日') >= 0:
                item['repeat'] = 2
                item['sdate'] = '6,7'
            elif xstr.find(u'工作日') >= 0:
                item['repeat'] = 2
                item['sdate'] = '1,2,3,4,5'
            else:
                item['repeat'] = 0
        except:
            print "**********" + oid + "repeat error"
            return item

        # nick
        item['nick'] = ''
        try: item['nick'] = ''.join(hxs.select('//li[@class="username"]/a/text()').extract()).replace(" ", "").replace('\n', '').replace(u'\xa0', u'')
        except:
            print "**********" + oid + "nick error"
            return item

        # phone
        item['phone'] = ''
        try: item['phone'] = ''.join(hxs.select('//li[@class="contactphonefc"]/a/text()').extract()).replace(" ", "").replace('\n', '').replace(u'\xa0', u'')
        except:
            print "**********" + oid + "phone error"
            return item
        
        # contact
        item['contact'] = ''
        try: item['contact'] = ''.join(hxs.select('//li[@class="email"]/a/text()').extract()).replace(" ", "").replace('\n', '').replace(u'\xa0', u'')
        except: pass
        
        item['city'] = 999999
        try:
            p = re.compile('http://(.*)\.edeng\.cn.*')
            item['city'] = cities[p.match(response.url).group(1)]['id']
        except: pass

        r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
        if (r.ntuples() > 0): print oid + "exist already!"
        else:
            sql = "INSERT INTO plan (ori, oid, dad, saddr, eaddr, repeat, sdate, stime, attach, phone, contact, nick, cityid, statu, ourl) VALUES ("
            sql += `ori` + ", "
            sql += "'"+oid+"'" + ", "

            sql += `item['dad']` + ","
            sql += "'"+item['saddr']+"'" + ", "
            sql += "'"+item['eaddr']+"'" + ", "

            sql += `item['repeat']` + ","

            sql += "'"+item['sdate']+"'" + ", "
            sql += "'"+item['stime']+"'" + ", "
            sql += "'"+item['attach']+"'" + ", "
            sql += "'"+item['phone']+"'" + ", "
            sql += "'"+item['contact']+"'" + ", "
            sql += "'"+item['nick']+"'" + ", "
            
            sql += `item['city']` + ", "
            sql += `10` + ", "

            sql += "'" +response.url+ "'"

            sql += ")"
            
            #print sql
            self.con.query(sql.encode('utf-8'))

        return item
