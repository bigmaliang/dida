//
//  HomeMapController.h
//  cmoon
//
//  Created by li yajie on 12-7-23.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
//#import <MapKit/MapKit.h>
#import "ArcHelper.h"
#import "BaseController.h"

@interface HomeMapController : BaseController <MKMapViewDelegate,CLLocationManagerDelegate>{
    MKMapView * mapView;
    CLLocationCoordinate2D curLoc;//当前位置信息
    CLLocationManager * mLocationMgr;//位置管理操作类
    
    bool isLocated;
}

@end
