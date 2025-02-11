//
//  XSXaplWindow.m
//  Xapl
//
//  Created by Julia on 6/7/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSXaplWindow.h"

@implementation XSXaplWindow

- (void)awakeFromNib
{	
	// Allow position slider in toolbar to grow
	NSToolbar *toolbar = [[self window] toolbar];
	NSArray *toolbarItems = [toolbar items];
	for ( size_t i = 0; i < [toolbarItems count]; ++i )
	{
		if ( [[[toolbarItems objectAtIndex:i] view] class] == [NSSlider class] )
		{
			NSToolbarItem *positionItem = [toolbarItems objectAtIndex:i];
			[positionItem setMaxSize:NSMakeSize(10000, 26)];
		}
	}
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
	// Show/Hide File Properties
	NSMenuItem *showHideFilePropsMenuItem = [[[[NSApp mainMenu] itemWithTag:1] submenu] itemWithTag:2];
	if (fileProperties != nil)
	{
		[showHideFilePropsMenuItem setTitle:@"Hide File Properties"];
		[showHideFilePropsMenuItem setState:NSOnState];
	}
	else
	{
		[showHideFilePropsMenuItem setTitle:@"Show File Properties"];
		[showHideFilePropsMenuItem setState:NSOffState];
	}
	
	// Show/Hide Playlist
	NSMenuItem *showHidePlaylistMenuItem = [[[[NSApp mainMenu] itemWithTag:1] submenu] itemWithTag:3];
	if ([playlistScrollView isHidden])
	{
		[showHidePlaylistMenuItem setTitle:@"Show Playlist"];
		[showHidePlaylistMenuItem setState:NSOffState];
	}
	else
	{
		[showHidePlaylistMenuItem setTitle:@"Hide Playlist"];
		[showHidePlaylistMenuItem setState:NSOnState];
	}
}

- (void)setFilePropertiesVisible:(BOOL)visible
{
	if (filePropertiesView == nil)
	{
		if (!visible) return;
			
		if (![NSBundle loadNibNamed:@"XSFileProperties" owner:self]) 
		{
			NSLog(@"Error loading Nib XSFileProperties!");
			return;
		}
	}
	
	if ([playlistScrollView isHidden]) return;
	
	if (filePropertiesDrawer == nil)
	{
		if (!visible) return;
		
		filePropertiesDrawer = [[NSDrawer alloc] initWithContentSize:[filePropertiesView frame].size preferredEdge:NSMaxXEdge];
		[filePropertiesDrawer setDelegate:self];
		[filePropertiesDrawer setContentView:filePropertiesView];
		[filePropertiesDrawer setMinContentSize:[filePropertiesView frame].size];
		[filePropertiesDrawer setParentWindow:[self window]];
		[filePropertiesDrawer setPreferredEdge:NSMaxXEdge];
		[filePropertiesDrawer setLeadingOffset:0];
		[filePropertiesDrawer setTrailingOffset:20];
	}
		
	if (visible)
		[filePropertiesDrawer openOnEdge:NSMaxXEdge];
	else		
		[filePropertiesDrawer close];
	
	filePropertiesWereOpen = visible;
}

- (void)drawerDidClose:(NSNotification *)notification
{
	NSMenuItem *showHideFilePropsMenuItem = [[[[NSApp mainMenu] itemWithTag:1] submenu] itemWithTag:2];
	[showHideFilePropsMenuItem setTitle:@"Show File Properties"];
	[showHideFilePropsMenuItem setState:NSOffState];
}

- (void)setPlaylistVisible:(BOOL)visible
{
	NSRect oldFrame, newFrame;
	
	if (visible)
	{
		oldFrame = [[self window] frame];
		newFrame = NSMakeRect(oldFrame.origin.x, oldFrame.origin.y, 300, 394);
		
		[[self window] setMinSize:NSMakeSize(300, 362)];
		[[self window] setMaxSize:NSMakeSize(FLT_MAX, FLT_MAX)];
		[[self window] setFrame: newFrame display: YES animate: YES];
		[[self window] setShowsResizeIndicator:YES];
		[[self window] setShowsToolbarButton:YES];
		[searchField setHidden:NO];
		[playlistScrollView setHidden:NO];
		[playlistScrollView setFrame:NSMakeRect(-1, 20, 302, 293)];	// Fix playlist position
		[playlistScrollView display];								// Fix playlist position
		[statusBarSeparator setHidden:NO];
		[itemCount setHidden:NO];
		//[statusText setHidden:NO];
		//[status setHidden:NO];
		
		// TODO: make this work
		// NSMenuItem *showHideFilePropsMenuItem = [[[[NSApp mainMenu] itemWithTag:1] submenu] itemWithTag:2];
		// [showHideFilePropsMenuItem setEnabled:YES];
		if (filePropertiesWereOpen && filePropertiesDrawer != nil)
			[filePropertiesDrawer openOnEdge:NSMaxXEdge];
	}
	else
	{		
		if (filePropertiesDrawer != nil)
			[filePropertiesDrawer close];
		// TODO: make this work
		// NSMenuItem *showHideFilePropsMenuItem = [[[[NSApp mainMenu] itemWithTag:1] submenu] itemWithTag:2];
		// [showHideFilePropsMenuItem setEnabled:NO];
		
		oldFrame = [[self window] frame];
		newFrame = NSMakeRect(oldFrame.origin.x, oldFrame.origin.y, oldFrame.size.width, 60);
		
		[searchField setHidden:YES];
		[playlistScrollView setHidden:YES];
		[statusBarSeparator setHidden:YES];
		[itemCount setHidden:YES];
		[statusText setHidden:YES];
		[status setHidden:YES];
		[[[self window] toolbar] setVisible:YES];
		[[self window] setShowsToolbarButton:NO];
		[[self window] setMinSize:NSMakeSize(300, 28)];
		[[self window] setMaxSize:NSMakeSize(FLT_MAX, 28)];
		[[self window] setShowsResizeIndicator:NO];
		[[self window] setFrame: newFrame display: YES animate: YES];
	}
	
	[[NSApp mainMenu] update];
}

- (void)focusSearchField
{
	[[self window] makeFirstResponder:searchField];
	[searchField selectText:nil];
}

@end
