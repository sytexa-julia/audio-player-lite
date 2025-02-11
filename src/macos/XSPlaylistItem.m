//
//  XSPlaylistItem.m
//  Xapl
//
//  Created by Julia on 6/1/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSPlaylistItem.h"

XSPlaylistItem* XSCreatePlaylistItem(long anId, NSString* aPath, int aFormat,
									   NSString* m3uTitle, unsigned int m3uLength, 
									   BOOL netStream)
{
	XSPlaylistItem* item = [[XSPlaylistItem alloc] initWithId:anId 
												  andFilePath:aPath 
													   format:aFormat];
	if (m3uTitle != nil) 
		[item setM3uTitle:m3uTitle];
	[item setM3uLength:m3uLength];
	[item setIsNetStream:netStream];
	
	return item;
}

@implementation XSPlaylistItem

@synthesize identifier;

@synthesize isNetStream;
@synthesize isOk;

@synthesize format;
@synthesize displayText;
@synthesize filePath;
@synthesize m3uLength;
@synthesize m3uTitle;

- (id)initWithId:(long)anId andFilePath:(NSString *)aFilePath format:(int)aFormat
{	
	if (self = [super init])
	{
		identifier = anId;
		m3uLength = 0;
		[self setFilePath:aFilePath];
		[self setFormat:aFormat];
		return self;
	}
	
	return nil;
}

- (void)dealloc
{
	[filePath release];
	[displayText release];
	[m3uTitle release];
	[super dealloc];
}

- (void)setFormat:(int)aFormat
{
	NSRange range;
	format = aFormat;
	
	switch (format) 
	{
		case XSFileName:
			range = [filePath rangeOfString:@"/" options:NSBackwardsSearch];
			if (range.location == NSNotFound)
			{
				displayText = filePath;
				break;
			}
			displayText = [filePath substringFromIndex:range.location + 1];
			break;
		case XSExtendedM3UTitle:
			if (m3uTitle == nil || [m3uTitle length] == 0)
				displayText = filePath;
			else
				displayText = m3uTitle;
			break;
		case XSFullPath:			
		default:
			displayText = filePath;
			break;
	}
}
@end
