//
//  XSPlaylistItemMetaData.h
//  Xapl
//
//  Created by Julia Anderson on 9/16/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "XSPlaylistItem.h"

@interface XSPlaylistItemMetaData : NSObject 
{
	XSPlaylistItem* playlistItem;
	unsigned int	sizeBytes;
	NSString*		sizeBytesFormatted;
	
	NSImage*		albumArt;
	NSString*		title;
	NSString*		artist;
	NSString*		album;
	unsigned int	year;
	NSString*		comments;
	unsigned int	trackNumber;
	NSString*		genre;
	
	unsigned int	lengthSeconds;
	NSString*		lengthSecondsFormatted;
	unsigned int	bitRate;
	unsigned int	sampleRate;
	unsigned int	channelCount;
}

- (id) initFromPlaylistItem:(XSPlaylistItem*)plItem;
- (BOOL) readMetaData;

@property(readonly, nonatomic) XSPlaylistItem* playlistItem;
@property(readonly, nonatomic) unsigned int sizeBytes;

@property(readonly, nonatomic) NSImage* albumArt;
@property(readonly, nonatomic) NSString* title;
@property(readonly, nonatomic) NSString* artist;
@property(readonly, nonatomic) NSString* album;
@property(readonly, nonatomic) unsigned int year;
@property(readonly, nonatomic) NSString* comments;
@property(readonly, nonatomic) unsigned int trackNumber;
@property(readonly, nonatomic) NSString* genre;

@property(readonly, nonatomic) unsigned int lengthSeconds;
@property(readonly, nonatomic) unsigned int bitRate;
@property(readonly, nonatomic) unsigned int sampleRate;
@property(readonly, nonatomic) unsigned int channelCount;

@end
