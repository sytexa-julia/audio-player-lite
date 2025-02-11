#import "XSAboutController.h"

@implementation XSAboutController

- (void)awakeFromNib
{
	[[self window] setExcludedFromWindowsMenu:YES];
	
	NSString *currVersionNumber = [[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleVersion"];
	[version setStringValue:[NSString stringWithFormat:@"Version %@ for Mac OS X", currVersionNumber]];
}

@end
