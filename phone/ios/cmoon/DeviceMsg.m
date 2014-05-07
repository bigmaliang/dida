//
//  DeviceMsg.m
//  cmoon
//
//  Created by li yajie on 12-7-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "DeviceMsg.h"
#import <sys/utsname.h>

static DeviceMsg * instance = nil;

@implementation DeviceMsg

@synthesize deviceMsg;

+(DeviceMsg*) getDeviceMsgInstance {
    @synchronized(instance) {
        if (instance == nil) {
            instance = [[DeviceMsg alloc]init];
            DeviceInfo * info = [[DeviceInfo alloc]init];
            NSDictionary * bundle = [[NSBundle mainBundle]infoDictionary];
            instance.deviceMsg = info;
            instance.deviceMsg.bundleVersion = [bundle valueForKey:(NSString *)kCFBundleVersionKey];
            NSString * bundleID = [bundle valueForKey:(NSString *)kCFBundleIdentifierKey];
            info.bundleId = bundleID;
            struct utsname systeminfo;
            uname(&systeminfo);
            NSString * str = [NSString stringWithCString:systeminfo.machine encoding:NSUTF8StringEncoding];
            if ([str isEqualToString:@"iPhone1,1"]) {
                info.bundleId = @"iPhone 1G";
            } else if ([str isEqualToString:@"iPhone1,2"]) {
                info.bundleId = @"iPhone 3G";
            } else if ([str isEqualToString:@"iPhone3,1"] || [str isEqualToString:@"iPhone3,2"] || [str isEqualToString:@"iPhone3,3"]) {
                info.bundleId = @"iPhone 4";
            } else if ([str isEqualToString:@"iPhone4,1"]) {
                info.bundleId = @"iPhone 4S";
            } else if ([str isEqualToString:@"iPod3,1"] || [str isEqualToString:@"iPod4,1"]) {
                info.bundleId = @"iPod touch";
            } else if ([str isEqualToString:@"iPad1,1"] || [str isEqualToString:@"iPad2,1"] || [str isEqualToString:@"iPad2,2"] || [str isEqualToString:@"iPad2,3"] || [str isEqualToString:@"3,1"] || [str isEqualToString:@"iPad3,2"] ){
                info.bundleId = @"iPad";
            } else if ([str isEqualToString:@"i386, x86_64"]) {
                info.bundleId = @"iPhone Simulator";
            } else {
                info.bundleId = [[UIDevice currentDevice] model];
            }
            
           info.systemVersion = [NSString stringWithFormat:@"Ios %@",[[UIDevice currentDevice] systemVersion] ];
            [info release];
        }
    }
    return instance;
}
+(id)allocWithZone:(NSZone *)zone {
    return instance;
}

+(id)copyWithZone:(NSZone *)zone {
    return instance;
}

-(id)autorelease {
    return instance;
}

@end



#pragma implement the device information object
@interface DeviceInfo()
@end
@implementation DeviceInfo

@synthesize bundleVersion;
@synthesize bundleId;
@synthesize phoneType;
@synthesize systemVersion;

- (void)dealloc
{
    bundleId = nil;
    bundleVersion = nil;
    phoneType = nil;
    systemVersion = nil;
//    [super dealloc];
}
@end


