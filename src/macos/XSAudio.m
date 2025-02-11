//
//  XSAudio.m
//  Xapl
//
//  Created by Julia Anderson on 9/13/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "XSAudio.h"

static XSAudio* _xsAudioSharedInstance = nil;

@implementation XSAudio
/* Singelton Class Implementation */
+ (id) sharedInstance
{
	@synchronized( self )
	{
		if ( _xsAudioSharedInstance == nil )
			[[self alloc] init];
	}
	
	return _xsAudioSharedInstance;
}

- (id) init
{
	if ( _xsAudioSharedInstance != nil )
	{
		[NSException raise:NSInternalInconsistencyException
					format:@"[%@ %@] cannot be called; use +[%@ %@] instead", 
						NSStringFromClass([self class]), 
						NSStringFromSelector(_cmd), 
						NSStringFromClass([self class]), 
						NSStringFromSelector(@selector(_xsAudioSharedInstance))];
	} 
	else if ( self = [super init] ) 
	{
		_xsAudioSharedInstance = self;
		
		// Get list of audio plugins
		NSBundle* main = [NSBundle mainBundle];
		NSArray* plugins = [main pathsForResourcesOfType:@"dylib" 
											 inDirectory:@"../PlugIns/Audio"];
		
		// Initialize audio interface
		audioInterface = [[XAudioInterface alloc] initWithPlugins:plugins 
														   buffer:3000 
											  forceSoftwareMixing:false];
		if (audioInterface == nil)
		{
			NSLog(@"Failed to initialize the audio subsystem.");
			[NSApp terminate];
		}
		else
		{
			[audioInterface setChannelEventListener:self];
			[audioInterface setVolume:1.0f];
		}		
	}
					  
	return _xsAudioSharedInstance;
}

- (NSUInteger) retainCount
{
	return NSUIntegerMax;
}

- (oneway void) release 
{}

- (id) retain 
{
	return _xsAudioSharedInstance;
}

- (id) autorelease
{
	return _xsAudioSharedInstance;
}

- (void) dealloc
{
	[audioInterface release];
	[super dealloc];
}

/* XChannelEventListener Implementation */
- (void) onChannelEvent:(int) eventID
{
	if ([audioInterface lastError] != 0 || [audioInterface state] != STATE_PLAYING)
		return;
	
	if (eventID == EVENT_MIXER_END)
	{
		// Get next song and load it (but don't play it until the end of the current track is heard).
		nextTrackId = [playlist next];
		XSPlaylistItem* plItem = [playlist itemWithId:nextTrackId];
		if (plItem != nil)
			[audioInterface loadNext:[plItem path]];
		else
			NSRunAlertPanel(@"Fatal Error",
							@"The next playlist item could not be played.",
							@"OK", nil, nil);
	}
	else if (eventID == EVENT_MIXER_END_HEARD)
	{
		XSPlaylistItem* oldTrack = [playlist playingItem];
		[playlist setPlayingItem:nextTrackId];
		[self notifyTrackChangedFrom:oldTrack to:[playlist playingItem]];
	}
}

/* XSPlaylist Getter/Setter */
- (XSPlaylist*) playlist
{
	return playlist;
}

- (void) setPlaylist: (XSPlaylist*)pl
{
	playlist = [pl retain];
}

/* Playable File Extension Info */
- (NSArray*) allPlayableExtensions
{
	return [audioInterface allPlayableFileExtensions];
}

- (BOOL) isExtensionPlayable: (NSString*)ext
{
	return NO;
}

/* Playback Control */
- (BOOL) play
{
	return NO;
}

- (BOOL) playWithFadeInTime: (int)fadeInSecs
{
	return NO;
}

- (BOOL) pause
{
	return NO;
}

- (BOOL) stop
{
	return NO;
}

- (BOOL) stopWithFadeOutTime: (int)fadeOutSecs
{
	return NO;
}

- (BOOL) next
{
	return NO;
}

- (BOOL) previous
{
	return NO;
}

- (BOOL) seekToPosition: (float)position
{
	return NO;
}

- (BOOL) seekInDirection: (int)direction byAmount: (int)amount
{
	return NO;
}

- (BOOL) setVolume: (unsigned char) volume;
{
	return NO;
}

/* Playback Info */
- (unsigned char) volume
{
	return 100;
}

- (double) trackPosition
{
	return 0.0;
}

- (int) trackLength
{
	return 0;
}

- (NSString*) trackInfo
{
	return @"";
}

- (int) playbackState
{
	return 0;
}
@end
