//
//  AboutControllerViewController.h
//  cmoon
//
//  Created by li yajie on 12-7-25.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "BaseController.h"
#import "MBProgressHUD.h"

@interface AboutController : BaseController<UIWebViewDelegate>
{
    UIWebView * mWebview;
    MBProgressHUD *progress;
}
@end
