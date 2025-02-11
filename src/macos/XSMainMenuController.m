#import "XSMainMenuController.h"
#import "XSFilePropertiesController.h"
#import "XSXaplDocument.h"
#import "XSXaplWindow.h"
#import "XSAboutController.h"

#import "MBPreferencesController.h"
#import "XSGeneralPreferences.h"
#import "XSAudioPreferences.h"
#import "XSAppearancePreferences.h"

@implementation XSMainMenuController

- (id) init
{
	if (self = [super init])
	{	
		// Init state
		playlistDisplayMode = XSFileName;
		playMode = RepeatPlaylist;
		
		// Preferences
		XSGeneralPreferences *general = [[XSGeneralPreferences alloc] initWithNibName:@"XSGeneralPreferences" bundle:nil];
		XSAudioPreferences *audio = [[XSAudioPreferences alloc] initWithNibName:@"XSAudioPreferences" bundle:nil];
		XSAppearancePreferences *appearance = [[XSAppearancePreferences alloc] initWithNibName:@"XSAppearancePreferences" bundle:nil];
		[[MBPreferencesController sharedController] setModules:[NSArray arrayWithObjects:general, audio, appearance, nil]];
		[general release];
		[audio release];
		[appearance release];
		
		// Open panels
		addLocationPanel = [[NSOpenPanel alloc] init];
		[addLocationPanel setCanChooseDirectories: YES];
		[addLocationPanel setCanChooseFiles:NO];
		[addLocationPanel setAllowsMultipleSelection: YES];
		[addLocationPanel retain];
		
		addFilesPanel = [[NSOpenPanel alloc] init];
		[addFilesPanel setCanChooseDirectories:YES];
		[addFilesPanel setCanChooseFiles:YES];
		[addFilesPanel setAllowsMultipleSelection:YES];
		[addFilesPanel retain];

		return self;
	}
	
	return nil;
}

- (void)awakeFromNib
{}

- (void) addLocationPanelDidEnd: (NSOpenPanel*)opener 
					 returnCode: (int)returnCode 
					contextInfo: (void *) x
{
	NSArray *paths;
	int i;
	
	if (returnCode == NSOKButton)
	{
		paths = [opener filenames];
		
		for (i = 0; i < [paths count]; ++i)
		{
			NSFileManager *fm;
			NSDirectoryEnumerator *dirEnum;
			id object;
			
			fm = [NSFileManager defaultManager];
			dirEnum = [fm enumeratorAtPath:[paths objectAtIndex:i]];
			
			while ((object = [dirEnum nextObject])) 
			{
				NSLog(@"%@", [((NSString *)object) lastPathComponent]);
			}
		}
	}
}

- (void) addFilesPanelDidEnd: (NSOpenPanel*)opener 
				  returnCode: (int)returnCode 
				 contextInfo: (void *) x
{
	NSArray *paths;
	int i;
	
	if (returnCode == NSOKButton)
	{
		paths = [opener filenames];
		
		for (i = 0; i < [paths count]; ++i)
		{
			NSFileManager *fm;
			NSDirectoryEnumerator *dirEnum;
			id object;
			
			fm = [NSFileManager defaultManager];
			dirEnum = [fm enumeratorAtPath:[paths objectAtIndex:i]];
			
			while ((object = [dirEnum nextObject])) 
			{
				NSLog(@"%@", [((NSString *)object) lastPathComponent]);
			}
		}
	}
}

/****** Main Menu Actions ******/

- (IBAction)aboutXapl:(id)sender 
{
	XSAboutController *aboutXapl = [[XSAboutController alloc] init];
	if (![NSBundle loadNibNamed:@"XSAboutXapl" owner:aboutXapl]) 
		NSLog(@"Error loading Nib XSAboutXapl!");
	else 
		[[aboutXapl window] makeKeyAndOrderFront:nil];
}

