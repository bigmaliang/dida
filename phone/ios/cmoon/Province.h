//
//  Province.h
//  cmoon
//
//  Created by li yajie on 12-5-27.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface Province : NSObject
{
    int id;
    int grade;
    int pid;
    NSString *province;//省份
    NSString * lanticle;
    NSString *portait;//纵坐标
}
@property int id;
@property int grade;
@property int pid;
@property(nonatomic,retain) NSString * province;
@property(nonatomic,retain) NSString * lanticle;
@property(nonatomic,retain) NSString * portait;


@end
