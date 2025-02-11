#import <Cocoa/Cocoa.h>

@interface XSAboutController : NSWindowController 
{
    IBOutlet NSTextField *version;
}

- (void)awakeFromNib;

@end
