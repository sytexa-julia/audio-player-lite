#include "mswAudioInterface.h"

void CALLBACK sync_ChanEnd( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	mswAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_END );
}

void CALLBACK sync_ChanStall( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	mswAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_STALL );
}

void CALLBACK sync_ChanResume( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	mswAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_RESUME );
}

void CALLBACK sync_ChanFreed( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	mswAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_FREED );
}

void CALLBACK sync_ChanDLDone( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	mswAudioInterface::NotifyChannelEventListener( EVENT_DOWNLOAD_DONE );
}

mswAudioInterface::mswAudioInterface( void )
{
	if ( HIWORD(BASS_GetVersion()) != BASSVERSION ) 
	{
		m_lastError = XA_ERROR_VERSION;
		m_state = STATE_STOPPED;
	}

	if ( !BASS_Init( 1, 44100, 0, 0, NULL ) )
	{
		m_lastError = BASS_ErrorGetCode();
		m_state = STATE_STOPPED;
	}

	m_state = STATE_READY;
	m_currentPath = NULL;
}

mswAudioInterface::~mswAudioInterface( void )
{
	m_state = STATE_STOPPED;
	BASS_StreamFree( channel );
	BASS_Free();
	BASS_PluginFree( 0 );
}

bool mswAudioInterface::LoadFile( const char *fileName, bool startPlay )
{
	if ( m_state == STATE_FREE && !BASS_Init( 1, 44100, 0, 0, NULL ) )
	{
		return false;
	}
	
	// If the specified file is already loaded, return.
	// Otherwise, clear the buffer and continue.
	if ( m_currentPath != NULL && strcmp( m_currentPath, fileName ) == 0 && m_state != STATE_STOPPED )
	{
		Seek( 0.0f );
		return true;
	}
	else
	{
		delete m_currentPath;
	}

	if ( m_state != STATE_STOPPED )
	{
		BASS_ChannelRemoveSync( channel, chanEndSync );
		BASS_ChannelRemoveSync( channel, chanStallSync );
		BASS_ChannelRemoveSync( channel, chanResumeSync );
		BASS_ChannelRemoveSync( channel, chanFreedSync );
		BASS_ChannelRemoveSync( channel, chanDlDoneSync );

		BASS_StreamFree( channel );
	}

	m_state = STATE_STOPPED;

	size_t length = strlen( fileName );
	m_currentPath = new char[ length + 1 ];
	strcpy_s( m_currentPath, length + 1, fileName );
	
	if ( !(channel = BASS_StreamCreateFile( false, m_currentPath, 0, 0, BASS_SAMPLE_LOOP )) )
	{
		m_lastError = BASS_ErrorGetCode();
		return false;
	}

	m_state = STATE_FILELOADED;

	// Prebuffer the channel for gapless playback
	BASS_ChannelUpdate( channel, 0 );

	// Init sync callbacks
	chanEndSync = BASS_ChannelSetSync( channel, BASS_SYNC_END, 0, &sync_ChanEnd, 0 );
	chanStallSync = BASS_ChannelSetSync( channel, BASS_SYNC_STALL, 0, &sync_ChanEnd, 0 );
	chanResumeSync = BASS_ChannelSetSync( channel, BASS_SYNC_STALL, 1, &sync_ChanResume, 0 );
	chanFreedSync = BASS_ChannelSetSync( channel, BASS_SYNC_FREE, 0, &sync_ChanFreed, 0 );
	chanDlDoneSync = BASS_ChannelSetSync( channel, BASS_SYNC_DOWNLOAD, 0, &sync_ChanDLDone, 0 ); 

	if ( startPlay )
	{
		if ( !BASS_ChannelPlay( channel, false ) )
		{
			return false;
		}
		else
		{
			m_state = STATE_PLAYING;
		}
	}
	
	return true;
}

