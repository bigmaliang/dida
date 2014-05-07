//
//  AboutControllerViewController.m
//  cmoon
//
//  Created by li yajie on 12-7-25.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "AboutController.h"

@interface AboutController ()

@end

@implementation AboutController

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)viewDidLoad
{
    [super viewDidLoad];
    	// Do any additional setup after loading the view.
    mWebview = [[UIWebView alloc]initWithFrame:CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)];
    NSURLRequest *request = [NSURLRequest requestWithURL:[NSURL URLWithString:@"http://imdida.org/readme.html"]];
    [mWebview loadRequest:request];
    mWebview.delegate = self;
    [self.view addSubview:mWebview];
    progress = [[MBProgressHUD alloc]initWithView:self.view];
    [progress setMode:MBProgressHUDModeIndeterminate];
    progress.taskInProgress = YES;
    [self.view addSubview:progress];
    [progress release];
    [mWebview release];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

-(void)dealloc{
    [mWebview release];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma webview show

- (void)webViewDidStartLoad:(UIWebView *)webView {
    [progress show:YES];
}
- (void)webViewDidFinishLoad:(UIWebView *)webView {
    [progress hide:YES];
}
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error {
    [progress hide:YES];
}

@end
