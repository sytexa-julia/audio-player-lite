//
//  XSPlaylist.h
//  Xapl
//
//  Created by Julia Anderson on 9/13/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#define XSNoItem -1
#define XSNoIndex -1

static unsigned long playlistAutoNumber = 0;

@interface XSPlaylist : NSObject 
{
	@private
	NSMutableArray*		 playlistData;
	NSMutableDictionary* idToIndexMap;
	
	NSString* playlistFilePath;
	NSString* playlistName;
	long	  playingId;
	int		  playMode;
	int		  format;
	int		  currentFormat;
	
	@protected
	NSMutableArray* playbackHistory;
	NSMutableArray* queuedItems;
	long			nextId;
}

@end
