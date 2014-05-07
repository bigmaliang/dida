//
//  DidaAppDelegate.m
//  cmoon
//
//  Created by li yajie on 12-5-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "DidaAppDelegate.h"
#import "DidaNetWorkEngine.h"
#import "DidaViewController.h"
#import "HomeMapController.h"

@implementation DidaAppDelegate

@synthesize window = _window;
@synthesize engine = _engine;


@synthesize tabeViewController;

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
    _window = [[UIWindow alloc]initWithFrame:[UIScreen mainScreen].bounds];
    [_window setBackgroundColor:[UIColor whiteColor]];
    //initial the networkengine
    [self initNetworkEngine];
    [_engine setReachabilityChangedHandler:networkChangedListener];
    //initial the location manager
        
    // Override point for customization after application launch.
    DidaViewController *controller = [[DidaViewController alloc ]init];
    
    tabeViewController = [[UITabBarController alloc] init];

    tabeViewController.delegate = self;
    NSMutableArray * childControllers = [[NSMutableArray alloc]initWithCapacity:2];
    UITabBarItem *secondTab = [[UITabBarItem alloc] initWithTabBarSystemItem:UITabBarSystemItemFeatured tag:1];
    secondTab.title = @"看图";
    controller.tabBarItem = secondTab;
    [secondTab release];
    [childControllers addObject:controller];
    [controller release];
    
    HomeMapController * mapController = [[HomeMapController alloc] init];
    
    UITabBarItem * firstItem = [[UITabBarItem alloc]initWithTitle:@"拼车" image:[UIImage imageNamed:@"earth.png"] tag:2];
    mapController.tabBarItem = firstItem;
    [firstItem release];
    
    [childControllers addObject:mapController];
    [mapController release];

    
    BoreController * thirdController = [[BoreController alloc]init];
    UITabBarItem* thirdTab = [[UITabBarItem alloc]initWithTabBarSystemItem:UITabBarSystemItemFeatured tag:3];
    thirdTab.title = @"随便看看";
    thirdController.tabBarItem = thirdTab;
    [thirdTab release];
    [childControllers addObject:thirdController];
    [thirdController release];
    
    AboutController * about = [[AboutController alloc]init];
    UITabBarItem *fourth = [[UITabBarItem alloc]initWithTitle:@"关于" image:[UIImage imageNamed:@"about.png"] tag:4];
    about.tabBarItem = fourth;
    [fourth release];
    [childControllers addObject:about];
    [about release];
    tabeViewController.viewControllers = childControllers;
    [tabeViewController setSelectedViewController:mapController];
    _window.rootViewController = tabeViewController;
    [_window makeKeyAndVisible];
    [childControllers release];
    return YES;
}
/**
 * 网络变化的代码块
 * 会有问题
 */
void (^networkChangedListener)(NetworkStatus netstatus) = ^(NetworkStatus nstatus) {
        switch(nstatus) {
            case NotReachable://网络不可用
        {
            //显示一个不可用的视图
        }
            break;
            case ReachableViaWiFi:
                break;
            case ReachableViaWWAN://手机网络
                break;
                default:
                break;
    }
};

/**
 * 初始化网络引擎
 */
-(void) initNetworkEngine {
    @try {
        NSMutableDictionary *headerFields = [NSMutableDictionary dictionary];
        [headerFields setValue:@"UTF-8,*;q=0.5" forKey:@"Accept-Charset"];
        [headerFields setValue:@"text/html,application/xhtml+xml,application/xml,application/json;q=0.9,*'\'*;q=0.8" forKey:@"Accept"];
        [headerFields setValue:@"IOS" forKey:@"User-Agent"];
        [headerFields setValue:@"gzip,deflate,sdch" forKey:@"Accept-Encoding"];
        DidaNetWorkEngine * engine = [[DidaNetWorkEngine alloc] initWithHostName:SERVER_HOST customHeaderFields:headerFields];
        self.engine = engine;
        [engine release];
        //NSLog(@"%d",[self.engine retainCount]);
        [_engine useCache];
    }
    @catch (NSException *exception) {
        NSLog(@"The Network initial exception %@",exception.description);
    }
    
}

- (void)applicationWillResignActive:(UIApplication *)application
{
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application
{
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later. 
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application
{
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application
{
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application
{
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    [_engine emptyCache];
    [_engine release];
    [tabeViewController release];
    
    
}

#pragma tabviewbar click delegate
- (void)tabBarController:(UITabBarController *)tabBarController didSelectViewController:(UIViewController *)viewController {
    //click the item bar
    NSLog(@"onClicked...");
}

@end
