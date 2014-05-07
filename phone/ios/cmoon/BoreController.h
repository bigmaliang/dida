//
//  BoreController.h
//  cmoon
//
//  Created by li yajie on 12-7-25.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "ArcHelper.h"
#import "BaseController.h"

@interface BoreController : BaseController<UITableViewDataSource,UITableViewDelegate>
{
    UITableView * mTableView;
    
    NSMutableArray * mDatasource;
}

@property(nonatomic,retain) UITableView *mTableView;

@end
