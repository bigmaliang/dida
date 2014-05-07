//
//  DidaViewController.m
//  cmoon
//
//  Created by li yajie on 12-5-10.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "DidaViewController.h"

@interface DidaViewController ()

@end

@implementation DidaViewController

- (void)viewDidLoad
{
    [super viewDidLoad];
    UIView * view = [[UIView alloc]initWithFrame:CGRectMake(0, 0, [UIScreen mainScreen].bounds.size.width, [UIScreen mainScreen].bounds.size.height)];
    view.backgroundColor = [UIColor whiteColor];
    self.view = view;
    [view release];
    UIButton *button = [UIButton buttonWithType:UIButtonTypeRoundedRect ];
    button.frame = CGRectMake(20, 80, 120, 30);
    [button setTitle:@"Click" forState:UIControlStateNormal];
    [button addTarget:self action:@selector(testA) forControlEvents:UIControlEventTouchUpInside];
    [self.view addSubview:button];
	// Do any additional setup after loading the view, typically from a nib.
}

-(void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
//    [self performSelectorInBackground:@selector(test) withObject:nil];
    
}

-(void) testA{
    
    DidaAppDelegate * app = (DidaAppDelegate *)[UIApplication sharedApplication].delegate;
    [app.engine sendServerRequest:@"/json/bore" withParam:nil userMethod:@"GET"];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation != UIInterfaceOrientationPortraitUpsideDown);
}

@end
