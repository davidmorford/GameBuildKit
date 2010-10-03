
/*!
@header		Cocos2DLibrary.h
@project	Cocos2D
@author		David
@copyright	(c) 2010, Semantap
@created	9/29/10
*/

#import <SenTestingKit/SenTestingKit.h>
#import <UIKit/UIKit.h>
#import <Cocos2D/Cocos2D.h>

@interface Cocos2DLibraryUnitTest : SenTestCase {

}

	-(void) testMath;

@end

#pragma mark -

@implementation Cocos2DLibraryUnitTest

	-(void) testMath {
		STAssertTrue((1 + 1) == 2, @"Compiler isn't feeling well today :-(" );
	}

@end
