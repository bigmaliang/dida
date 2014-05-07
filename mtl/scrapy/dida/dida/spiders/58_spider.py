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

cities = {u'gt': {'s': u'\u9986\u9676', 'id': 2933}, u'ale': {'s': u'\u963f\u62c9\u5c14', 'id': 659002}, u'gz': {'s': u'\u5e7f\u5dde', 'id': 4401}, u'gy': {'s': u'\u8d35\u9633', 'id': 5201}, u'gg': {'s': u'\u8d35\u6e2f', 'id': 4508}, u'ga': {'s': u'\u5e7f\u5b89', 'id': 5116}, u'gn': {'s': u'\u7518\u5357', 'id': 6230}, u'gl': {'s': u'\u6842\u6797', 'id': 4503}, u'tz': {'s': u'\u53f0\u5dde', 'id': 3310}, u'ty': {'s': u'\u592a\u539f', 'id': 1401}, u'tw': {'s': u'\u53f0\u6e7e'}, u'tr': {'s': u'\u94dc\u4ec1', 'id': 5222}, u'ts': {'s': u'\u5510\u5c71', 'id': 1302}, u'zmd': {'s': u'\u9a7b\u9a6c\u5e97', 'id': 4117}, u'tl': {'s': u'\u94c1\u5cad', 'id': 2112}, u'anshun': {'s': u'\u5b89\u987a', 'id': 5204}, u'puyang': {'s': u'\u6fee\u9633', 'id': 4109}, u'th': {'s': u'\u901a\u5316', 'id': 2205}, u'tc': {'s': u'\u94dc\u5ddd', 'id': 6102}, u'ta': {'s': u'\u6cf0\u5b89', 'id': 3709}, u'jiyuan': {'s': u'\u6d4e\u6e90', 'id': 410881}, u'wjq': {'s': u'\u4e94\u5bb6\u6e20', 'id': 659004}, u'scnj': {'s': u'\u5185\u6c5f', 'id': 5110}, u'ankang': {'s': u'\u5b89\u5eb7', 'id': 6109}, u'zh': {'s': u'\u73e0\u6d77', 'id': 4404}, u'yuzhou': {'s': u'\u79b9\u5dde', 'id': 4296}, u'zj': {'s': u'\u9547\u6c5f', 'id': 3211}, u'zk': {'s': u'\u5468\u53e3', 'id': 4116}, u'zd': {'s': u'\u6b63\u5b9a', 'id': 2881}, u'tongliao': {'s': u'\u901a\u8fbd', 'id': 1505}, u'zb': {'s': u'\u6dc4\u535a', 'id': 3703}, u'zc': {'s': u'\u8bf8\u57ce', 'id': 4122}, u'zx': {'s': u'\u8d75\u53bf', 'id': 2891}, u'zy': {'s': u'\u8d44\u9633', 'id': 5120}, u'zz': {'s': u'\u90d1\u5dde', 'id': 4101}, u'zt': {'s': u'\u662d\u901a', 'id': 5306}, u'zw': {'s': u'\u4e2d\u536b', 'id': 6405}, u'zq': {'s': u'\u8087\u5e86', 'id': 4412}, u'zs': {'s': u'\u4e2d\u5c71', 'id': 4420}, u'xinzhou': {'s': u'\u5ffb\u5dde', 'id': 1409}, u'yingtan': {'s': u'\u9e70\u6f6d', 'id': 3606}, u'bc': {'s': u'\u767d\u57ce', 'id': 2208}, u'tongling': {'s': u'\u94dc\u9675', 'id': 3407}, u'huaibei': {'s': u'\u6dee\u5317', 'id': 3406}, u'zhangzhou': {'s': u'\u6f33\u5dde', 'id': 3506}, u'tianshui': {'s': u'\u5929\u6c34', 'id': 6205}, u'bj': {'s': u'\u5317\u4eac', 'id': 1101}, u'changzhi': {'s': u'\u957f\u6cbb', 'id': 1404}, u'wuhai': {'s': u'\u4e4c\u6d77', 'id': 1503}, u'sjz': {'s': u'\u77f3\u5bb6\u5e84', 'id': 1301}, u'aks': {'s': u'\u963f\u514b\u82cf', 'id': 6529}, u'hshi': {'s': u'\u9ec4\u77f3', 'id': 4202}, u'luoyang': {'s': u'\u6d1b\u9633', 'id': 4103}, u'rkz': {'s': u'\u65e5\u5580\u5219', 'id': 5423}, u'qingxu': {'s': u'\u6e05\u5f90', 'id': 3052}, u'hezhou': {'s': u'\u8d3a\u5dde', 'id': 4511}, u'zunyi': {'s': u'\u9075\u4e49', 'id': 5203}, u'dandong': {'s': u'\u4e39\u4e1c', 'id': 2106}, u'anqing': {'s': u'\u5b89\u5e86', 'id': 3408}, u'tm': {'s': u'\u5929\u95e8', 'id': 429006}, u'tj': {'s': u'\u5929\u6d25', 'id': 1201}, u'bygl': {'s': u'\u5df4\u97f3\u90ed\u695e', 'id': 6528}, u'qdn': {'s': u'\u9ed4\u4e1c\u5357', 'id': 5226}, u'jms': {'s': u'\u4f73\u6728\u65af', 'id': 2308}, u'yxx': {'s': u'\u6c38\u65b0', 'id': 4027}, u'guyuan': {'s': u'\u56fa\u539f', 'id': 6404}, u'mg': {'s': u'\u660e\u6e2f'}, u'mm': {'s': u'\u8302\u540d', 'id': 4409}, u'linyixian': {'s': u'\u4e34\u7317', 'id': 3109}, u'ms': {'s': u'\u7709\u5c71', 'id': 5114}, u'jdz': {'s': u'\u666f\u5fb7\u9547', 'id': 3602}, u'mz': {'s': u'\u6885\u5dde', 'id': 4414}, u'benxi': {'s': u'\u672c\u6eaa', 'id': 2105}, u'bazhong': {'s': u'\u5df4\u4e2d', 'id': 5119}, u'mas': {'s': u'\u9a6c\u978d\u5c71', 'id': 3405}, u'fs': {'s': u'\u4f5b\u5c71', 'id': 4406}, u'shiyan': {'s': u'\u5341\u5830', 'id': 4203}, u'fx': {'s': u'\u961c\u65b0', 'id': 2109}, u'fy': {'s': u'\u961c\u9633', 'id': 3412}, u'fz': {'s': u'\u798f\u5dde', 'id': 3501}, u'snj': {'s': u'\u795e\u519c\u67b6', 'id': 429021}, u'bozhou': {'s': u'\u4eb3\u5dde', 'id': 3416}, u'sz': {'s': u'\u6df1\u5733', 'id': 4403}, u'sy': {'s': u'\u6c88\u9633', 'id': 2101}, u'sx': {'s': u'\u7ecd\u5174', 'id': 3306}, u'sr': {'s': u'\u4e0a\u9976', 'id': 3611}, u'sq': {'s': u'\u5546\u4e18', 'id': 4114}, u'sp': {'s': u'\u56db\u5e73', 'id': 2203}, u'sw': {'s': u'\u6c55\u5c3e', 'id': 4415}, u'su': {'s': u'\u82cf\u5dde', 'id': 3205}, u'st': {'s': u'\u6c55\u5934', 'id': 4405}, u'nanchong': {'s': u'\u5357\u5145', 'id': 5113}, u'sh': {'s': u'\u4e0a\u6d77', 'id': 3101}, u'sn': {'s': u'\u5c71\u5357', 'id': 5422}, u'sm': {'s': u'\u4e09\u660e', 'id': 3504}, u'sl': {'s': u'\u5546\u6d1b', 'id': 6110}, u'sg': {'s': u'\u97f6\u5173', 'id': 4402}, u'sd': {'s': u'\u987a\u5fb7', 'id': 4621}, u'baishan': {'s': u'\u767d\u5c71', 'id': 2206}, u'songyuan': {'s': u'\u677e\u539f', 'id': 2207}, u'lincang': {'s': u'\u4e34\u6ca7', 'id': 5309}, u'lf': {'s': u'\u5eca\u574a', 'id': 1310}, u'liaoyang': {'s': u'\u8fbd\u9633', 'id': 2110}, u'lb': {'s': u'\u6765\u5bbe', 'id': 4513}, u'lc': {'s': u'\u804a\u57ce', 'id': 3715}, u'la': {'s': u'\u516d\u5b89', 'id': 3415}, u'kaifeng': {'s': u'\u5f00\u5c01', 'id': 4102}, u'linyi': {'s': u'\u4e34\u6c82', 'id': 3713}, u'lj': {'s': u'\u4e3d\u6c5f', 'id': 5307}, u'lw': {'s': u'\u83b1\u829c', 'id': 3712}, u'ls': {'s': u'\u4e50\u5c71', 'id': 5111}, u'lz': {'s': u'\u5170\u5dde', 'id': 6201}, u'ly': {'s': u'\u9f99\u5ca9', 'id': 3508}, u'yk': {'s': u'\u8425\u53e3', 'id': 2108}, u'hainan': {'s': u'\u6d77\u5357', 'id': 22}, u'qinzhou': {'s': u'\u94a6\u5dde', 'id': 4507}, u'ya': {'s': u'\u96c5\u5b89', 'id': 5118}, u'yc': {'s': u'\u5b9c\u660c', 'id': 4205}, u'yb': {'s': u'\u5b9c\u5bbe', 'id': 5115}, u'yf': {'s': u'\u4e91\u6d6e', 'id': 4453}, u'yy': {'s': u'\u5cb3\u9633', 'id': 4306}, u'yx': {'s': u'\u7389\u6eaa', 'id': 5304}, u'yz': {'s': u'\u626c\u5dde', 'id': 3210}, u'qhd': {'s': u'\u79e6\u7687\u5c9b', 'id': 1303}, u'yq': {'s': u'\u9633\u6cc9', 'id': 1403}, u'fushun': {'s': u'\u629a\u987a', 'id': 2104}, u'ys': {'s': u'\u7389\u6811', 'id': 6327}, u'yt': {'s': u'\u70df\u53f0', 'id': 3706}, u'taishan': {'s': u'\u53f0\u5c71', 'id': 4627}, u'nujiang': {'s': u'\u6012\u6c5f', 'id': 5333}, u'sanya': {'s': u'\u4e09\u4e9a', 'id': 4602}, u'zhuzhou': {'s': u'\u682a\u6d32', 'id': 4302}, u'linzhi': {'s': u'\u6797\u829d', 'id': 5426}, u'yanan': {'s': u'\u5ef6\u5b89', 'id': 6106}, u'heyuan': {'s': u'\u6cb3\u6e90', 'id': 4416}, u'jingzhou': {'s': u'\u8346\u5dde', 'id': 4210}, u'hlr': {'s': u' \u6d77\u62c9\u5c14'}, u'haibei': {'s': u'\u6d77\u5317', 'id': 6322}, u'ez': {'s': u'\u9102\u5dde', 'id': 4207}, u'hld': {'s': u'\u846b\u82a6\u5c9b', 'id': 2114}, u'es': {'s': u'\u6069\u65bd', 'id': 4228}, u'heze': {'s': u'\u83cf\u6cfd', 'id': 3717}, u'smx': {'s': u'\u4e09\u95e8\u5ce1', 'id': 4112}, u'changde': {'s': u'\u5e38\u5fb7', 'id': 4307}, u'changdu': {'s': u'\u660c\u90fd', 'id': 5421}, u'klmy': {'s': u'\u514b\u62c9\u739b\u4f9d', 'id': 6502}, u'dazhou': {'s': u'\u8fbe\u5dde', 'id': 5117}, u'zjk': {'s': u'\u5f20\u5bb6\u53e3', 'id': 1307}, u'zjj': {'s': u'\u5f20\u5bb6\u754c', 'id': 4308}, u'shuyang': {'s': u'\u6cad\u9633', 'id': 3680}, u'xj': {'s': u'\u4e4c\u9c81\u6728\u9f50', 'id': 6501}, u'xn': {'s': u'\u897f\u5b81', 'id': 6301}, u'xl': {'s': u'\u9521\u6797\u90ed\u52d2\u76df', 'id': 1525}, u'xm': {'s': u'\u53a6\u95e8', 'id': 3502}, u'xc': {'s': u'\u8bb8\u660c', 'id': 4110}, u'xa': {'s': u'\u897f\u5b89', 'id': 6101}, u'xf': {'s': u'\u8944\u9633'}, u'xz': {'s': u'\u5f90\u5dde', 'id': 3203}, u'xx': {'s': u'\u65b0\u4e61', 'id': 4107}, u'xy': {'s': u'\u4fe1\u9633', 'id': 4115}, u'chongzuo': {'s': u'\u5d07\u5de6', 'id': 4514}, u'xt': {'s': u'\u90a2\u53f0', 'id': 1305}, u'lvliang': {'s': u'\u5415\u6881', 'id': 1411}, u'wuzhong': {'s': u'\u5434\u5fe0', 'id': 6403}, u'jinchang': {'s': u'\u91d1\u660c', 'id': 6203}, u'zg': {'s': u'\u81ea\u8d21', 'id': 5103}, u'ganzi': {'s': u'\u7518\u5b5c', 'id': 5133}, u'taizhou': {'s': u'\u6cf0\u5dde', 'id': 3212}, u'hlbe': {'s': u'\u547c\u4f26\u8d1d\u5c14', 'id': 1507}, u'chenzhou': {'s': u'\u90f4\u5dde', 'id': 4310}, u'yiyang': {'s': u'\u76ca\u9633', 'id': 4309}, u'tmsk': {'s': u'\u56fe\u6728\u8212\u514b', 'id': 659003}, u'kzls': {'s': u'\u514b\u5b5c\u52d2\u82cf', 'id': 6530}, u'panzhihua': {'s': u'\u6500\u679d\u82b1', 'id': 5104}, u'mianyang': {'s': u'\u7ef5\u9633', 'id': 5107}, u'yichun': {'s': u'\u5b9c\u6625', 'id': 3609}, u'alsm': {'s': u'\u963f\u62c9\u5584\u76df', 'id': 1529}, u'baise': {'s': u'\u767e\u8272', 'id': 4510}, u'suihua': {'s': u'\u7ee5\u5316', 'id': 2312}, u'fcg': {'s': u'\u9632\u57ce\u6e2f', 'id': 4506}, u'chifeng': {'s': u'\u8d64\u5cf0', 'id': 1504}, u'chuzhou': {'s': u'\u6ec1\u5dde', 'id': 3411}, u'haikou': {'s': u'\u6d77\u53e3', 'id': 4601}, u'lps': {'s': u'\u516d\u76d8\u6c34', 'id': 5202}, u'km': {'s': u'\u6606\u660e', 'id': 5301}, u'kl': {'s': u'\u57a6\u5229', 'id': 4100}, u'ks': {'s': u'\u5580\u4ec0', 'id': 6531}, u'guangyuan': {'s': u'\u5e7f\u5143', 'id': 5108}, u'yinchuan': {'s': u'\u94f6\u5ddd', 'id': 6401}, u'heihe': {'s': u'\u9ed1\u6cb3', 'id': 2311}, u'szs': {'s': u'\u77f3\u5634\u5c71', 'id': 6402}, u'wzs': {'s': u'\u4e94\u6307\u5c71', 'id': 469001}, u'wuhu': {'s': u'\u829c\u6e56', 'id': 3402}, u'zhangbei': {'s': u'\u5f20\u5317', 'id': 2986}, u'hrb': {'s': u'\u54c8\u5c14\u6ee8', 'id': 2301}, u'dl': {'s': u'\u5927\u8fde', 'id': 2102}, u'dh': {'s': u'\u5fb7\u5b8f', 'id': 5331}, u'dg': {'s': u'\u4e1c\u839e', 'id': 4419}, u'deyang': {'s': u'\u5fb7\u9633', 'id': 5106}, u'dz': {'s': u'\u5fb7\u5dde', 'id': 3714}, u'dx': {'s': u'\u5b9a\u897f', 'id': 6211}, u'dy': {'s': u'\u4e1c\u8425', 'id': 3705}, u'zhangqiu': {'s': u'\u7ae0\u4e18', 'id': 4071}, u'dt': {'s': u'\u5927\u540c', 'id': 1402}, u'dq': {'s': u'\u5927\u5e86', 'id': 2306}, u'diqing': {'s': u'\u8fea\u5e86', 'id': 5334}, u'jincheng': {'s': u'\u664b\u57ce', 'id': 1405}, u'weihai': {'s': u'\u5a01\u6d77', 'id': 3710}, u'shz': {'s': u'\u77f3\u6cb3\u5b50', 'id': 659001}, u'qz': {'s': u'\u6cc9\u5dde', 'id': 3505}, u'jiaozuo': {'s': u'\u7126\u4f5c', 'id': 4108}, u'qd': {'s': u'\u9752\u5c9b', 'id': 3702}, u'qj': {'s': u'\u66f2\u9756', 'id': 5303}, u'qn': {'s': u'\u9ed4\u5357', 'id': 5227}, u'huizhou': {'s': u'\u60e0\u5dde', 'id': 4413}, u'bengbu': {'s': u' \u868c\u57e0'}, u'kel': {'s': u'\u5e93\u5c14\u52d2', 'id': 5609}, u'shuozhou': {'s': u'\u6714\u5dde', 'id': 1406}, u'qqhr': {'s': u'\u9f50\u9f50\u54c8\u5c14', 'id': 2302}, u'qth': {'s': u'\u4e03\u53f0\u6cb3', 'id': 2309}, u'honghe': {'s': u'\u7ea2\u6cb3', 'id': 5325}, u'bycem': {'s': u'\u5df4\u5f66\u6dd6\u5c14\u76df'}, u'wf': {'s': u'\u6f4d\u574a', 'id': 3707}, u'yich': {'s': u'\u4f0a\u6625', 'id': 2307}, u'jx': {'s': u'\u5609\u5174', 'id': 3304}, u'jy': {'s': u'\u63ed\u9633', 'id': 4452}, u'jz': {'s': u'\u664b\u4e2d', 'id': 1407}, u'wn': {'s': u'\u6e2d\u5357', 'id': 6105}, u'yanling': {'s': u'\u9122\u9675', 'id': 4294}, u'jq': {'s': u'\u9152\u6cc9', 'id': 6209}, u'wh': {'s': u'\u6b66\u6c49', 'id': 4201}, u'jl': {'s': u'\u5409\u6797', 'id': 2202}, u'jm': {'s': u'\u6c5f\u95e8', 'id': 4407}, u'jn': {'s': u'\u6d4e\u5357', 'id': 3701}, u'ws': {'s': u'\u6587\u5c71', 'id': 5326}, u'jj': {'s': u'\u4e5d\u6c5f', 'id': 3604}, u'wz': {'s': u'\u6e29\u5dde', 'id': 3303}, u'wx': {'s': u'\u65e0\u9521', 'id': 3202}, u'cangzhou': {'s': u'\u6ca7\u5dde', 'id': 1309}, u'pds': {'s': u'\u5e73\u9876\u5c71', 'id': 4104}, u'lasa': {'s': u'\u62c9\u8428', 'id': 5401}, u'dafeng': {'s': u'\u5927\u4e30', 'id': 3658}, u'jh': {'s': u'\u91d1\u534e', 'id': 3307}, u'xianning': {'s': u'\u54b8\u5b81', 'id': 4212}, u'rizhao': {'s': u'\u65e5\u7167', 'id': 3711}, u'cn': {'s': u'\u5176\u4ed6'}, u'cc': {'s': u'\u957f\u6625', 'id': 2201}, u'cd': {'s': u'\u6210\u90fd', 'id': 5101}, u'qingyuan': {'s': u'\u6e05\u8fdc', 'id': 4418}, u'cz': {'s': u'\u5e38\u5dde', 'id': 3204}, u'cy': {'s': u'\u671d\u9633', 'id': 2113}, u'cx': {'s': u'\u695a\u96c4', 'id': 5323}, u'wuzhou': {'s': u'\u68a7\u5dde', 'id': 4504}, u'zaozhuang': {'s': u'\u67a3\u5e84', 'id': 3704}, 'cs': {'s': u'\u957f\u6c99', 'id': 4301}, u'cq': {'s': u'\u91cd\u5e86', 'id': 5001}, u'zhangye': {'s': u'\u5f20\u6396', 'id': 6207}, u'pt': {'s': u'\u8386\u7530', 'id': 3503}, u'px': {'s': u'\u840d\u4e61', 'id': 3603}, u'ja': {'s': u'\u5409\u5b89', 'id': 3608}, u'xam': {'s': u'\u5174\u5b89\u76df', 'id': 1522}, u'pe': {'s': u'\u666e\u6d31', 'id': 5174}, u'pj': {'s': u'\u76d8\u9526', 'id': 2111}, u'yanbian': {'s': u'\u5ef6\u8fb9', 'id': 2224}, u'pl': {'s': u'\u5e73\u51c9', 'id': 6208}, u'changge': {'s': u'\u957f\u845b', 'id': 4297}, u'wfd': {'s': u'\u74e6\u623f\u5e97', 'id': 3286}, u'lyg': {'s': u'\u8fde\u4e91\u6e2f', 'id': 3207}, u'suzhou': {'s': u'\u5bbf\u5dde', 'id': 3413}, u'wuyishan': {'s': u'\u6b66\u5937\u5c71', 'id': 3944}, u'wuwei': {'s': u'\u6b66\u5a01', 'id': 6206}, u'yj': {'s': u'\u9633\u6c5f', 'id': 4417}, u'yl': {'s': u'\u6986\u6797', 'id': 6108}, u'guoluo': {'s': u'\u679c\u6d1b', 'id': 6326}, u'tlf': {'s': u'\u5410\u9c81\u756a', 'id': 6521}, u'huzhou': {'s': u'\u6e56\u5dde', 'id': 3305}, u'qianjiang': {'s': u'\u6f5c\u6c5f', 'id': 429005}, u'yancheng': {'s': u'\u76d0\u57ce', 'id': 3209}, u'nj': {'s': u'\u5357\u4eac', 'id': 3201}, u'sys': {'s': u'\u53cc\u9e2d\u5c71', 'id': 2305}, u'jinzhou': {'s': u'\u9526\u5dde', 'id': 2107}, u'hanzhong': {'s': u'\u6c49\u4e2d', 'id': 6107}, u'qingyang': {'s': u'\u5e86\u9633', 'id': 6210}, u'yuncheng': {'s': u'\u8fd0\u57ce', 'id': 1408}, u'ganzhou': {'s': u'\u8d63\u5dde', 'id': 3607}, u'haidong': {'s': u'\u6d77\u4e1c', 'id': 6321}, u'suizhou': {'s': u'\u968f\u5dde', 'id': 4213}, u'xuancheng': {'s': u'\u5ba3\u57ce', 'id': 3418}, u'changji': {'s': u'\u660c\u5409', 'id': 6523}, u'shaoyang': {'s': u'\u90b5\u9633', 'id': 4305}, u'xiangtan': {'s': u'\u6e58\u6f6d', 'id': 4303}, u'liaoyuan': {'s': u'\u8fbd\u6e90', 'id': 2204}, u'xiantao': {'s': u'\u4ed9\u6843', 'id': 429004}, u'liangshan': {'s': u'\u51c9\u5c71', 'id': 5134}, u'zhanjiang': {'s': u'\u6e5b\u6c5f', 'id': 4408}, u'qxn': {'s': u'\u9ed4\u897f\u5357', 'id': 5223}, u'jyg': {'s': u'\u5609\u5cea\u5173', 'id': 6202}, u'fuzhou': {'s': u'\u629a\u5dde', 'id': 3610}, u'tongcheng': {'s': u'\u6850\u57ce', 'id': 3822}, u'linfen': {'s': u'\u4e34\u6c7e', 'id': 1410}, u'dxal': {'s': u'\u5927\u5174\u5b89\u5cad', 'id': 2327}, u'yulin': {'s': u'\u7389\u6797', 'id': 4509}, u'wlcb': {'s': u'\u4e4c\u5170\u5bdf\u5e03', 'id': 1509}, u'yongzhou': {'s': u'\u6c38\u5dde', 'id': 4311}, u'chaozhou': {'s': u'\u6f6e\u5dde', 'id': 4451}, u'jining': {'s': u'\u6d4e\u5b81', 'id': 3708}, u'suining': {'s': u'\u9042\u5b81', 'id': 5109}, u'bd': {'s': u'\u4fdd\u5b9a', 'id': 1306}, u'linxia': {'s': u'\u4e34\u590f', 'id': 6229}, u'bn': {'s': u'\u897f\u53cc\u7248\u7eb3', 'id': 5328}, u'bh': {'s': u'\u5317\u6d77', 'id': 4505}, u'quzhou': {'s': u'\u8862\u5dde', 'id': 3308}, u'bt': {'s': u'\u5305\u5934', 'id': 1502}, u'pld': {'s': u'\u5e84\u6cb3', 'id': 3288}, u'lishui': {'s': u'\u4e3d\u6c34', 'id': 3311}, u'hegang': {'s': u'\u9e64\u5c97', 'id': 2304}, u'bs': {'s': u'\u4fdd\u5c71', 'id': 5305}, u'by': {'s': u'\u767d\u94f6', 'id': 6204}, u'bz': {'s': u'\u6ee8\u5dde', 'id': 3716}, u'suqian': {'s': u'\u5bbf\u8fc1', 'id': 3213}, u'mdj': {'s': u'\u7261\u4e39\u6c5f', 'id': 2310}, u'hexian': {'s': u'\u548c\u53bf', 'id': 3855}, u'chengde': {'s': u'\u627f\u5fb7', 'id': 1308}, u'yangchun': {'s': u'\u9633\u6625', 'id': 4680}, u'betl': {'s': u'\u535a\u5c14\u5854\u62c9', 'id': 6527}, u'hz': {'s': u'\u676d\u5dde', 'id': 3301}, u'hx': {'s': u'\u6d77\u897f', 'id': 6328}, u'hy': {'s': u'\u8861\u9633', 'id': 4304}, u'hs': {'s': u'\u8861\u6c34', 'id': 1311}, u'hq': {'s': u'\u970d\u90b1', 'id': 3859}, u'ht': {'s': u'\u548c\u7530', 'id': 6532}, u'hu': {'s': u'\u547c\u548c\u6d69\u7279', 'id': 1501}, u'hk': {'s': u'\u9999\u6e2f'}, u'hh': {'s': u'\u6000\u5316', 'id': 4312}, u'hn': {'s': u'\u6dee\u5357', 'id': 3404}, u'xiaogan': {'s': u'\u5b5d\u611f', 'id': 4209}, u'xianyang': {'s': u'\u54b8\u9633', 'id': 6104}, u'hb': {'s': u'\u9e64\u58c1', 'id': 4106}, u'hc': {'s': u'\u6cb3\u6c60', 'id': 4512}, u'ha': {'s': u'\u6dee\u5b89', 'id': 3208}, u'hf': {'s': u'\u5408\u80a5', 'id': 3401}, u'hg': {'s': u'\u9ec4\u5188', 'id': 4211}, u'hd': {'s': u'\u90af\u90f8', 'id': 1304}, u'zhoushan': {'s': u'\u821f\u5c71', 'id': 3309}, u'erds': {'s': u'\u9102\u5c14\u591a\u65af', 'id': 1506}, u'luzhou': {'s': u'\u6cf8\u5dde', 'id': 5105}, u'liuzhou': {'s': u'\u67f3\u5dde', 'id': 4502}, u'luohe': {'s': u'\u6f2f\u6cb3', 'id': 4111}, u'chizhou': {'s': u'\u6c60\u5dde', 'id': 3417}, u'jingmen': {'s': u'\u8346\u95e8', 'id': 4208}, u'ab': {'s': u'\u963f\u575d', 'id': 4994}, u'ld': {'s': u'\u5a04\u5e95', 'id': 4313}, u'ln': {'s': u'\u9647\u5357', 'id': 6212}, u'huangnan': {'s': u'\u9ec4\u5357', 'id': 6323}, u'am': {'s': u'\u6fb3\u95e8'}, u'al': {'s': u'\u963f\u91cc', 'id': 5425}, u'ch': {'s': u'\u5de2\u6e56', 'id': 3414}, u'as': {'s': u'\u978d\u5c71', 'id': 2103}, u'yili': {'s': u'\u4f0a\u7281', 'id': 6540}, u'bijie': {'s': u'\u6bd5\u8282', 'id': 5224}, u'huangshan': {'s': u'\u9ec4\u5c71', 'id': 3410}, u'ay': {'s': u'\u5b89\u9633', 'id': 4105}, u'jixi': {'s': u'\u9e21\u897f', 'id': 2303}, u'hami': {'s': u'\u54c8\u5bc6', 'id': 6522}, u'yiwu': {'s': u'\u4e49\u4e4c', 'id': 3742}, u'nn': {'s': u'\u5357\u5b81', 'id': 4501}, u'nb': {'s': u'\u5b81\u6ce2', 'id': 3302}, u'nc': {'s': u'\u5357\u660c', 'id': 3601}, u'nd': {'s': u'\u5b81\u5fb7', 'id': 3509}, u'ny': {'s': u'\u5357\u9633', 'id': 4113}, u'np': {'s': u'\u5357\u5e73', 'id': 3507}, u'nq': {'s': u'\u90a3\u66f2', 'id': 5424}, u'nt': {'s': u'\u5357\u901a', 'id': 3206}, u'xinyu': {'s': u'\u65b0\u4f59', 'id': 3605}, u'dali': {'s': u'\u5927\u7406', 'id': 5329}, u'baoji': {'s': u'\u5b9d\u9e21', 'id': 6103}, u'xiangxi': {'s': u'\u6e58\u897f'}, u'dingzhou': {'s': u'\u5b9a\u5dde', 'id': 2978}}


