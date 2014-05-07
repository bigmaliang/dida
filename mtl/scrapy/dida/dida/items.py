# Define here the models for your scraped items
#
# See documentation in:
# http://doc.scrapy.org/topics/items.html

from scrapy.item import Item, Field

class DidaItem(Item):
    # define the fields for your item here like:
    # name = Field()
    nick = Field()
    
    city = Field()
    phone = Field()
    contact = Field()
    size = Field()
    mode = Field()

    dad = Field()
    repeat = Field()
    saddr = Field()
    eaddr = Field()
    sdate = Field()
    stime = Field()

    attach = Field()
    nick = Field()
    seat = Field()
    fee = Field()
    marks = Field()
