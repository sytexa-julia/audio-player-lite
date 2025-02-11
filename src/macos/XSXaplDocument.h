//
//  XSXaplDocument.h
//  Xapl
//
//  Created by Julia on 6/5/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "XSXaplWindow.h"

@interface XSXaplDocument : NSDocument 
{
	IBOutlet XSXaplWindow *documentWindowController;
	NSMutableArray *playlistData;
}

- (id)init;
- (XSXaplWindow *)documentWindowController;

@end
