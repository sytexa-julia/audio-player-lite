//
//  xAudioInterface.m
//  Xapl
//
//  Created by Julia on 5/28/09.
//  Copyright 2009 __MyCompanyName__. All rights reserved.
//

#import "xAudioInterface.h"

static id <XChannelEventListener> m_channelEventListener;

static HSYNC mixerEndSync;
static HSYNC mixerEndHeardSync;
static HSYNC chanStartSync;
static HSYNC chanStallSync;
static HSYNC chanResumeSync;
static HSYNC chanFreedSync;
static HSYNC chanDlDoneSync;
static char s_activeChannel = 'A';
static BOOL s_randSeeded = NO;
static HSTREAM s_mixer;

static void	 NotifyChannelEventListener( XA_STATE eventID );
static void CALLBACK sync_MixerEnd( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_MixerEndHeard( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanStart( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanStall( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanResume( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanFreed( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanDLDone( HSYNC handle, DWORD channel, DWORD data, void *user );

void NotifyChannelEventListener( int eventId )
{
	if ( m_channelEventListener != nil )
		[m_channelEventListener onChannelEvent:eventId];
}

void CALLBACK sync_MixerEnd( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	NotifyChannelEventListener( EVENT_MIXER_END );
}

void CALLBACK sync_MixerEndHeard( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	s_activeChannel = s_activeChannel == 'A' ? 'B' : 'A';
	BASS_ChannelPlay( s_mixer, TRUE );
	NotifyChannelEventListener( EVENT_MIXER_END_HEARD );
}

void CALLBACK sync_ChanStart( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	// Since syncs run on their own thread, they have their own copy of
	// stdlib. So, need to seed the random generator for shuffle mode.
	if ( !s_randSeeded )
	{
		time_t now = time( NULL );
		unsigned char *p = (unsigned char *)&now;
		unsigned int seed = 0;
		size_t i;
		
		for ( i = 0; i < sizeof now; i++ )
			seed = seed * ( UCHAR_MAX + 2U ) + p[i];
		
		srand( seed );
		s_randSeeded = true;
	}
	
	NotifyChannelEventListener( EVENT_CHANNEL_START );
}

void CALLBACK sync_ChanStall( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	NotifyChannelEventListener( EVENT_CHANNEL_STALL );
}

void CALLBACK sync_ChanResume( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	NotifyChannelEventListener( EVENT_CHANNEL_RESUME );
}

void CALLBACK sync_ChanFreed( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	NotifyChannelEventListener( EVENT_CHANNEL_FREED );
}

void CALLBACK sync_ChanDLDone( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	NotifyChannelEventListener( EVENT_DOWNLOAD_DONE );
}

@implementation XAudioInterface

- (id) init
{
	if (self = [super init])
	{
		if ([self _initializeWithPlugins:nil buffer:3000 forceSoftwareMixing:NO])
			return self;
	}
	
	return nil;
}

- (id) initWithBufferSize:(int) buffer forceSoftwareMixing:(BOOL)forceSoftwareMixing
{
	if (self = [super init])
	{
		if([self _initializeWithPlugins:nil buffer:buffer forceSoftwareMixing:forceSoftwareMixing])
			return self;
	}
	
	return nil;
}

- (id) initWithPlugins:(NSArray*)plugins buffer: (int)buffer forceSoftwareMixing: (BOOL)forceSoftwareMixing
{
	if (self = [super init])
	{
		if ([self _initializeWithPlugins:plugins buffer:buffer forceSoftwareMixing:forceSoftwareMixing])
			return self;
	}
	
	return nil;
}

- (void) dealloc
{
	[m_infoStr release];
	[m_currentPath release];
	m_state = STATE_FREE;
	
	BASS_ChannelRemoveSync( s_mixer, mixerEndSync );
	BASS_ChannelRemoveSync( s_mixer, mixerEndHeardSync );
	
	BASS_StreamFree( channelA );
	BASS_StreamFree( channelB );
	BASS_StreamFree( s_mixer );
	BASS_Free();
	BASS_PluginFree( 0 );
	
	[super dealloc];
}

- (BOOL) _initializeWithPlugins:(NSArray*)plugins buffer: (int)buffer forceSoftwareMixing: (BOOL)forceSoftwareMixing
{
	m_buffer = buffer;
	m_forceSoftware = forceSoftwareMixing;
	
	// Initialize BASS
	if ( HIWORD(BASS_GetVersion()) != BASSVERSION ) 
	{
		m_lastError = BASS_ERROR_VERSION;
		m_state = STATE_STOPPED;
		return NO;
	}
	
	BASS_SetConfig( BASS_CONFIG_BUFFER, m_buffer );
	
	if ( !BASS_Init( 1, 44100, 0, 0, NULL ) )
	{
		m_lastError = BASS_ErrorGetCode();
		m_state = STATE_FREE;
		return NO;
	}
	
	// Load plugins
	if (plugins != nil)
	{	
		NSEnumerator* enumerator = [plugins objectEnumerator];
		NSString* currentData;
		while (currentData = [enumerator nextObject])
		{
			BASS_PluginLoad([currentData UTF8String], 0);
		}
	}
	
	// Setup mixer channel
	DWORD flags = m_forceSoftware ? BASS_MIXER_END | BASS_SAMPLE_SOFTWARE : BASS_MIXER_END;
	s_mixer = BASS_Mixer_StreamCreate( 44100, 2, flags );
	mixerEndSync = BASS_ChannelSetSync( s_mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, sync_MixerEnd, 0 );
	mixerEndHeardSync = BASS_ChannelSetSync( s_mixer, BASS_SYNC_END, 0, sync_MixerEndHeard, 0 );
	
	// Initialize state
	m_state = STATE_READY;
	m_currentPath = nil;
	m_lastError = BASS_OK;
	
	return YES;
}

/* ====================================================================
 * ==== Misc. =========================================================*/
- (NSArray*) allPlayableFileExtensions
{
	// Add BASS built-in formats
	NSMutableArray* exts = [[NSMutableArray alloc] initWithObjects:	@"wav", @"aif", @"aiff", 
																	@"mp1", @"mp2", @"mp3", 
																	@"ogg", nil];
	
	// Get plugin supported formats
	NSEnumerator* enumerator = [m_pluginHandles objectEnumerator];
	HPLUGIN currentData;
	while (currentData = (HPLUGIN)[enumerator nextObject])
	{
		const BASS_PLUGININFO *pluginInfo = BASS_PluginGetInfo( currentData );
		for ( size_t i = 0; i < pluginInfo->formatc; ++i )
		{
			const BASS_PLUGINFORM pluginForm = pluginInfo->formats[i];
			NSString* pluginFormStr = [[[NSString alloc] initWithUTF8String: pluginForm.exts] 
							   stringByReplacingOccurrencesOfString:@"*." 
															  withString:@""];
			[exts addObjectsFromArray: [pluginFormStr componentsSeparatedByString:@";"]];
			
		}
	}
	
	return exts;
}

/* ====================================================================
 * ==== File Loading ==================================================*/
- (BOOL) loadFile: (NSString *)fileName
{
	return [self loadFile:fileName andStartPlaying:YES];
}

- (BOOL) loadFile: (NSString *)fileName andStartPlaying: (BOOL) startPlay
{
	if ( m_state == STATE_FREE || fileName == NULL )
		return NO;

	m_state = STATE_STOPPED;
	[fileName retain];
	[m_currentPath release];
	m_currentPath = fileName;

	HSTREAM *oldChan = s_activeChannel == 'A' ? &channelA : &channelB;
	HSTREAM *newChan = s_activeChannel == 'A' ? &channelB : &channelA;

	BASS_ChannelStop( s_mixer );

	BASS_Mixer_ChannelRemove( *oldChan );
	BASS_ChannelRemoveSync( *oldChan, chanStartSync );
	BASS_ChannelRemoveSync( *oldChan, chanStallSync );
	BASS_ChannelRemoveSync( *oldChan, chanResumeSync );
	BASS_ChannelRemoveSync( *oldChan, chanFreedSync );
	BASS_ChannelRemoveSync( *oldChan, chanDlDoneSync );
	BASS_Mixer_ChannelRemove( *newChan );  // In case a song is manually loaded between Mixer_End and Mixer_End_Heard
	BASS_ChannelRemoveSync( *newChan, chanStartSync );
	BASS_ChannelRemoveSync( *newChan, chanStallSync );
	BASS_ChannelRemoveSync( *newChan, chanResumeSync );
	BASS_ChannelRemoveSync( *newChan, chanFreedSync );
	BASS_ChannelRemoveSync( *newChan, chanDlDoneSync );

	DWORD flags = m_forceSoftware ? BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_SAMPLE_SOFTWARE : BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT;
	*newChan = BASS_StreamCreateFile( false, [m_currentPath UTF8String], 0, 0, flags );
	if ( *newChan )
	{
		s_activeChannel = s_activeChannel == 'A' ? 'B' : 'A';
		m_state = STATE_FILELOADED;
		chanStartSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_POS, BASS_ChannelSeconds2Bytes( *newChan, m_buffer / 1000 ), sync_ChanStart, 0 );
		chanStallSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_STALL, 0, sync_ChanStall, 0 );
		chanResumeSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_STALL, 1, sync_ChanResume, 0 );
		chanFreedSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_FREE, 0, sync_ChanFreed, 0 );
		chanDlDoneSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_DOWNLOAD, 0, sync_ChanDLDone, 0 );
		
		BASS_Mixer_StreamAddChannel( s_mixer, *newChan, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN ); 
		BASS_ChannelPlay( s_mixer, TRUE );
		m_infoStale = true;
	}

	m_lastError = BASS_ErrorGetCode();

	if ( m_lastError != BASS_OK )
	return NO;

	if ( startPlay )
	{
		if ( !BASS_ChannelPlay( s_mixer, false ) )
		{
			return NO;
		}
		else
		{
			m_state = STATE_PLAYING;
		}
	}

	return YES;
}

- (BOOL) loadNext: (NSString *)fileName
{
	if ( fileName == nil )
		return NO;
	
	[fileName retain];
	[m_currentPath release];
	m_currentPath = fileName;
	
	HSTREAM *oldChan = s_activeChannel == 'A' ? &channelA : &channelB;
	HSTREAM *newChan = s_activeChannel == 'A' ? &channelB : &channelA;
	
	BASS_ChannelStop( s_mixer );
	
	BASS_ChannelRemoveSync( *oldChan, chanStartSync );
	BASS_ChannelRemoveSync( *oldChan, chanStallSync );
	BASS_ChannelRemoveSync( *oldChan, chanResumeSync );
	BASS_ChannelRemoveSync( *oldChan, chanFreedSync );
	BASS_ChannelRemoveSync( *oldChan, chanDlDoneSync );
	
	DWORD flags = m_forceSoftware ? BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_SAMPLE_SOFTWARE : BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT;
	*newChan = BASS_StreamCreateFile( false, [m_currentPath UTF8String], 0, 0, flags );
	if ( *newChan )
	{
		chanStartSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_POS, BASS_ChannelSeconds2Bytes( *newChan, m_buffer / 1000 ), sync_ChanStart, 0 );
		chanStallSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_STALL, 0, sync_ChanStall, 0 );
		chanResumeSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_STALL, 1, sync_ChanResume, 0 );
		chanFreedSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_FREE, 0, sync_ChanFreed, 0 );
		chanDlDoneSync = BASS_ChannelSetSync( *newChan, BASS_SYNC_DOWNLOAD, 0, sync_ChanDLDone, 0 );
		
		BASS_Mixer_StreamAddChannel( s_mixer, *newChan, BASS_STREAM_AUTOFREE | BASS_MIXER_NORAMPIN ); 
		m_infoStale = true;
		m_lastError = BASS_ErrorGetCode();
	}
	else 
	{
		m_lastError = BASS_ErrorGetCode();
		return NO;
	}
	
	return YES;
}

