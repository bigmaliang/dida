//
//  IpInfo.h
//  cmoon
//
//  Created by li yajie on 12-5-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Province.h"


@interface IpInfo : NSObject

@property(nonatomic,retain) NSString *ip;
@property(nonatomic,retain) NSString *type;

@property(nonatomic,retain) Province *province;


-(void) setProvince:(Province *)province;

-(Province*) province;
@end
