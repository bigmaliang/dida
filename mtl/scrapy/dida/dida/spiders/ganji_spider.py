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

cities = {u'ali': {'s': u'\u963f\u91cc', 'id': 5425}, u'dongying': {'s': u'\u4e1c\u8425', 'id': 3705}, u'gz': {'s': u'\u5e7f\u5dde', 'id': 4401}, u'gy': {'s': u'\u8d35\u9633', 'id': 5201}, u'qiannan': {'s': u'\u9ed4\u5357', 'id': 5227}, u'gl': {'s': u'\u6842\u6797', 'id': 4503}, u'huaihua': {'s': u'\u6000\u5316', 'id': 4312}, u'liupanshui': {'s': u'\u516d\u76d8\u6c34', 'id': 5202}, u'ty': {'s': u'\u592a\u539f', 'id': 1401}, u'suining': {'s': u'\u9042\u5b81', 'id': 5109}, u'anshun': {'s': u'\u5b89\u987a', 'id': 5204}, u'puyang': {'s': u'\u6fee\u9633', 'id': 4109}, u'yushu': {'s': u'\u7389\u6811', 'id': 6327}, u'huangshi': {'s': u'\u9ec4\u77f3', 'id': 4202}, u'hetian': {'s': u'\u548c\u7530', 'id': 6532}, u'jiyuan': {'s': u'\u6d4e\u6e90', 'id': 410881}, u'sanming': {'s': u'\u4e09\u660e', 'id': 3504}, u'yangzhou': {'s': u'\u626c\u5dde', 'id': 3210}, u'pingdingshan': {'s': u'\u5e73\u9876\u5c71', 'id': 4104}, u'ankang': {'s': u'\u5b89\u5eb7', 'id': 6109}, u'heihe': {'s': u'\u9ed1\u6cb3', 'id': 2311}, u'qujing': {'s': u'\u66f2\u9756', 'id': 5303}, u'tongliao': {'s': u'\u901a\u8fbd', 'id': 1505}, u'qiqihaer': {'s': u'\u9f50\u9f50\u54c8\u5c14', 'id': 2302}, u'zhuhai': {'s': u'\u73e0\u6d77', 'id': 4404}, u'zhangjiakou': {'s': u'\u5f20\u5bb6\u53e3', 'id': 1307}, u'zz': {'s': u'\u90d1\u5dde', 'id': 4101}, u'jilin': {'s': u'\u5409\u6797', 'id': 2202}, u'aba': {'s': u'\u963f\u575d', 'id': 4994}, u'ziyang': {'s': u'\u8d44\u9633', 'id': 5120}, u'xinzhou': {'s': u'\u5ffb\u5dde', 'id': 1409}, u'yingtan': {'s': u'\u9e70\u6f6d', 'id': 3606}, u'tianmen': {'s': u'\u5929\u95e8', 'id': 429006}, u'tongling': {'s': u'\u94dc\u9675', 'id': 3407}, u'xingtai': {'s': u'\u90a2\u53f0', 'id': 1305}, u'huaibei': {'s': u'\u6dee\u5317', 'id': 3406}, u'zhangzhou': {'s': u'\u6f33\u5dde', 'id': 3506}, u'tianshui': {'s': u'\u5929\u6c34', 'id': 6205}, u'quzhou': {'s': u'\u8862\u5dde', 'id': 3308}, u'changzhi': {'s': u'\u957f\u6cbb', 'id': 1404}, u'wuhai': {'s': u'\u4e4c\u6d77', 'id': 1503}, u'sjz': {'s': u'\u77f3\u5bb6\u5e84', 'id': 1301}, u'fuyang': {'s': u'\u961c\u9633', 'id': 3412}, u'eerduosi': {'s': u'\u9102\u5c14\u591a\u65af', 'id': 1506}, u'hulunbeier': {'s': u'\u547c\u4f26\u8d1d\u5c14', 'id': 1507}, u'shangrao': {'s': u'\u4e0a\u9976', 'id': 3611}, u'changzhou': {'s': u'\u5e38\u5dde', 'id': 3204}, u'hengshui': {'s': u'\u8861\u6c34', 'id': 1311}, u'luoyang': {'s': u'\u6d1b\u9633', 'id': 4103}, u'guangan': {'s': u'\u5e7f\u5b89', 'id': 5116}, u'sxyulin': {'s': u'\u6986\u6797', 'id': 6108}, u'hezhou': {'s': u'\u8d3a\u5dde', 'id': 4511}, u'langfang': {'s': u'\u5eca\u574a', 'id': 1310}, u'longnan': {'s': u'\u9647\u5357', 'id': 6212}, u'zunyi': {'s': u'\u9075\u4e49', 'id': 5203}, u'fuxin': {'s': u'\u961c\u65b0', 'id': 2109}, u'yibin': {'s': u'\u5b9c\u5bbe', 'id': 5115}, u'datong': {'s': u'\u5927\u540c', 'id': 1402}, u'dandong': {'s': u'\u4e39\u4e1c', 'id': 2106}, u'kelamayi': {'s': u'\u514b\u62c9\u739b\u4f9d', 'id': 6502}, u'anqing': {'s': u'\u5b89\u5e86', 'id': 3408}, u'pingliang': {'s': u'\u5e73\u51c9', 'id': 6208}, u'dingxi': {'s': u'\u5b9a\u897f', 'id': 6211}, u'tj': {'s': u'\u5929\u6d25', 'id': 1201}, u'kezilesu': {'s': u'\u514b\u5b5c\u52d2\u82cf', 'id': 6530}, u'shanwei': {'s': u'\u6c55\u5c3e', 'id': 4415}, u'jiaxing': {'s': u'\u5609\u5174', 'id': 3304}, u'guyuan': {'s': u'\u56fa\u539f', 'id': 6404}, u'kashi': {'s': u'\u5580\u4ec0', 'id': 6531}, u'lijiang': {'s': u'\u4e3d\u6c5f', 'id': 5307}, u'qitaihe': {'s': u'\u4e03\u53f0\u6cb3', 'id': 2309}, u'yichang': {'s': u'\u5b9c\u660c', 'id': 4205}, u'benxi': {'s': u'\u672c\u6eaa', 'id': 2105}, u'jieyang': {'s': u'\u63ed\u9633', 'id': 4452}, u'bazhong': {'s': u'\u5df4\u4e2d', 'id': 5119}, u'shihezi': {'s': u'\u77f3\u6cb3\u5b50', 'id': 659001}, u'aletai': {'s': u'\u963f\u52d2\u6cf0', 'id': 6543}, u'shiyan': {'s': u'\u5341\u5830', 'id': 4203}, u'dehong': {'s': u'\u5fb7\u5b8f', 'id': 5331}, u'fz': {'s': u'\u798f\u5dde', 'id': 3501}, u'bozhou': {'s': u'\u4eb3\u5dde', 'id': 3416}, u'sz': {'s': u'\u6df1\u5733', 'id': 4403}, u'sy': {'s': u'\u6c88\u9633', 'id': 2101}, u'su': {'s': u'\u82cf\u5dde', 'id': 3205}, u'nanchong': {'s': u'\u5357\u5145', 'id': 5113}, u'sh': {'s': u'\u4e0a\u6d77', 'id': 3101}, u'tangshan': {'s': u'\u5510\u5c71', 'id': 1302}, u'tongren': {'s': u'\u94dc\u4ec1', 'id': 5222}, u'guoluo': {'s': u'\u679c\u6d1b', 'id': 6326}, u'beihai': {'s': u'\u5317\u6d77', 'id': 4505}, u'zhoukou': {'s': u'\u5468\u53e3', 'id': 4116}, u'hengyang': {'s': u'\u8861\u9633', 'id': 4304}, u'songyuan': {'s': u'\u677e\u539f', 'id': 2207}, u'xn': {'s': u'\u897f\u5b81', 'id': 6301}, u'xinyang': {'s': u'\u4fe1\u9633', 'id': 4115}, u'lincang': {'s': u'\u4e34\u6ca7', 'id': 5309}, u'liaoyang': {'s': u'\u8fbd\u9633', 'id': 2110}, u'kaifeng': {'s': u'\u5f00\u5c01', 'id': 4102}, u'linyi': {'s': u'\u4e34\u6c82', 'id': 3713}, u'lz': {'s': u'\u5170\u5dde', 'id': 6201}, u'jiuquan': {'s': u'\u9152\u6cc9', 'id': 6209}, u'qinzhou': {'s': u'\u94a6\u5dde', 'id': 4507}, u'yc': {'s': u'\u94f6\u5ddd', 'id': 6401}, u'maoming': {'s': u'\u8302\u540d', 'id': 4409}, u'xiangyang': {'s': u'\u8944\u9633'}, u'huaian': {'s': u'\u6dee\u5b89', 'id': 3208}, u'fushun': {'s': u'\u629a\u987a', 'id': 2104}, u'yueyang': {'s': u'\u5cb3\u9633', 'id': 4306}, u'boertala': {'s': u'\u535a\u5c14\u5854\u62c9', 'id': 6527}, u'nujiang': {'s': u'\u6012\u6c5f', 'id': 5333}, u'sanya': {'s': u'\u4e09\u4e9a', 'id': 4602}, u'panjin': {'s': u'\u76d8\u9526', 'id': 2111}, u'linzhi': {'s': u'\u6797\u829d', 'id': 5426}, u'foshan': {'s': u'\u4f5b\u5c71', 'id': 4406}, u'shaoxing': {'s': u'\u7ecd\u5174', 'id': 3306}, u'yanan': {'s': u'\u5ef6\u5b89', 'id': 6106}, u'heyuan': {'s': u'\u6cb3\u6e90', 'id': 4416}, u'chongzuo': {'s': u'\u5d07\u5de6', 'id': 4514}, u'jingzhou': {'s': u'\u8346\u5dde', 'id': 4210}, u'ahsuzhou': {'s': u'\u5bbf\u5dde', 'id': 3413}, u'haibei': {'s': u'\u6d77\u5317', 'id': 6322}, u'heze': {'s': u'\u83cf\u6cfd', 'id': 3717}, u'changde': {'s': u'\u5e38\u5fb7', 'id': 4307}, u'chaohu': {'s': u'\u5de2\u6e56', 'id': 3414}, u'xilinguole': {'s': u'\u9521\u6797\u90ed\u52d2', 'id': 1525}, u'changdu': {'s': u'\u660c\u90fd', 'id': 5421}, u'baiyin': {'s': u'\u767d\u94f6', 'id': 6204}, u'dazhou': {'s': u'\u8fbe\u5dde', 'id': 5117}, u'nantong': {'s': u'\u5357\u901a', 'id': 3206}, u'tieling': {'s': u'\u94c1\u5cad', 'id': 2112}, u'rikaze': {'s': u'\u65e5\u5580\u5219', 'id': 5423}, u'zhumadian': {'s': u'\u9a7b\u9a6c\u5e97', 'id': 4117}, u'jstaizhou': {'s': u'\u6cf0\u5dde', 'id': 3212}, u'xj': {'s': u'\u4e4c\u9c81\u6728\u9f50', 'id': 6501}, u'weifang': {'s': u'\u6f4d\u574a', 'id': 3707}, u'binzhou': {'s': u'\u6ee8\u5dde', 'id': 3716}, u'xm': {'s': u'\u53a6\u95e8', 'id': 3502}, u'xa': {'s': u'\u897f\u5b89', 'id': 6101}, u'pingxiang': {'s': u'\u840d\u4e61', 'id': 3603}, u'xz': {'s': u'\u62c9\u8428', 'id': 5401}, u'zhuzhou': {'s': u'\u682a\u6d32', 'id': 4302}, u'wujiaqu': {'s': u'\u4e94\u5bb6\u6e20', 'id': 659004}, u'zhenjiang': {'s': u'\u9547\u6c5f', 'id': 3211}, u'lianyungang': {'s': u'\u8fde\u4e91\u6e2f', 'id': 3207}, u'guigang': {'s': u'\u8d35\u6e2f', 'id': 4508}, u'zhongshan': {'s': u'\u4e2d\u5c71', 'id': 4420}, u'lvliang': {'s': u'\u5415\u6881', 'id': 1411}, u'wuzhong': {'s': u'\u5434\u5fe0', 'id': 6403}, u'jinchang': {'s': u'\u91d1\u660c', 'id': 6203}, u'ganzi': {'s': u'\u7518\u5b5c', 'id': 5133}, u'hljyichun': {'s': u'\u4f0a\u6625', 'id': 2307}, u'jingdezhen': {'s': u'\u666f\u5fb7\u9547', 'id': 3602}, u'laiwu': {'s': u'\u83b1\u829c', 'id': 3712}, u'naqu': {'s': u'\u90a3\u66f2', 'id': 5424}, u'chenzhou': {'s': u'\u90f4\u5dde', 'id': 4310}, u'jinhua': {'s': u'\u91d1\u534e', 'id': 3307}, u'anyang': {'s': u'\u5b89\u9633', 'id': 4105}, u'zjtaizhou': {'s': u'\u53f0\u5dde', 'id': 3310}, u'yingkou': {'s': u'\u8425\u53e3', 'id': 2108}, u'panzhihua': {'s': u'\u6500\u679d\u82b1', 'id': 5104}, u'mianyang': {'s': u'\u7ef5\u9633', 'id': 5107}, u'yangquan': {'s': u'\u9633\u6cc9', 'id': 1403}, u'baise': {'s': u'\u767e\u8272', 'id': 4510}, u'qiandongnan': {'s': u'\u9ed4\u4e1c\u5357', 'id': 5226}, u'yantai': {'s': u'\u70df\u53f0', 'id': 3706}, u'quanzhou': {'s': u'\u6cc9\u5dde', 'id': 3505}, u'suihua': {'s': u'\u7ee5\u5316', 'id': 2312}, u'huludao': {'s': u'\u846b\u82a6\u5c9b', 'id': 2114}, u'chifeng': {'s': u'\u8d64\u5cf0', 'id': 1504}, u'yunfu': {'s': u'\u4e91\u6d6e', 'id': 4453}, u'chuzhou': {'s': u'\u6ec1\u5dde', 'id': 3411}, u'km': {'s': u'\u6606\u660e', 'id': 5301}, u'longyan': {'s': u'\u9f99\u5ca9', 'id': 3508}, u'hn': {'s': u'\u6d77\u53e3', 'id': 4601}, u'xinxiang': {'s': u'\u65b0\u4e61', 'id': 4107}, u'guangyuan': {'s': u'\u5e7f\u5143', 'id': 5108}, u'shaoguan': {'s': u'\u97f6\u5173', 'id': 4402}, u'kuerle': {'s': u'\u5e93\u5c14\u52d2', 'id': 5609}, u'enshi': {'s': u'\u6069\u65bd', 'id': 4228}, u'huainan': {'s': u'\u6dee\u5357', 'id': 3404}, u'shannan': {'s': u'\u5c71\u5357', 'id': 5422}, u'huanggang': {'s': u'\u9ec4\u5188', 'id': 4211}, u'gxyulin': {'s': u'\u7389\u6797', 'id': 4509}, u'wuhu': {'s': u'\u829c\u6e56', 'id': 3402}, u'siping': {'s': u'\u56db\u5e73', 'id': 2203}, u'hrb': {'s': u'\u54c8\u5c14\u6ee8', 'id': 2301}, u'dl': {'s': u'\u5927\u8fde', 'id': 2102}, u'dg': {'s': u'\u4e1c\u839e', 'id': 4419}, u'deyang': {'s': u'\u5fb7\u9633', 'id': 5106}, u'jiamusi': {'s': u'\u4f73\u6728\u65af', 'id': 2308}, u'diqing': {'s': u'\u8fea\u5e86', 'id': 5334}, u'jincheng': {'s': u'\u664b\u57ce', 'id': 1405}, u'jiaozuo': {'s': u'\u7126\u4f5c', 'id': 4108}, u'alaer': {'s': u'\u963f\u62c9\u5c14', 'id': 659002}, u'qd': {'s': u'\u9752\u5c9b', 'id': 3702}, u'jxfuzhou': {'s': u'\u629a\u5dde', 'id': 3610}, u'huizhou': {'s': u'\u60e0\u5dde', 'id': 4413}, u'zhangjiajie': {'s': u'\u5f20\u5bb6\u754c', 'id': 4308}, u'bengbu': {'s': u'\u868c\u57e0', 'id': 3403}, u'shuozhou': {'s': u'\u6714\u5dde', 'id': 1406}, u'tulufan': {'s': u'\u5410\u9c81\u756a', 'id': 6521}, u'xingan': {'s': u'\u5174\u5b89', 'id': 1522}, u'honghe': {'s': u'\u7ea2\u6cb3', 'id': 5325}, u'hebi': {'s': u'\u9e64\u58c1', 'id': 4106}, u'qinhuangdao': {'s': u'\u79e6\u7687\u5c9b', 'id': 1303}, u'jiangmen': {'s': u'\u6c5f\u95e8', 'id': 4407}, u'wh': {'s': u'\u6b66\u6c49', 'id': 4201}, u'jn': {'s': u'\u6d4e\u5357', 'id': 3701}, u'leshan': {'s': u'\u4e50\u5c71', 'id': 5111}, u'xuzhou': {'s': u'\u5f90\u5dde', 'id': 3203}, u'jiayuguan': {'s': u'\u5609\u5cea\u5173', 'id': 6202}, u'wx': {'s': u'\u65e0\u9521', 'id': 3202}, u'bayinguoleng': {'s': u'\u5df4\u97f3\u90ed\u695e', 'id': 6528}, u'cangzhou': {'s': u'\u6ca7\u5dde', 'id': 1309}, u'nanping': {'s': u'\u5357\u5e73', 'id': 3507}, u'zigong': {'s': u'\u81ea\u8d21', 'id': 5103}, u'xianning': {'s': u'\u54b8\u5b81', 'id': 4212}, u'changji': {'s': u'\u660c\u5409', 'id': 6523}, u'cc': {'s': u'\u957f\u6625', 'id': 2201}, u'meishan': {'s': u'\u7709\u5c71', 'id': 5114}, u'cd': {'s': u'\u6210\u90fd', 'id': 5101}, u'qingyuan': {'s': u'\u6e05\u8fdc', 'id': 4418}, u'wuzhou': {'s': u'\u68a7\u5dde', 'id': 4504}, u'zaozhuang': {'s': u'\u67a3\u5e84', 'id': 3704}, u'yaan': {'s': u'\u96c5\u5b89', 'id': 5118}, 'cs': {'s': u'\u957f\u6c99', 'id': 4301}, u'taian': {'s': u'\u6cf0\u5b89', 'id': 3709}, u'zhangye': {'s': u'\u5f20\u6396', 'id': 6207}, u'tongchuan': {'s': u'\u94dc\u5ddd', 'id': 6102}, u'yanbian': {'s': u'\u5ef6\u8fb9', 'id': 2224}, u'shantou': {'s': u'\u6c55\u5934', 'id': 4405}, u'qianxinan': {'s': u'\u9ed4\u897f\u5357', 'id': 5223}, u'yuxi': {'s': u'\u7389\u6eaa', 'id': 5304}, u'wenshan': {'s': u'\u6587\u5c71', 'id': 5326}, u'tacheng': {'s': u'\u5854\u57ce', 'id': 6542}, u'wuwei': {'s': u'\u6b66\u5a01', 'id': 6206}, u'baoshan': {'s': u'\u4fdd\u5c71', 'id': 5305}, u'shennongjia': {'s': u'\u795e\u519c\u67b6', 'id': 429021}, u'jinzhong': {'s': u'\u664b\u4e2d', 'id': 1407}, u'baishan': {'s': u'\u767d\u5c71', 'id': 2206}, u'hechi': {'s': u'\u6cb3\u6c60', 'id': 4512}, u'baicheng': {'s': u'\u767d\u57ce', 'id': 2208}, u'akesu': {'s': u'\u963f\u514b\u82cf', 'id': 6529}, u'baoding': {'s': u'\u4fdd\u5b9a', 'id': 1306}, u'laibin': {'s': u'\u6765\u5bbe', 'id': 4513}, u'huzhou': {'s': u'\u6e56\u5dde', 'id': 3305}, u'qianjiang': {'s': u'\u6f5c\u6c5f', 'id': 429005}, u'yancheng': {'s': u'\u76d0\u57ce', 'id': 3209}, u'baotou': {'s': u'\u5305\u5934', 'id': 1502}, u'yangjiang': {'s': u'\u9633\u6c5f', 'id': 4417}, u'wuzhishan': {'s': u'\u4e94\u6307\u5c71', 'id': 469001}, u'liangshan': {'s': u'\u51c9\u5c71', 'id': 5134}, u'jinzhou': {'s': u'\u9526\u5dde', 'id': 2107}, u'hanzhong': {'s': u'\u6c49\u4e2d', 'id': 6107}, u'liaocheng': {'s': u'\u804a\u57ce', 'id': 3715}, u'daqing': {'s': u'\u5927\u5e86', 'id': 2306}, u'putian': {'s': u'\u8386\u7530', 'id': 3503}, u'anshan': {'s': u'\u978d\u5c71', 'id': 2103}, u'nc': {'s': u'\u5357\u660c', 'id': 3601}, u'qingyang': {'s': u'\u5e86\u9633', 'id': 6210}, u'yuncheng': {'s': u'\u8fd0\u57ce', 'id': 1408}, u'ganzhou': {'s': u'\u8d63\u5dde', 'id': 3607}, u'zhongwei': {'s': u'\u4e2d\u536b', 'id': 6405}, u'haidong': {'s': u'\u6d77\u4e1c', 'id': 6321}, u'suizhou': {'s': u'\u968f\u5dde', 'id': 4213}, u'xuancheng': {'s': u'\u5ba3\u57ce', 'id': 3418}, u'wei': {'s': u'\u5a01\u6d77', 'id': 3710}, u'rizhao': {'s': u'\u65e5\u7167', 'id': 3711}, u'shaoyang': {'s': u'\u90b5\u9633', 'id': 4305}, u'xiangtan': {'s': u'\u6e58\u6f6d', 'id': 4303}, u'liaoyuan': {'s': u'\u8fbd\u6e90', 'id': 2204}, u'neijiang': {'s': u'\u5185\u6c5f', 'id': 5110}, u'xiantao': {'s': u'\u4ed9\u6843', 'id': 429004}, u'shangqiu': {'s': u'\u5546\u4e18', 'id': 4114}, u'ezhou': {'s': u'\u9102\u5dde', 'id': 4207}, u'zhanjiang': {'s': u'\u6e5b\u6c5f', 'id': 4408}, u'linfen': {'s': u'\u4e34\u6c7e', 'id': 1410}, u'loudi': {'s': u'\u5a04\u5e95', 'id': 4313}, u'yongzhou': {'s': u'\u6c38\u5dde', 'id': 4311}, u'chaozhou': {'s': u'\u6f6e\u5dde', 'id': 4451}, u'maanshan': {'s': u'\u9a6c\u978d\u5c71', 'id': 3405}, u'cq': {'s': u'\u91cd\u5e86', 'id': 5001}, u'shuangyashan': {'s': u'\u53cc\u9e2d\u5c71', 'id': 2305}, u'mudanjiang': {'s': u'\u7261\u4e39\u6c5f', 'id': 2310}, u'jining': {'s': u'\u6d4e\u5b81', 'id': 3708}, u'puer': {'s': u'\u666e\u6d31', 'id': 5174}, u'xuchang': {'s': u'\u8bb8\u660c', 'id': 4110}, u'jiujiang': {'s': u'\u4e5d\u6c5f', 'id': 3604}, u'linxia': {'s': u'\u4e34\u590f', 'id': 6229}, u'bj': {'s': u'\u5317\u4eac', 'id': 1101}, u'nmg': {'s': u'\u547c\u548c\u6d69\u7279', 'id': 1501}, u'gannan': {'s': u'\u7518\u5357', 'id': 6230}, u'lishui': {'s': u'\u4e3d\u6c34', 'id': 3311}, u'hegang': {'s': u'\u9e64\u5c97', 'id': 2304}, u'suqian': {'s': u'\u5bbf\u8fc1', 'id': 3213}, u'bayannaoer': {'s': u'\u5df4\u5f66\u6dd6\u5c14', 'id': 1508}, u'ningde': {'s': u'\u5b81\u5fb7', 'id': 3509}, u'luan': {'s': u'\u516d\u5b89', 'id': 3415}, u'shangluo': {'s': u'\u5546\u6d1b', 'id': 6110}, u'jian': {'s': u'\u5409\u5b89', 'id': 3608}, u'meizhou': {'s': u'\u6885\u5dde', 'id': 4414}, u'zibo': {'s': u'\u6dc4\u535a', 'id': 3703}, u'wulanchabu': {'s': u'\u4e4c\u5170\u5bdf\u5e03', 'id': 1509}, u'chengde': {'s': u'\u627f\u5fb7', 'id': 1308}, u'tonghua': {'s': u'\u901a\u5316', 'id': 2205}, u'daxinganling': {'s': u'\u5927\u5174\u5b89\u5cad', 'id': 2327}, u'haixi': {'s': u'\u6d77\u897f', 'id': 6328}, u'shizuishan': {'s': u'\u77f3\u5634\u5c71', 'id': 6402}, u'hz': {'s': u'\u676d\u5dde', 'id': 3301}, u'yiyang': {'s': u'\u76ca\u9633', 'id': 4309}, u'zhaotong': {'s': u'\u662d\u901a', 'id': 5306}, u'xiaogan': {'s': u'\u5b5d\u611f', 'id': 4209}, u'xianyang': {'s': u'\u54b8\u9633', 'id': 6104}, u'dezhou': {'s': u'\u5fb7\u5dde', 'id': 3714}, u'hf': {'s': u'\u5408\u80a5', 'id': 3401}, u'xishuangbanna': {'s': u'\u897f\u53cc\u7248\u7eb3', 'id': 5328}, u'wenzhou': {'s': u'\u6e29\u5dde', 'id': 3303}, u'zhoushan': {'s': u'\u821f\u5c71', 'id': 3309}, u'luzhou': {'s': u'\u6cf8\u5dde', 'id': 5105}, u'weinan': {'s': u'\u6e2d\u5357', 'id': 6105}, u'liuzhou': {'s': u'\u67f3\u5dde', 'id': 4502}, u'luohe': {'s': u'\u6f2f\u6cb3', 'id': 4111}, u'chizhou': {'s': u'\u6c60\u5dde', 'id': 3417}, u'jingmen': {'s': u'\u8346\u95e8', 'id': 4208}, u'huangnan': {'s': u'\u9ec4\u5357', 'id': 6323}, u'sanmenxia': {'s': u'\u4e09\u95e8\u5ce1', 'id': 4112}, u'chuxiong': {'s': u'\u695a\u96c4', 'id': 5323}, u'yili': {'s': u'\u4f0a\u7281', 'id': 6540}, u'bijie': {'s': u'\u6bd5\u8282', 'id': 5224}, u'jixi': {'s': u'\u9e21\u897f', 'id': 2303}, u'alashan': {'s': u'\u963f\u62c9\u5584', 'id': 1529}, u'hami': {'s': u'\u54c8\u5bc6', 'id': 6522}, u'nj': {'s': u'\u5357\u4eac', 'id': 3201}, u'nn': {'s': u'\u5357\u5b81', 'id': 4501}, u'nb': {'s': u'\u5b81\u6ce2', 'id': 3302}, u'nanyang': {'s': u'\u5357\u9633', 'id': 4113}, u'tumushuke': {'s': u'\u56fe\u6728\u8212\u514b', 'id': 659003}, u'fangchenggang': {'s': u'\u9632\u57ce\u6e2f', 'id': 4506}, u'zhaoqing': {'s': u'\u8087\u5e86', 'id': 4412}, u'xinyu': {'s': u'\u65b0\u4f59', 'id': 3605}, u'dali': {'s': u'\u5927\u7406', 'id': 5329}, u'baoji': {'s': u'\u5b9d\u9e21', 'id': 6103}, u'xiangxi': {'s': u'\u6e58\u897f'}, u'huangshan': {'s': u'\u9ec4\u5c71', 'id': 3410}, u'handan': {'s': u'\u90af\u90f8', 'id': 1304}, u'jxyichun': {'s': u'\u5b9c\u6625', 'id': 3609}}


