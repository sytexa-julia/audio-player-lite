//
//  XSPlaylistItemMetaData.m
//  Xapl
//
//  Created by Julia Anderson on 9/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSPlaylistItemMetaData.h"


@implementation XSPlaylistItemMetaData

- (id) initFromPlaylistItem:(XSPlaylistItem *)plItem
{
	if (self = [super init])
	{
		[self readMetaData];
		return self;
	}
	
	return nil;
}

- (BOOL) readMetaData
{
	if ([self playlistItem] == nil)
	{
		NSLog(@"Could not read meta data because playlistItem is nil.");
		return NO;
	}
		
	NSLog(@"Method not yet implemented.");
	return NO;
}

@synthesize playlistItem;
@synthesize sizeBytes;

@synthesize albumArt;
@synthesize title;
@synthesize artist;
@synthesize album;
@synthesize year;
@synthesize comments;
@synthesize trackNumber;
@synthesize genre;

@synthesize lengthSeconds;
@synthesize bitRate;
@synthesize sampleRate;
@synthesize channelCount;

@end
