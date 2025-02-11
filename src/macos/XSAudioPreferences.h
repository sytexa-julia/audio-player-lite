#import <Cocoa/Cocoa.h>
#import "MBPreferencesController.h"

@interface XSAudioPreferences : NSViewController <MBPreferencesModule>
{
    IBOutlet NSTextField *bufferSize;
    IBOutlet NSStepper *bufferSizeStepper;
    IBOutlet NSButton *forceSoftwareMixing;
}

- (NSString *)title;
- (NSString *)identifier;
- (NSImage *)image;

- (IBAction)bufferSizeStep:(id)sender;
- (IBAction)bufferSizeChanged:(id)sender;
@end