/* ====================================================================
 * ==== Starting Playback =============================================*/
- (BOOL) startPlayback
{
	if ( m_state == STATE_PAUSED || m_state == STATE_FILELOADED )
	{
		BASS_ChannelPlay( s_mixer, false );
		m_state = STATE_PLAYING;
		return YES;
	}
	else if ( m_state == STATE_PLAYING )
	{
		return YES;
	}
	
	return NO;
}

- (BOOL) startPlaybackWithFadeIn: (int)fadeInTimeMillis
{
	if ( m_state == STATE_PLAYING )
		return false;
	
	[self setVolume:0.0f];
	
	if ( BASS_ChannelIsSliding( s_mixer, BASS_ATTRIB_VOL ) )
		return false;
	
	return BASS_ChannelSlideAttribute( s_mixer, BASS_ATTRIB_VOL, 1.0f, fadeInTimeMillis ) && BASS_ChannelPlay( s_mixer, false );
}

/* ====================================================================
 * ==== Stopping Playback =============================================*/
- (BOOL) pausePlayback
{
	if ( m_state == STATE_PLAYING )
	{
		if ( BASS_ChannelPause( s_mixer ) )
		{
			m_state = STATE_PAUSED;
			return YES;
		}
	}
	else if ( m_state == STATE_PAUSED )
	{
		return YES;
	}
	
	return NO;
}

