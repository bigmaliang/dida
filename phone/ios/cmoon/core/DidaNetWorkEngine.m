//
//  DidaNetWorkEngine.m
//  cmoon
//
//  Created by li yajie on 12-7-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "DidaNetWorkEngine.h"

@implementation DidaNetWorkEngine

/**
 *Accept:text/html,application/xhtml+xml,application/xml;q=0.9,*\*;q=0.8
* Accept-Charset:UTF-8,*;q=0.5
Accept-Encoding:gzip,deflate,sdch
Accept-Language:en-US,en;q=0.8
Cache-Control:max-age=0
Connection:keep-alive
 */
-(void) sendServerRequest:(NSString*) path withParam:(NSMutableDictionary*) params userMethod:(NSString*) method {
    MKNetworkOperation * op = [self operationWithPath:path params:nil httpMethod:method ]; 
    UIWindow * window = [UIApplication sharedApplication].keyWindow;
    MBProgressHUD *hub = [[MBProgressHUD alloc] initWithWindow:window];
    [hub setMode:MBProgressHUDModeIndeterminate];
    hub.frame = CGRectMake(0, 0, 40.0, 40.0);
    hub.center = window.center;
    hub.taskInProgress = YES;
    [window addSubview:hub];
    [hub show:YES];
    [op onCompletion:^(MKNetworkOperation *completedOperation) {
        [hub hide:YES];
        [self parseJsonData:completedOperation]; 
    } onError:^(NSError *error) {
         [hub hide:YES];
        [self alertError:error];
    }];
    [hub release];
    [self enqueueOperation:op];
}
/**
 * @param 解析json数据
 */
-(void) parseJsonData:(MKNetworkOperation*) operation {
    if ([operation isCachedResponse]) {
        //cached data
        NSString *jonData = [[operation responseString] JSONString];//直接清除原理数据
        NSLog(@"Cache %@",jonData);
        [operation cancel];
    } else {
        //if (cached == NO) {//追加数据
            //fresh data
            NSString *jonData = [[operation responseString] JSONString];
            NSLog(@"Server %@",jonData);
       // }
    }
}

/**
 * 将错误消息发送出去
 * @param error 
 */
-(void) alertError:(NSError*) error {
//    [error.description
    int errorCode = error.code;
    NSLog(@"get the error code = %d",errorCode);
    switch (errorCode) {
//        case Mk:
//            <#statements#>
//            break;
            
        default:
            break;
    }
    [[NSNotificationCenter defaultCenter] postNotificationName:@"networkrequesterror" object:error.description  userInfo:nil];
}

@end
