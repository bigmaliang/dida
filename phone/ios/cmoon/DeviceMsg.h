//
//  DeviceMsg.h
//  cmoon
//
//  Created by li yajie on 12-7-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ArcHelper.h"


@interface DeviceInfo : NSObject

@property(nonatomic,assign) NSString* bundleVersion;

@property(nonatomic,assign) NSString* bundleId;

@property(nonatomic,assign) NSString* phoneType;

@property(nonatomic,assign) NSString* systemVersion;
@end


@interface DeviceMsg : NSObject

@property(nonatomic,retain) DeviceInfo* deviceMsg;

@end