class DidaSpider(CrawlSpider):
    name = "58"
    allowed_domains = ["58.com"]
    start_urls = [
        "http://www.58.com/pinche/changecity/"
    ]
    rules = (
        Rule(SgmlLinkExtractor(allow=('.*\.58\.com\/pinche\/$', )), callback='parse_site'),
    )
    download_delay = 10
    con = pg.connect(dbname='merry', host='localhost', user='dida', passwd='loveu')
    
    def parse_site(self, response):
        for link in SgmlLinkExtractor(allow='.*pinche\/[0-9]+x\.shtml$').extract_links(response):
            ori = 1
            oid = re.compile('.*pinche\/([0-9]+)x.shtml').match(link.url).group(1)

            r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
            if (r.ntuples() > 0): print oid + "exist already!"
            else: yield Request(link.url, callback=self.parse_node)

        for link in SgmlLinkExtractor(allow='.*pinche\/pn[0-9]+\/$').extract_links(response):
            yield Request(link.url, callback=self.parse_site_child)

    def parse_site_child(self, response):
        for link in SgmlLinkExtractor(allow='.*pinche\/[0-9]+x\.shtml$').extract_links(response):
            ori = 1
            oid = re.compile('.*pinche\/([0-9]+)x.shtml').match(link.url).group(1)

            r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
            if (r.ntuples() > 0): print oid + "exist already!"
            else: yield Request(link.url, callback=self.parse_node)

    def parse_node(self, response):
        self.log('this is node page %s' % response.url)

        hxs = HtmlXPathSelector(response)

        item = DidaItem()

        ori = 1
        oid = re.compile('.*pinche\/([0-9]+)x.shtml').match(response.url).group(1)
        
        try:
            if hxs.select('//div[@class="w headline"]/h1/text()').extract()[0].find(u'有车'):
                item['dad'] = 1
            else:
                item['dad'] = 0
        except:
            print "**********" + oid + "dad error"
            return item

        try:
            xstr = hxs.select('//ul[@class="info"]/li[1]/text()').extract()[0].strip()
            ystr = hxs.select('//ul[@class="info"]/li[2]/text()').extract()[0].strip().replace(u"\xa0", u" ")
            ylist = ystr.split(' ')
            zstr = hxs.select('//ul[@class="info"]/li[3]/a/text()').extract()[0].strip()

            item['saddr'] = zstr.split(u'→')[0].strip()
            item['eaddr'] = zstr.split(u'→')[1].strip()
        except:
            print "**********" + oid + "string error"
            return item

        if (ystr.find(u'每天') >= 0):
            item['repeat'] = 1
            item['sdate'] = ' '
            if len(ylist) > 1: item['stime'] = ylist[1]
            else: item['stime'] = '08:00:00'
        elif (ystr.find(u'工作日') >= 0):
            item['repeat'] = 2
            item['sdate'] = '1,2,3,4,5'
            if len(ylist) > 1: item['stime'] = ylist[1]
            else: item['stime'] = '08:00:00'
        elif (ystr.find(u'周末') >= 0):
            item['repeat'] = 2
            item['sdate'] = '6,7'
            if len(ylist) > 1: item['stime'] = ylist[1]
            else: item['stime'] = '08:00:00'
        else:
            item['repeat'] = 0
            item['sdate'] = ystr.split(' ')[0]
            if len(ylist) > 1: item['stime'] = ylist[1]
            else: item['stime'] = '08:00:00'
        
        item['attach'] = ''
        try: item['attach'] = ''.join(hxs.select('//div[@class="maincon"]/text() | //div[@class="maincon"]//*/text()').extract()).strip()
        except:
             print "**********" + oid + "attach error"
             return item

        try:
            xnode = hxs.select('//span[@id="t_phone"]/script/text()')
            if len(xnode) < 1:
                item['phone'] = hxs.select('//span[@id="t_phone"]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
                if item['phone'] == u'':
                    item['phone'] = hxs.select('//span[@id="t_phone"]/img/@src').extract()[0]
            else:
                item['phone'] = xnode.re('.*(http:[^\']+).*')[0]
        except:
             print "**********" + oid + "phone error"
             return item
        
        item['nick'] = ''
        try: item['nick'] = hxs.select('//div[@class="user"]/script/text()').re('username:\'(.*)\'')[0]
        except:
             print "**********" + oid + "nick error"
             return item
        
        item['city'] = 999999
        try:
            p = re.compile('http://(.*)\.58\.com.*')
            item['city'] = cities[p.match(response.url).group(1)]['id']
        except: pass

        r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
        if (r.ntuples() > 0): print oid + "exist already!"
        else:
            sql = "INSERT INTO plan (ori, oid, dad, saddr, eaddr, repeat, sdate, stime, attach, phone, nick, cityid, statu, ourl) VALUES ("
            sql += `1` + ", "
            sql += "'"+oid+"'" + ", "

            sql += `item['dad']` + ","
            sql += "'"+item['saddr']+"'" + ", "
            sql += "'"+item['eaddr']+"'" + ", "

            sql += `item['repeat']` + ","

            sql += "'"+item['sdate']+"'" + ", "
            sql += "'"+item['stime']+"'" + ", "
            sql += "'"+item['attach']+"'" + ", "
            sql += "'"+item['phone']+"'" + ", "
            sql += "'"+item['nick']+"'" + ", "
            
            sql += `item['city']` + ", "
            sql += `10` + ", "

            sql += "'" +response.url+ "'"

            sql += ")"
            
            #print sql
            self.con.query(sql.encode('utf-8'))

        return item
