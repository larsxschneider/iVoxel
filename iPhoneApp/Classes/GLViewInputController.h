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
#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>


#define INPUT(__MODE__) ((__MODE__) * (_invertedInputMode ? -1 : 1))


@interface GLViewInputController : NSObject<UIAccelerometerDelegate>
{
    int             _touchThreshold[3];
    BOOL            _invertedInputMode,
                    _isActive;
}

@property (assign, nonatomic) BOOL invertedInputMode;
@property (assign, nonatomic) BOOL isActive;

- (void)singleTap;
- (void)doubleTap;
- (void)tripleTap;

- (void)singleTouchMoveDelta:(CGPoint)moveDelta;
- (void)doubleTouchMoveDelta1:(CGPoint)moveDelta1 delta2:(CGPoint)moveDelta2;
- (void)tripleTouchMoveDelta1:(CGPoint)moveDelta1 delta2:(CGPoint)moveDelta2 delta3:(CGPoint)moveDelta3;

- (void)doubleTouchZoomGestureDelta:(CGFloat)zoomGestureDelta;

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event;
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event;

- (void)toggleAccelerometerInput;

@end