- (IBAction)addFiles:(id)sender
{
	[addFilesPanel beginSheetForDirectory: NSHomeDirectory()
									 file: nil
									types: [NSArray arrayWithObjects: @"mp3", @"ogg", @"wav", @"aiff", @"flac", @"m4a", @"aac", @"alac", nil]
						   modalForWindow: [self window]
							modalDelegate: self
						   didEndSelector: @selector(addFilesPanelDidEnd:returnCode:contextInfo:)
							  contextInfo: NULL];
}

- (IBAction)addLocation:(id)sender 
{
	[addLocationPanel beginSheetForDirectory: NSHomeDirectory()
										file: nil
									   types: [NSArray arrayWithObjects: @"mp3", @"ogg", @"wav", @"aiff", @"flac", @"m4a", @"aac", @"alac", nil]
							  modalForWindow: [self window]
							   modalDelegate: self
							  didEndSelector: @selector(addLocationPanelDidEnd:returnCode:contextInfo:)
								 contextInfo: NULL];
}

- (IBAction)addPlaylistToFavorites:(id)sender 
{
    
}

- (IBAction)clearPlaybackHistory:(id)sender 
{
    
}

- (IBAction)clearPlaylist:(id)sender 
{
    
}

- (IBAction)search:(id)sender
{
	XSXaplWindow *xaplWindow = [[[NSApp orderedDocuments] objectAtIndex: 0] documentWindowController];
	[xaplWindow focusSearchField];
}

- (IBAction)goToPlayingItem:(id)sender 
{
    
}

- (IBAction)goToSelectedItem:(id)sender 
{
    
}

- (IBAction)manageFavorites:(id)sender
{
	if (manageFavorites == nil)
	{
		manageFavorites = [[XSManageFavoritesController alloc] init];
		if (![NSBundle loadNibNamed:@"XSManageFavorites" owner:manageFavorites])
		{
			NSLog(@"Error loading Nib XSManageFavorites!");
			return;
		}
	}
    [[manageFavorites window] makeKeyAndOrderFront:nil];
}

- (IBAction)newPlaylist:(id)sender 
{
    
}

- (IBAction)next:(id)sender 
{

}

- (IBAction)openPlaylist:(id)sender 
{
    
}

- (IBAction)pause:(id)sender 
{
    
}

- (IBAction)play:(id)sender 
{
    
}

- (IBAction)preferences:(id)sender 
{
    [[MBPreferencesController sharedController] showWindow:sender];
}

- (IBAction)previous:(id)sender 
{
    
}

- (IBAction)removeDuplicateFiles:(id)sender 
{
    
}

- (IBAction)removeMissingFiles:(id)sender
{
    
}

- (IBAction)repeatPlaylist:(id)sender 
{
    [repeatSongMenuItem setState:NSOffState];
	[shuffleMenuItem setState:NSOffState];
	[sender setState:NSOnState];
	playMode = RepeatPlaylist;
}

- (IBAction)repeatSong:(id)sender 
{
    [repeatPlaylistMenuItem setState:NSOffState];
	[shuffleMenuItem setState:NSOffState];
	[sender setState:NSOnState];
	playMode = RepeatSong;
}

- (IBAction)resetInvalidFlags:(id)sender 
{
    
}

- (IBAction)savePlaylistAs:(id)sender 
{
    
}

- (IBAction)showHidePlaylist:(id)sender 
{
	NSDocument *doc = [[NSApp orderedDocuments] objectAtIndex: 0];
	XSXaplDocument *xaplDoc = (XSXaplDocument *)doc;
	XSXaplWindow *xaplWindow = [xaplDoc documentWindowController];
	NSMenuItem *menuItem = (NSMenuItem *) sender;
	
	if ( [menuItem state] == NSOffState )
	{
		[menuItem setTitle:@"Hide Playlist"];
		[menuItem setState:NSOnState];
		[xaplWindow setPlaylistVisible:YES];
	}
	else if ( [menuItem state] == NSOnState )
	{
		[menuItem setTitle:@"Show Playlist"];
		[menuItem setState:NSOffState];
		[xaplWindow setPlaylistVisible:NO];
	}
}

