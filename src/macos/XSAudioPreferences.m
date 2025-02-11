#import "XSAudioPreferences.h"

@implementation XSAudioPreferences
- (NSString *)title
{
	return NSLocalizedString(@"Audio", @"Audio Preferences");
}

- (NSString *)identifier
{
	return @"AudioPane";
}

- (NSImage *)image
{
	return [NSImage imageNamed:@"SoundPref.icns"];
}

- (IBAction)bufferSizeStep:(id)sender 
{
    int value = [bufferSizeStepper intValue];
	[bufferSize setStringValue: [NSString stringWithFormat: @"%ims", value]];
}

- (IBAction)bufferSizeChanged:(id)sender
{
	NSInteger integer = [sender integerValue];
	if (integer < 100) integer = 100;
	if (integer > 5000) integer = 5000;
	[bufferSize setStringValue: [NSString stringWithFormat:@"%ims", integer]];
}
@end
