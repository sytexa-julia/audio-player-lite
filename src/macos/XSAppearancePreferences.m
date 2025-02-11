#import "XSAppearancePreferences.h"

@implementation XSAppearancePreferences
/****** MBPreferencesModule overrides ******/
- (NSString *)title
{
	return NSLocalizedString(@"Appearance", @"Appearance Preferences");
}

- (NSString *)identifier
{
	return @"AppearancePane";
}

- (NSImage *)image
{
	return [NSImage imageNamed:@"AppearancePref.icns"];
}

/****** Getters / Setters ******/

- (NSWindow *)playlistWindow
{
	return playlistWindow;
}

- (void)setPlaylistWindow:(NSWindow *)thePlaylistWindow
{
	playlistWindow = thePlaylistWindow;
}

- (NSTableView *)playlist
{
	return playlist;
}

- (void)setPlaylist:(NSTableView *)thePlaylist
{
	playlist = thePlaylist;
}

/****** IBActions ******/
- (IBAction)normalItemBgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[normalItem setBackgroundColor:newColor];
}

- (IBAction)normalItemFgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[normalItem setTextColor:newColor];
}

- (IBAction)opacityChanging:(id)sender 
{
	NSArray *docs = [NSApp orderedDocuments];
	float newAlpha;
	NSEnumerator *enumerator = [docs objectEnumerator];
	NSWindow *currentWindow;
	NSDocument *currentDoc;
	
	while (currentDoc = [enumerator nextObject])
	{
		currentWindow = [[[currentDoc windowControllers] objectAtIndex:0] window];
		newAlpha = ([sender intValue] / 255.0f);
		if (newAlpha < 1.0f)
			[currentWindow setOpaque:NO];
		else
			[currentWindow setOpaque:YES];
		
		[currentWindow setAlphaValue:newAlpha];
	}
}

- (IBAction)playingItemBgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[playingItem setBackgroundColor:newColor];
}

- (IBAction)playingItemFgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[playingItem setTextColor:newColor];
}

- (IBAction)queuedItemBgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[queuedItem setBackgroundColor:newColor];
}

- (IBAction)queuedItemFgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[queuedItem setTextColor:newColor];
}

- (IBAction)selectedItemBgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[selectedItem setBackgroundColor:newColor];
}

- (IBAction)selectedItemFgColorChanged:(id)sender 
{
    NSColor *newColor = [sender color];
	[selectedItem setTextColor:newColor];
}
@end
