#import <Cocoa/Cocoa.h>
#import "XSPlaylistItem.h"
#import "XSManageFavoritesController.h"

typedef enum
{
	RepeatPlaylist,
	RepeatSong,
	Shuffle
} XaplPlayMode;

@interface XSMainMenuController : NSWindowController 
{	
	NSOpenPanel *addLocationPanel;
	NSOpenPanel *addFilesPanel;
	
	// State
	int playMode;
	int playlistDisplayMode;
	
	// Some menu items
	IBOutlet NSMenuItem *viewAsExtM3UTitleMenuItem;
	IBOutlet NSMenuItem *viewAsFileNameMenuItem;
	IBOutlet NSMenuItem *viewAsFullPathMenuItem;
	
	IBOutlet NSMenuItem *repeatPlaylistMenuItem;
	IBOutlet NSMenuItem *repeatSongMenuItem;
	IBOutlet NSMenuItem *shuffleMenuItem;
			
	// Manage playlist window
	XSManageFavoritesController *manageFavorites;
}

- (id)init;
- (void)awakeFromNib;

- (void) addLocationPanelDidEnd: (NSOpenPanel*)addLocationPanel returnCode: (int)returnCode contextInfo: (void *) x;
- (void) addFilesPanelDidEnd: (NSOpenPanel*)addFilesPanel returnCode: (int)returnCode contextInfo: (void *) x;

/****** Main Menu Actions ******/
- (IBAction)aboutXapl:(id)sender;
- (IBAction)addFiles:(id)sender;
- (IBAction)addLocation:(id)sender;
- (IBAction)addPlaylistToFavorites:(id)sender;
- (IBAction)clearPlaybackHistory:(id)sender;
- (IBAction)clearPlaylist:(id)sender;
- (IBAction)search:(id)sender;
- (IBAction)goToPlayingItem:(id)sender;
- (IBAction)goToSelectedItem:(id)sender;
- (IBAction)manageFavorites:(id)sender;
- (IBAction)newPlaylist:(id)sender;
- (IBAction)next:(id)sender;
- (IBAction)openPlaylist:(id)sender;
- (IBAction)pause:(id)sender;
- (IBAction)play:(id)sender;
- (IBAction)preferences:(id)sender;
- (IBAction)previous:(id)sender;
- (IBAction)removeDuplicateFiles:(id)sender;
- (IBAction)removeMissingFiles:(id)sender;
- (IBAction)repeatPlaylist:(id)sender;
- (IBAction)repeatSong:(id)sender;
- (IBAction)resetInvalidFlags:(id)sender;
- (IBAction)savePlaylistAs:(id)sender;
- (IBAction)showHidePlaylist:(id)sender;
- (IBAction)showHideFileProperties:(id)sender;
- (IBAction)shuffle:(id)sender;
- (IBAction)sleepTimer:(id)sender;
- (IBAction)softMute:(id)sender;
- (IBAction)stop:(id)sender;
- (IBAction)togglePlayMode:(id)sender;
- (IBAction)togglePlaylistView:(id)sender;
- (IBAction)viewAsExtM3UData:(id)sender;
- (IBAction)viewAsFileName:(id)sender;
- (IBAction)viewAsFullPath:(id)sender;
- (IBAction)viewPlaybackHistory:(id)sender;
- (IBAction)volumeDown:(id)sender;
- (IBAction)volumeUp:(id)sender;
@end