- (BOOL) stopPlayback
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		if ( BASS_ChannelStop( s_mixer ) )
		{
			m_state = STATE_STOPPED;
			return YES;
		}
	}
	
	return NO;
}

- (BOOL) stopPlaybackWithFadeOut: (int)fadeOutTimeMillis
{
	if ( m_state != STATE_PLAYING )
		return NO;
	
	if ( BASS_ChannelIsSliding( s_mixer, BASS_ATTRIB_VOL ) )
		return YES;
	
	BOOL result = BASS_ChannelSlideAttribute( s_mixer, BASS_ATTRIB_VOL, 0.0f, fadeOutTimeMillis );
	if ( result )
		return YES;
	
	return NO;
}

/* ====================================================================
 * ==== Set Playback Position / Volume ================================*/
- (BOOL) seekToPosition: (float) position
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		QWORD newPos = BASS_ChannelSeconds2Bytes( chan, position );
		if ( BASS_Mixer_ChannelSetPosition( chan, newPos, BASS_POS_BYTE ) &&
			BASS_ChannelSetPosition( s_mixer, 0, BASS_POS_BYTE ) )
		{
			return YES;
		}
	}
	
	return NO;
}

- (BOOL) setVolume: (float) volume
{
	if ( volume > 1.0 || volume < 0.0 )
		return NO;
	
	BASS_ChannelSetAttribute( s_mixer, BASS_ATTRIB_VOL, volume );
	return YES;
}

