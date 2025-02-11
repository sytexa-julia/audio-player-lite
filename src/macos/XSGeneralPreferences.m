#import "XSGeneralPreferences.h"

@implementation XSGeneralPreferences
- (NSString *)title
{
	return NSLocalizedString(@"General", @"General Preferences");
}

- (NSString *)identifier
{
	return @"GeneralPane";
}

- (NSImage *)image
{
	return [NSImage imageNamed:NSImageNamePreferencesGeneral];
}

- (IBAction) openAAcChanged: (id) sender
{}

- (IBAction) openAlacChanged: (id) sender
{}

- (IBAction) openFlacChanged: (id) sender
{}

- (IBAction) openMp3Changed: (id) sender
{}

- (IBAction) openOggChanged: (id) sender
{}

- (IBAction) openWavChanged: (id) sender
{}

- (IBAction) showXaplWhereChanged: (id) sender
{}

- (IBAction) alwaysOnTopChanged: (id) sender
{}

- (IBAction) removeInvalidOnSaveChanged: (id) sender
{}
@end
