/******************************************************************************
 * MiniSG - iPhone OS Scenegraph Rendering System
 * Copyright (c) 2009 Torsten Becker, Norman Holz, Lars Schneider,
 * Computer Graphics Systems Group at the Hasso-Plattner-Institute (HPI), 
 * Potsdam, Germany.
 * 
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from the 
 * use of this software. Permission is granted to anyone to use this software 
 * for any purpose, including commercial applications, and to alter it and
 * redistribute it freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in a
 *    product, an acknowledgment in the product documentation would be 
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be 
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 ******************************************************************************/

#import "GLViewInputController.h"


@implementation GLViewInputController


@synthesize invertedInputMode = _invertedInputMode,
            isActive = _isActive;


- (id)init
{
    if ((self = [super init]))
	{		
        _touchThreshold[0] = 0;
        _touchThreshold[1] = 0;
        _touchThreshold[2] = 0;
        
        _invertedInputMode = NO;
        _isActive = YES;
    }
    return self;
}


// UIAccelerometerDelegate method, called when the device accelerates.
- (void)accelerometer:(UIAccelerometer *)accelerometer didAccelerate:(UIAcceleration *)acceleration 
{
    // Steering
    float rotationDelta = -acceleration.z;
   
   NSLog(@"%f %f %f\n", acceleration.x,acceleration.y,acceleration.z);
//
//    if (fabs(rotationDelta) > CAMERA_FIRST_PERSON_ROTATION_TRESHOLD)
//    {
//        _camera->rotateAroundLookFrom(CAMERA_FIRST_PERSON_ROTATION_FACTOR * rotationDelta);
//    }
}


- (void)toggleAccelerometerInput
{
    // toggle accelerometer input
    if ([UIAccelerometer sharedAccelerometer].delegate != nil) 
    {
        NSLog(@"Accelerometer input deactivated...");
        [[UIAccelerometer sharedAccelerometer] setDelegate:nil];
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(10000.0f)];
    } 
    else 
    {
        NSLog(@"Accelerometer input activated...");
        [[UIAccelerometer sharedAccelerometer] setUpdateInterval:(1.0f / 60.0f)];
        [[UIAccelerometer sharedAccelerometer] setDelegate:self];
    }
}


- (void)singleTouchMoveDelta:(CGPoint)moveDelta
{
}


- (void)doubleTouchMoveDelta1:(CGPoint)moveDelta1 delta2:(CGPoint)moveDelta2
{
}


- (void)tripleTouchMoveDelta1:(CGPoint)moveDelta1 delta2:(CGPoint)moveDelta2 delta3:(CGPoint)moveDelta3
{
}


- (void)doubleTouchZoomGestureDelta:(CGFloat)zoomGestureDelta
{
}


- (void)singleTap
{
}


- (void)doubleTap
{
}


- (void)tripleTap
{
}


- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event 
{
	UITouch *touch = [touches anyObject];
	NSUInteger tapCount = [touch tapCount];
	
	switch (tapCount) {
		case 1:
			[self performSelector:@selector(singleTap) withObject:nil afterDelay:.4];
			break;
		case 2:
            // Double tap occured - cancel single tap
			[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(singleTap) object:nil];
			
            // Trigger double tap
            [self performSelector:@selector(doubleTap) withObject:nil afterDelay:.4];
            break;
        case 3:
            // Triple tap occured - cancel double tap
			[NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(doubleTap) object:nil];
			
            // Trigger triple tap
            [self tripleTap];
            break;
		default:
			break;
	}
}


- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event 
{
    NSUInteger touchCount = [touches count];
    
    _touchThreshold[touchCount-1]++;
    
    CGPoint newTouch[touchCount];
    CGPoint oldTouch[touchCount];
    CGPoint delta[touchCount];
    unsigned int i = 0;
        
    for(UITouch* touch in touches) {
        
        newTouch[i] = [touch locationInView:touch.view];
        oldTouch[i] = [touch previousLocationInView:touch.view];
        delta[i].x = newTouch[i].x - oldTouch[i].x;
        delta[i].y = newTouch[i].y - oldTouch[i].y;
        i++;
    }
        
    if ((touchCount == 1) && (_touchThreshold[touchCount-1] > 5))
    {
        _touchThreshold[1] = 0;
        _touchThreshold[2] = 0;
        
        [self singleTouchMoveDelta:delta[0]];
    } 
    else if ((touchCount == 2) && (_touchThreshold[touchCount-1] > 5))
    {
        _touchThreshold[0] = 0;
        _touchThreshold[2] = 0;
        
        float newDiff, oldDiff, diffX, diffY;
    
        diffX = (newTouch[0].x - newTouch[1].x);
        diffX = diffX*diffX;

        diffY = (newTouch[0].y - newTouch[1].y);
        diffY = diffY*diffY;

        newDiff = (diffX + diffY);
        
        diffX = (oldTouch[0].x - oldTouch[1].x);
        diffX = diffX*diffX;

        diffY = (oldTouch[0].y - oldTouch[1].y);
        diffY = diffY*diffY;
        
        oldDiff = (diffX + diffY);

        [self doubleTouchMoveDelta1:delta[0] delta2:delta[1]];
        [self doubleTouchZoomGestureDelta:(sqrt(newDiff)-sqrt(oldDiff))];
    } 
    else if ((touchCount == 3) && (_touchThreshold[touchCount-1] > 5))
    {
        _touchThreshold[1] = 0;
        _touchThreshold[2] = 0;
        
        [self tripleTouchMoveDelta1:delta[0] delta2:delta[1] delta3:delta[2]];
    }
}


- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event 
{
}


@end
