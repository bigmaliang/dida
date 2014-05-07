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
    name = "dir58"
    allowed_domains = ["58.com"]
    start_urls = [
        "http://www.58.com/pinche/changecity/"
    ]
    download_delay = 10

    def parse(self, response):

        hxs = HtmlXPathSelector(response)

        cs = hxs.select('//dl[@id="clist"]/dd/a')
        p = re.compile('http://(.*)\.58\.com.*')

        for c in cs:
            href = p.match(c.select('@href').extract()[0]).group(1)
            name = c.select('text()').extract()[0]

            cities[href] = name

        print cities











