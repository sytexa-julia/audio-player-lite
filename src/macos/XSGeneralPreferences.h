#import <Cocoa/Cocoa.h>
#import "MBPreferencesController.h"

@interface XSGeneralPreferences : NSViewController <MBPreferencesModule>
{
    IBOutlet NSButton *openAac;
    IBOutlet NSButton *openAlac;
    IBOutlet NSButton *openFlac;
    IBOutlet NSButton *openMp3;
    IBOutlet NSButton *openOgg;
    IBOutlet NSButton *openWav;
	
	IBOutlet NSButton *alwaysOnTop;
    IBOutlet NSButton *removeInvalidOnSave;    
}

- (NSString *)title;
- (NSString *)identifier;
- (NSImage *)image;

- (IBAction) openAAcChanged: (id) sender;
- (IBAction) openAlacChanged: (id) sender;
- (IBAction) openFlacChanged: (id) sender;
- (IBAction) openMp3Changed: (id) sender;
- (IBAction) openOggChanged: (id) sender;
- (IBAction) openWavChanged: (id) sender;
- (IBAction) showXaplWhereChanged: (id) sender;
- (IBAction) alwaysOnTopChanged: (id) sender;
- (IBAction) removeInvalidOnSaveChanged: (id) sender;

@end
