# -*- coding: utf-8 -*-

from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor
from scrapy.selector import HtmlXPathSelector
from scrapy.item import Item
from dida.items import DidaItem
from scrapy.http import Request
import re

cities = {'changsha': '长沙'}

class DidaSpider(CrawlSpider):
    name = "diredeng"
    allowed_domains = ["bigml.com"]
    start_urls = [
        "http://www.bigml.com/map.html"
    ]
    download_delay = 10

    def parse(self, response):

        hxs = HtmlXPathSelector(response)

        cs = hxs.select('//div[@class="ed_map"]//dd/a | //div[@class="ed_map"]//dt/a')
        p = re.compile('http://(.*)\.edeng\.cn\/$')

        for c in cs:
            if p.match(c.select('@href').extract()[0]):
                href = p.match(c.select('@href').extract()[0]).group(1)
                name = c.select('text()').extract()[0]
                cities[href] = name

        print cities











