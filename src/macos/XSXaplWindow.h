//
//  XSXaplWindow.h
//  Xapl
//
//  Created by Julia on 6/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "XSFilePropertiesController.h"

@interface XSXaplWindow : NSWindowController 
{	
	// State variables
	BOOL filePropertiesWereOpen;
	
	// Toolbar items
	IBOutlet NSToolbarItem *next;
    IBOutlet NSToolbarItem *playPause;
	IBOutlet NSToolbarItem *previous;
    IBOutlet NSToolbarItem *stop;
	IBOutlet NSSlider *position;
	
	// File properties
	NSDrawer *filePropertiesDrawer;
	IBOutlet NSView *filePropertiesView;
	IBOutlet XSFilePropertiesController *fileProperties;
	
	// Playlist window controls
	IBOutlet NSSearchField *searchField;
	IBOutlet NSScrollView *playlistScrollView;
    IBOutlet NSTableView *playlist;
	
	IBOutlet NSBox *statusBarSeparator;
	IBOutlet NSTextField *itemCount;
	IBOutlet NSTextField *fileFormat;
	IBOutlet NSTextField *statusText;
	IBOutlet NSProgressIndicator *status;
}

- (void)awakeFromNib;

- (void)setFilePropertiesVisible:(BOOL)visible;
- (void)setPlaylistVisible:(BOOL)visible;
- (void)focusSearchField;

@end
