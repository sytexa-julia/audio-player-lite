//
//  XSAudio.h
//  Xapl
//
//  Created by Julia Anderson on 9/13/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import <Cocoa/Cocoa.h>

#import "xaiChannelEventListener.h"
#import "xAudioInterface.h"
#import "XSPlaylistItem.h"
#import "XSPlaylist.h"

@interface XSAudio : NSObject <XChannelEventListener>
{
	@private
	XAudioInterface* audioInterface;
	
	XSPlaylist* playlist;
	long currentTrackId;
	long nextTrackId;
}

+ (id) sharedInstance;
- (id) init;
- (NSUInteger) retainCount;
- (oneway void) release;
- (id) retain;
- (id) autorelease;
- (void) dealloc;

- (void) onChannelEvent:(int) eventID;

- (XSPlaylist*) playlist;
- (void) setPlaylist: (XSPlaylist*)pl;

- (NSArray*) allPlayableExtensions;
- (BOOL) isExtensionPlayable: (NSString*)ext;

- (BOOL) play;
- (BOOL) playWithFadeInTime: (int)fadeInSecs;
- (BOOL) pause;
- (BOOL) stop;
- (BOOL) stopWithFadeOutTime: (int)fadeOutSecs;
- (BOOL) next;
- (BOOL) previous;
- (BOOL) seekToPosition: (float)position;
- (BOOL) seekInDirection: (int)direction byAmount: (int)amount;
- (BOOL) setVolume: (unsigned char) volume;

- (unsigned char) volume;
- (double) trackPosition;
- (int) trackLength;
- (NSString*) trackInfo;
- (int) playbackState;

@end