bool mswAudioInterface::StartPlayback( void )
{
	if ( m_state == STATE_PAUSED )
	{
		BASS_ChannelPlay( channel, false );
		m_state = STATE_PLAYING;
		return true;
	}
	else if ( m_state == STATE_FILELOADED )
	{
		BASS_ChannelPlay( channel, false );
		return true;
	}
	else if ( m_state == STATE_PLAYING )
	{
		return true;
	}

	return false;
}

bool mswAudioInterface::PausePlayback( void )
{
	if ( m_state == STATE_PLAYING )
	{
		if ( BASS_ChannelPause( channel ) )
		{
			m_state = STATE_PAUSED;
			return true;
		}
	}
	else if ( m_state == STATE_PAUSED )
	{
		return true;
	}

	return false;
}

bool mswAudioInterface::StopPlayback( bool releaseResources )
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		if ( BASS_ChannelStop( channel ) )
		{
			m_state = STATE_STOPPED;

			if ( releaseResources )
			{
				BASS_ChannelRemoveSync( channel, chanEndSync );
				BASS_ChannelRemoveSync( channel, chanStallSync );
				BASS_ChannelRemoveSync( channel, chanResumeSync );
				BASS_ChannelRemoveSync( channel, chanFreedSync );
				BASS_ChannelRemoveSync( channel, chanDlDoneSync );
				BASS_StreamFree( channel );
				BASS_Free();
				BASS_PluginFree( 0 );
				m_state = STATE_FREE;
			}

			return true;
		}
	}

	return false;
}

bool mswAudioInterface::Seek( float position )
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		QWORD newPos = BASS_ChannelSeconds2Bytes( channel, position );
		if ( BASS_ChannelSetPosition( channel, newPos, BASS_POS_BYTE ) )
		{
			return true;
		}
	}
	
	return false;
}

bool mswAudioInterface::SetVolume( float volume )
{
	if ( volume > 1.0 || volume < 0.0 )
		return false;

	BASS_ChannelSetAttribute( channel, BASS_ATTRIB_VOL, volume );
	return true;
}

double mswAudioInterface::GetCurrentPosition( void )
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		QWORD currPos = BASS_ChannelGetPosition( channel, BASS_POS_BYTE );
		return BASS_ChannelBytes2Seconds( channel, currPos );
	}

	return -1.0f;
}
	
void mswAudioInterface::GetCurrentVolume( float *vol )
{
	if ( vol != NULL )
	BASS_ChannelGetAttribute( channel, BASS_ATTRIB_VOL, vol );
}

int mswAudioInterface::GetStreamLength( void )
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		return (int) BASS_ChannelBytes2Seconds( channel, GetStreamLengthBytes() );
	}

	return -1;
}

int mswAudioInterface::GetStreamLengthBytes( void )
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		return (int) BASS_ChannelGetLength( channel, BASS_POS_BYTE );
	}

	return -1;
}

void mswAudioInterface::NotifyChannelEventListener( int eventId )
{
	if ( m_callbackListener != NULL )
		xAudioInterface::m_callbackListener->OnChannelEvent( eventId );
}

XA_DEVICEINFO mswAudioInterface::GetDeviceInfo( int deviceNum  )
{
	BASS_INFO *bassInfo = new BASS_INFO;
	if ( !BASS_GetInfo( bassInfo ) )
		return XA_DEVICEINFO();

	XA_DEVICEINFO xaInfo;
	xaInfo.eax				= bassInfo->eax != 0;
	xaInfo.free3DSamples	= bassInfo->free3d;
	xaInfo.freeMem			= bassInfo->hwfree;
	xaInfo.freeSamples		= bassInfo->freesam;
	xaInfo.hardMem			= bassInfo->hwsize;
	xaInfo.maxSampleRate	= bassInfo->maxrate;
	xaInfo.minSampleRate	= bassInfo->minrate;
	xaInfo.speakers			= bassInfo->speakers;

	return xaInfo;
}