/* ====================================================================
 * ==== Getters =======================================================*/
- (double) currentPosition
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		QWORD currPos = BASS_Mixer_ChannelGetPosition( chan, BASS_POS_BYTE );
		return BASS_ChannelBytes2Seconds( chan, currPos );
	}
	
	return -1.0;
}

- (long long) currentPositionInBytes
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		QWORD currPos = BASS_Mixer_ChannelGetPosition( chan, BASS_POS_BYTE );
		return currPos;
	}
	
	return -1;
}

- (void) volume: (float *) volume
{
	if ( volume != NULL )
		BASS_ChannelGetAttribute( s_mixer, BASS_ATTRIB_VOL, volume );
}

- (int) streamLength;
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		return (int) BASS_ChannelBytes2Seconds( chan, [self streamLengthInBytes] );
	}
	
	return -1;
}

- (long long) streamLengthInBytes
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		return BASS_ChannelGetLength( chan, BASS_POS_BYTE );
	}
	
	return -1;
}

- (NSString *) activeFile
{
	return m_currentPath;
}

- (NSString *) activeFileInfoString
{
	if ( m_infoStale ) 
	{ 
		[self _recalcInfoString]; 
		m_infoStale = false; 
	} 
	
	return m_infoStr;
}

- (BOOL) getDeviceInfo: (BASS_INFO *) info
{
	if ( info == NULL || !BASS_GetInfo( info ) )
		return NO;
	
	return YES;
}

- (XA_STATE) state
{
	return m_state;
}

- (int) lastError
{
	return m_lastError;
}

- (BOOL) isReady
{
	return m_state != STATE_FREE;
}

/* ====================================================================
 * ==== Channel Event Listener ========================================*/
- (void) setChannelEventListener: (id <XChannelEventListener>) listener
{
	m_channelEventListener = listener;
}

- (void) _recalcInfoString
{
	NSString *encoding, *frequency, *channels;
		
	BASS_CHANNELINFO info;
	DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
	BASS_ChannelGetInfo( chan, &info );
	
	// Get container type
	switch ( info.ctype )
	{
		case BASS_CTYPE_STREAM_OGG:
			encoding = @"Ogg Vorbis  ";
			break;
		case BASS_CTYPE_STREAM_MP1:
			encoding = @"MPEG-1  ";
			break;
		case BASS_CTYPE_STREAM_MP2:
			encoding = @"MPEG-2  ";
			break;
		case BASS_CTYPE_STREAM_MP3:
			encoding = @"MPEG-3  ";
			break;
		case BASS_CTYPE_STREAM_AIFF:
			encoding = @"AIFF  ";
			break;
		case BASS_CTYPE_STREAM_WAV_PCM:
			encoding = @"Integer PCM WAVE  ";
			break;
		case BASS_CTYPE_STREAM_WAV_FLOAT:
			encoding = @"Floating-Point PCM WAVE  ";
			break;
		case BASS_CTYPE_STREAM_WAV:
			encoding = @"WAVE  ";
			break;
	}
	
	// Get frequency
	QWORD len = BASS_StreamGetFilePosition( chan, BASS_FILEPOS_END );
	double time = BASS_ChannelBytes2Seconds( chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE) );
	DWORD kbps = (DWORD) ( len / (125 * time) + 0.5 );
	
	frequency = [NSString stringWithFormat:@"%u kbps  ", kbps];
	
	// Get channels
	switch ( info.chans )
	{
		case 1:
			channels = @"Mono";
			break;
		case 2:
			channels = @"Stereo";
			break;
		default:
			channels = [NSString stringWithFormat:@"(%u Channels)", info.chans];
			break;
	}
	
	// Build new information string
	[m_infoStr release];
	m_infoStr = [[NSString alloc] initWithFormat:@"%@%@%@", encoding, frequency, channels];
}

@end