- (IBAction)showHideFileProperties:(id)sender
{
	NSDocument *doc = [[NSApp orderedDocuments] objectAtIndex: 0];
	XSXaplDocument *xaplDoc = (XSXaplDocument *)doc;
	XSXaplWindow *xaplWindow = [xaplDoc documentWindowController];
	NSMenuItem *menuItem = (NSMenuItem *) sender;
	
	if ( [menuItem state] == NSOffState )
	{
		[menuItem setTitle:@"Hide File Properties"];
		[menuItem setState:NSOnState];
		[xaplWindow setFilePropertiesVisible:YES];
	}
	else if ( [menuItem state] == NSOnState )
	{
		[menuItem setTitle:@"Show File Properties"];
		[menuItem setState:NSOffState];
		[xaplWindow setFilePropertiesVisible:NO];
	}
}

- (IBAction)shuffle:(id)sender 
{
    [repeatPlaylistMenuItem setState:NSOffState];
	[repeatSongMenuItem setState:NSOffState];
	[sender setState:NSOnState];
	playMode = Shuffle;
}

- (IBAction)sleepTimer:(id)sender 
{
    
}

- (IBAction)softMute:(id)sender 
{
    
}

- (IBAction)stop:(id)sender 
{
    
}

- (IBAction)togglePlayMode:(id)sender 
{
    [repeatPlaylistMenuItem setState:NSOffState];
	[repeatSongMenuItem setState:NSOffState];
	[shuffleMenuItem setState:NSOffState];
	
	switch (playMode) 
	{
		case Shuffle:
			[repeatPlaylistMenuItem setState:NSOnState];
			playMode = RepeatPlaylist;
			break;
		case RepeatSong:
			[shuffleMenuItem setState:NSOnState];
			playMode = Shuffle;
			break;
		case RepeatPlaylist:
		default:
			[repeatSongMenuItem setState:NSOnState];
			playMode = RepeatSong;
			break;
	}
}

- (IBAction)togglePlaylistView:(id)sender 
{
	[viewAsFileNameMenuItem setState:NSOffState];
	[viewAsFullPathMenuItem setState:NSOffState];
	[viewAsExtM3UTitleMenuItem setState:NSOffState];
	
	switch (playlistDisplayMode)
	{
		case XSExtendedM3UTitle:
			[viewAsFileNameMenuItem setState:NSOnState];
			playlistDisplayMode = XSFileName;
			break;
		case XSFullPath:
			[viewAsExtM3UTitleMenuItem setState:NSOnState];
			playlistDisplayMode = XSExtendedM3UTitle;
			break;
		case XSFileName:
		default:
			[viewAsFullPathMenuItem setState:NSOnState];
			playlistDisplayMode = XSFullPath;
			break;
	}
}

- (IBAction)viewAsExtM3UData:(id)sender 
{
	[viewAsFileNameMenuItem setState:NSOffState];
	[viewAsFullPathMenuItem setState:NSOffState];
	[sender setState:NSOnState];
	playlistDisplayMode = XSExtendedM3UTitle;
}

- (IBAction)viewAsFileName:(id)sender 
{
	[viewAsFullPathMenuItem setState:NSOffState];
	[viewAsExtM3UTitleMenuItem setState:NSOffState];
	[sender setState:NSOnState];
	playlistDisplayMode = XSFileName;
}

- (IBAction)viewAsFullPath:(id)sender 
{
    [viewAsFileNameMenuItem setState:NSOffState];
	[viewAsExtM3UTitleMenuItem setState:NSOffState];
	[sender setState:NSOnState];
	playlistDisplayMode = XSFullPath;
}

- (IBAction)viewPlaybackHistory:(id)sender 
{
    
}

- (IBAction)volumeDown:(id)sender 
{
    
}

- (IBAction)volumeUp:(id)sender 
{
    
}


@end
