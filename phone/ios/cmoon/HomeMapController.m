//
//  HomeMapController.m
//  cmoon
//
//  Created by li yajie on 12-7-23.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "HomeMapController.h"

@interface HomeMapController ()

@end

@implementation HomeMapController

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
    isLocated = false;
    if ([CLLocationManager locationServicesEnabled]) {
        mLocationMgr = [[CLLocationManager alloc] init];
        [mLocationMgr setDelegate:self];
        mLocationMgr.distanceFilter = 100.0f;
        mLocationMgr.desiredAccuracy = kCLLocationAccuracyBest;
        [mLocationMgr startUpdatingLocation];
    }

    UIView * container = [[UIView alloc] initWithFrame:CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)];
    self.view = container;
    mapView = [[MKMapView alloc] initWithFrame:CGRectMake(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)];
    mapView.zoomEnabled = YES;
    mapView.scrollEnabled = YES;
    mapView.mapType = MKMapTypeStandard;
    mapView.showsUserLocation = YES;
    [self.view addSubview:mapView];
    [mapView release];
	// Do any additional setup after loading the view.
    //    MKCoordinateSpan span;
//    region.span = span;
    mapView.delegate = self;
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

#pragma 真机才能定位操作
- (MKAnnotationView *)mapView:(MKMapView *)mV viewForAnnotation:(id <MKAnnotation>)annotation
{
    MKPinAnnotationView *pinView = nil;
    if (isLocated) {
        static NSString *defaultPinID = @"imdida.org";
        pinView = (MKPinAnnotationView *)[mapView dequeueReusableAnnotationViewWithIdentifier:defaultPinID];
        if ( pinView == nil ) pinView = [[[MKPinAnnotationView alloc]
                                          initWithAnnotation:annotation reuseIdentifier:defaultPinID] autorelease];
        pinView.pinColor = MKPinAnnotationColorRed;
        pinView.canShowCallout = YES;
        pinView.animatesDrop = YES;
        [mapView.userLocation setTitle:@"当前位置"];
        [mapView.userLocation setSubtitle:@"content"];
    }
    return pinView;
}
#pragma location manager update
-(void)locationManager:(CLLocationManager *)manager didUpdateToLocation:(CLLocation *)newLocation fromLocation:(CLLocation *)oldLocation {
    curLoc = manager.location.coordinate;
    MKCoordinateRegion region;
    region.center.latitude = curLoc.latitude == 0.0 ? 35.9097 : curLoc.latitude;
    region.center.longitude = curLoc.longitude == 0.0 ? 115.3476 : curLoc.longitude;
    
    [mapView setRegion:region animated:TRUE];
    isLocated = true;
    [mLocationMgr stopUpdatingLocation];//结束更新
}

-(void)dealloc {
    
    [mapView release];
    
    if (mLocationMgr != nil) {
        [mLocationMgr release];
        mLocationMgr = nil;
    }
    //[super dealloc];
}

@end