class DidaSpider(CrawlSpider):
    name = "ganji"
    allowed_domains = ["ganji.com"]
    start_urls = [
        "http://www.ganji.com/index.htm"
    ]
    rules = (
        Rule(SgmlLinkExtractor(allow=('.*\.ganji\.com\/$', )), callback='get_site'),
    )
    download_delay = 10
    con = pg.connect(dbname='merry', host='localhost', user='dida', passwd='loveu')

    def get_site(self, response):
        site = response.url + "pincheshangxiaban/"
        yield Request(site, callback=self.parse_site)
        
    def parse_site(self, response):
        for link in SgmlLinkExtractor(allow='.*pincheshangxiaban\/[0-9]+x\.htm$').extract_links(response):
            ori = 2
            oid = re.compile('.*pincheshangxiaban\/([0-9]+)x.htm').match(link.url).group(1)

            r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
            if (r.ntuples() > 0): print oid + "exist already!"
            else: yield Request(link.url, callback=self.parse_node)

        for link in SgmlLinkExtractor(allow='.*pincheshangxiaban\/o[0-9]+\/$').extract_links(response):
            yield Request(link.url, callback=self.parse_site_child)
            

    def parse_site_child(self, response):
        for link in SgmlLinkExtractor(allow='.*pincheshangxiaban\/[0-9]+x\.htm$').extract_links(response):
            ori = 2
            oid = re.compile('.*pincheshangxiaban\/([0-9]+)x.htm').match(link.url).group(1)

            r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
            if (r.ntuples() > 0): print oid + "exist already!"
            else: yield Request(link.url, callback=self.parse_node)

    def parse_node(self, response):
        self.log('this is node page %s' % response.url)

        hxs = HtmlXPathSelector(response)

        item = DidaItem()

        item['saddr'] = ''
        item['eaddr'] = ''
        item['repeat'] = 0
        
        ori = 2
        oid = re.compile('.*pincheshangxiaban\/([0-9]+)x.htm').match(response.url).group(1)

        if hxs.select('//ul[@class="detail-sub"]/li[1]/text()').extract()[0].find(u'有车'):
            item['dad'] = 1
        else:
            item['dad'] = 0
            
        stmp = hxs.select('//ul[@class="detail-sub"]/li[2]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")
        ltmp = stmp.split()
        stmp2 = hxs.select('//ul[@class="detail-sub"]/li[3]/text()').extract()[0].strip().replace(u"\xa0", u" ").replace("\n", " ")

        item['attach'] = ''.join(hxs.select('//ul[@class="detail-sub"]/li[5]/text() | //ul[@class="detail-sub"]/li[5]//*/text()').extract()).strip()
        if len(item['attach']) <= 0:
            item['attach'] = ''.join(hxs.select('//ul[@class="detail-sub"]/li[4]/text() | //ul[@class="detail-sub"]/li[4]//*/text()').extract()).strip()
            if len(item['attach']) <= 0:
                print "**********" + oid + "attach error"
                return item
        item['attach'] = item['attach'].replace(u'联系我时，请说明是在赶集网看到的，谢谢！', u'')

        if len(ltmp) < 3:
            print "**********" + oid + "li[2] format error"
            return item

        item['saddr'] = ltmp[0]
        item['eaddr'] = stmp2

        if re.compile('^.*-[0-9]+$').match(ltmp[1]):
            item['repeat'] = 0
            item['sdate'] = ltmp[1]
            item['stime'] = '08:00:00'
        elif re.compile('^.*:[0-9]+$').match(ltmp[1]):
            if item['attach'].find(u'每天'):
                item['repeat'] = 1
                item['sdate'] = ' '
                item['stime'] = ltmp[1]
            elif item['attach'].find(u'上下班'):
                item['repeat'] = 2
                item['sdate'] = '1,2,3,4,5'
                item['stime'] = ltmp[1]
            elif item['attach'].find(u'周末'):
                item['repeat'] = 2
                item['sdate'] = '6,7'
                item['stime'] = ltmp[1]
            else:
                item['repeat'] = 0
                item['sdate'] = ltmp[1]
                item['stime'] = '08:00:00'
        else:
            print "**********" + oid + "sdate, stime format error"
            return item

        item['phone'] = ''
        try: item['phone'] = 'http://www.ganji.com/' + hxs.select('//div[@class="ct-manner"]/p[2]/img/@src').extract()[0]
        except: pass
        
        item['nick'] = ''
        try: item['nick'] = ''.join(hxs.select('//div[@class="ct-manner"]/p/text()').extract()).replace(" ", "").replace('\n', '').replace(u'\xa0', u'')
        except: pass

        item['city'] = 999999
        try:
            p = re.compile('http://(.*)\.ganji\.com.*')
            item['city'] = cities[p.match(response.url).group(1)]['id']
        except: pass

        r = self.con.query("SELECT id FROM plan WHERE ori=" +`ori`+ " AND oid='" +oid+ "'")
        if (r.ntuples() > 0): print oid + "exist already!"
        else:
            sql = "INSERT INTO plan (ori, oid, dad, saddr, eaddr, repeat, sdate, stime, attach, phone, nick, cityid, statu, ourl) VALUES ("
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
            sql += "'"+item['nick']+"'" + ", "
            
            sql += `item['city']` + ", "
            sql += `10` + ", "

            sql += "'" +response.url+ "'"

            sql += ")"
            
            #print sql
            self.con.query(sql.encode('utf-8'))

        return item
