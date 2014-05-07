//
//  DidaAppDelegate.h
//  cmoon
//
//  Created by li yajie on 12-5-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreLocation/CoreLocation.h>
#import "DidaNetWorkEngine.h"
#import "CommomDefine.h"
#import "ArcHelper.h"
#import "BoreController.h"
#import "AboutController.h"

@interface DidaAppDelegate : UIResponder <UIApplicationDelegate,UITabBarControllerDelegate>



@property (retain, nonatomic) UIWindow *window;

@property (retain,nonatomic) DidaNetWorkEngine * engine;

@property (nonatomic,retain) UITabBarController *tabeViewController;

@end
