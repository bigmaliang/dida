//
//  IpInfo.m
//  cmoon
//
//  Created by li yajie on 12-5-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "IpInfo.h"

@implementation IpInfo

@synthesize ip;
@synthesize type;

-(void)setProvince:(Province *)province {
    if (province) {
        return;
    }
self.province = province;
}

-(Province*) province {
    return self.province;
}
@end
