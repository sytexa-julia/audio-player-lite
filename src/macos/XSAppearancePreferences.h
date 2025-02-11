#import <Cocoa/Cocoa.h>
#import "MBPreferencesController.h"

@interface XSAppearancePreferences : NSViewController <MBPreferencesModule>
{
	NSWindow *playlistWindow;
	NSTableView *playlist;
    
	IBOutlet NSTextField *normalItem;
    IBOutlet NSColorWell *normalItemBg;
    IBOutlet NSColorWell *normalItemFg;
    IBOutlet NSSlider *opacity;
    IBOutlet NSTextField *playingItem;
    IBOutlet NSColorWell *playingItemBg;
    IBOutlet NSColorWell *playingItemFg;
    IBOutlet NSTextField *queuedItem;
    IBOutlet NSColorWell *queuedItemBg;
    IBOutlet NSColorWell *queuedItemFg;
    IBOutlet NSTextField *selectedItem;
    IBOutlet NSColorWell *selectedItemBg;
    IBOutlet NSColorWell *selectedItemFg;
}

- (NSString *)title;
- (NSString *)identifier;
- (NSImage *)image;

- (NSWindow *)playlistWindow;
- (void)setPlaylistWindow:(NSWindow *)thePlaylistWindow;
- (NSTableView *)playlist;
- (void)setPlaylist:(NSTableView *)thePlaylist;

- (IBAction)normalItemBgColorChanged:(id)sender;
- (IBAction)normalItemFgColorChanged:(id)sender;
- (IBAction)opacityChanging:(id)sender;
- (IBAction)playingItemBgColorChanged:(id)sender;
- (IBAction)playingItemFgColorChanged:(id)sender;
- (IBAction)queuedItemBgColorChanged:(id)sender;
- (IBAction)queuedItemFgColorChanged:(id)sender;
- (IBAction)selectedItemBgColorChanged:(id)sender;
- (IBAction)selectedItemFgColorChanged:(id)sender;
@end
