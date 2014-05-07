# -*- coding: utf-8 -*-

from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.contrib.linkextractors.sgml import SgmlLinkExtractor
from scrapy.selector import HtmlXPathSelector
from scrapy.item import Item
from dida.items import DidaItem
from scrapy.http import Request
import re

cities = {'cs': '长沙'}

class DidaSpider(CrawlSpider):
    name = "dirganji"
    allowed_domains = ["ganji.com"]
    start_urls = [
        "http://www.ganji.com/index.htm"
    ]
    download_delay = 10

    def parse(self, response):

        hxs = HtmlXPathSelector(response)

        cs = hxs.select('//div[@class="all-city"]//dd/a')
        p = re.compile('http://(.*)\.ganji\.com.*')

        for c in cs:
            href = p.match(c.select('@href').extract()[0]).group(1)
            name = c.select('text()').extract()[0]

            cities[href] = name

        print cities











