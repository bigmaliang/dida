//
//  DidaNetWorkEngine.h
//  cmoon
//
//  Created by li yajie on 12-7-13.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Archelper.h"
#import "MKNetworkEngine.h"
#import "CommomDefine.h"
#import "JSONKit.h"
#import "MBProgressHUD.h"

@interface DidaNetWorkEngine : MKNetworkEngine

{
   
}

/**
 *发送信息到服务端
 * @param params 相关参数
 */
-(void) sendServerRequest:(NSString*) path withParam:(NSMutableDictionary*) params userMethod:(NSString*) method;


//typedef void (^completeJsonParse)(MKNetworkOperation*);


@end
