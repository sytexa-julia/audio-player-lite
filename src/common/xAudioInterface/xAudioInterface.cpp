/* 
  Copyright (c) 2009, Xive Software
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without modification, 
  are permitted provided that the following conditions are met:

     * Redistributions of source code must retain the above copyright notice, this 
	   list of conditions and the following disclaimer.
     * Redistributions in binary form must reproduce the above copyright notice, 
	   this list of conditions and the following disclaimer in the documentation 
	   and/or other materials provided with the distribution.
     * Neither the name of Xive Software nor the names of its contributors 
	   may be used to endorse or promote products derived from this software 
	   without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED 
  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH 
  DAMAGE.
*/

#include "xAudioInterface.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

// Define the static members so C++ doesn't have a conniption
xaiChannelEventListener *xAudioInterface::m_callbackListener;

HSTREAM xAudioInterface::s_mixer;

HSYNC xAudioInterface::mixerEndSync;
HSYNC xAudioInterface::mixerEndHeardSync;
HSYNC xAudioInterface::mixerSlideDoneSync;
HSYNC xAudioInterface::chanStartSync;
HSYNC xAudioInterface::chanStallSync;
HSYNC xAudioInterface::chanResumeSync;
HSYNC xAudioInterface::chanFreedSync;
HSYNC xAudioInterface::chanDlDoneSync;
char xAudioInterface::s_activeChannel = 'A';
bool xAudioInterface::s_randSeeded = false;

/* ====================================================================
 * ==== Private Helpers ===============================================*/
bool xAudioInterface::Initialize( std::vector<std::string> plugins, 
								  DWORD buffer, 
								  bool forceSoftwareMixing )
{
	// Free old BASS instance
	if ( m_state != STATE_FREE )
	{
		Shutdown();
	}

	// Make sure BASS library version is correct
	if ( HIWORD(BASS_GetVersion()) != BASSVERSION ) 
	{
		m_lastError = BASS_ERROR_VERSION;
		m_state = STATE_STOPPED;
		return false;
	}

	// Fix sound skipping issues on Linux
	#ifdef linux
	  BASS_SetConfig( BASS_CONFIG_DEV_BUFFER, 100 );
	#endif

	// Set the audio buffer size
	BASS_SetConfig( BASS_CONFIG_BUFFER, m_buffer );

	// Initialize BASS
	if ( !BASS_Init( 1, 44100, 0, 0, NULL ) )
	{
		m_lastError = BASS_ErrorGetCode();
		m_state = STATE_FREE;
		return false;
	}

	// Load BASS plugins
	if ( plugins.size() > 0 )
	{
		for ( vector<string>::iterator it = plugins.begin(); it != plugins.end(); it++ )
		{
			HPLUGIN result = BASS_PluginLoad( it->c_str(), 0 );

			if ( result != NULL )
				m_pluginHandles.push_back( result );
		}
	}

	// Initialize mixer channel and syncs
	DWORD flags = m_forceSoftware ? BASS_MIXER_END | BASS_SAMPLE_SOFTWARE : BASS_MIXER_END;
	s_mixer = BASS_Mixer_StreamCreate( 44100, 2, flags );
	mixerEndSync = BASS_ChannelSetSync( s_mixer, BASS_SYNC_END | BASS_SYNC_MIXTIME, 0, sync_MixerEnd, 0 );
	mixerEndHeardSync = BASS_ChannelSetSync( s_mixer, BASS_SYNC_END, 0, sync_MixerEndHeard, 0 );
	mixerSlideDoneSync = BASS_ChannelSetSync( s_mixer, BASS_SYNC_END, 0, sync_MixerSlideDone, 0 );

	// Ready to go!
	m_state = STATE_READY;
	m_currentPath = "";
	m_lastError = BASS_OK;

	return true;
}

void xAudioInterface::Shutdown( void )
{
	m_state = STATE_FREE;

	BASS_ChannelRemoveSync( s_mixer, mixerEndSync );
	BASS_ChannelRemoveSync( s_mixer, mixerEndHeardSync );
	BASS_ChannelRemoveSync( s_mixer, mixerSlideDoneSync );

	BASS_StreamFree( channelA );
	BASS_StreamFree( channelB );
	BASS_StreamFree( s_mixer );
	BASS_Free();
	BASS_PluginFree( 0 );
}

std::vector<std::string> &split( const std::string &s, char delim, std::vector<std::string> &elems ) 
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split( const std::string &s, char delim ) 
{
    std::vector<std::string> elems;
    return split(s, delim, elems);
}

/* ====================================================================
 * ==== Protected Helpers =============================================*/
void xAudioInterface::RecalcInfoString( void )
{
	std::stringstream infoStream;
	m_infoStr = "";

	BASS_CHANNELINFO info;
	DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
	BASS_ChannelGetInfo( chan, &info );

	// Get container type
	switch ( info.ctype )
	{
	case BASS_CTYPE_STREAM_OGG:
		infoStream << "Ogg Vorbis  ";
		break;
	case BASS_CTYPE_STREAM_MP1:
		infoStream << "MPEG-1  ";
		break;
	case BASS_CTYPE_STREAM_MP2:
		infoStream << "MPEG-2  ";
		break;
	case BASS_CTYPE_STREAM_MP3:
		infoStream << "MPEG-3  ";
		break;
	case BASS_CTYPE_STREAM_AIFF:
		infoStream << "AIFF  ";
		break;
	case BASS_CTYPE_STREAM_WAV_PCM:
		infoStream << "Integer PCM WAVE  ";
		break;
	case BASS_CTYPE_STREAM_WAV_FLOAT:
		infoStream << "Floating-Point PCM WAVE  ";
		break;
	case BASS_CTYPE_STREAM_WAV:
		infoStream << "WAVE  ";
		break;
	case BASS_CTYPE_STREAM_FLAC:
		infoStream << "FLAC  ";
		break;
	case BASS_CTYPE_STREAM_FLAC_OGG:
		infoStream << "Ogg FLAC  ";
		break;
	case BASS_CTYPE_STREAM_WMA:
		infoStream << "WMA  ";
		break;
	case BASS_CTYPE_STREAM_WMA_MP3:
		infoStream << "MPEG-3 WMA  ";
		break;
	case BASS_CTYPE_STREAM_AAC:
		infoStream << "AAC  ";
		break;
	case BASS_CTYPE_STREAM_MP4:
		infoStream << "MPEG-4  ";
		break;
	}

	// Get frequency
	QWORD len = BASS_StreamGetFilePosition( chan, BASS_FILEPOS_END );
	double time = BASS_ChannelBytes2Seconds( chan, BASS_ChannelGetLength(chan, BASS_POS_BYTE) );
	DWORD kbps = (DWORD) ( len / (125 * time) + 0.5 );

	infoStream << kbps << " kbps  ";

	// Get channels
	switch ( info.chans )
	{
	case 1:
		infoStream << "Mono";
		break;
	case 2:
		infoStream << "Stereo";
		break;
	default:
		infoStream << "(" << info.chans << " Channels)";
		break;
	}

	m_infoStr = infoStream.str();
}

/* ====================================================================
 * ==== Constructors / Destructor =====================================*/
xAudioInterface::xAudioInterface( DWORD buffer, 
								  bool forceSoftwareMixing ) 
	: m_buffer( buffer ), 
	  m_forceSoftware( forceSoftwareMixing ), 
	  m_infoStale( false ), 
	  m_state( STATE_FREE )
{
	vector<string> noPlugins;
	Initialize( noPlugins, buffer, forceSoftwareMixing );
}

xAudioInterface::xAudioInterface( std::vector<std::string> plugins, 
								  DWORD buffer, 
								  bool forceSoftwareMixing )
	: m_buffer( buffer ),
	  m_forceSoftware( forceSoftwareMixing ), 
	  m_infoStale( false ), 
	  m_state( STATE_FREE )
{
	Initialize( plugins, buffer, forceSoftwareMixing );
}

xAudioInterface::~xAudioInterface( void )
{	
	Shutdown();
}

/* ====================================================================
 * ==== Misc. =========================================================*/
vector<string> xAudioInterface::GetPlayableFileExtensions( void )
{
	vector<string> exts;
	
	// Add BASS built-in formats
	exts.push_back( "*.wav" );
	exts.push_back( "*.aif" );
	exts.push_back( "*.aiff" );
	exts.push_back( "*.mp1" );
	exts.push_back( "*.mp2" );
	exts.push_back( "*.mp3" );
	exts.push_back( "*.ogg" );

	// Get plugin supported formats
	for ( vector<HPLUGIN>::iterator it = m_pluginHandles.begin(); it != m_pluginHandles.end(); it++ )
	{
		const BASS_PLUGININFO *pluginInfo = BASS_PluginGetInfo( *it );
		for ( size_t i = 0; i < pluginInfo->formatc; ++i )
		{
			const BASS_PLUGINFORM pluginForm = pluginInfo->formats[i];
			split( pluginForm.exts, ';', exts );
		}
	}

	return exts;
}

const string xAudioInterface::GetCompletePlayableFileSpec( void )
{
	string fileSpec = "MPEG Audio Files (*.mp1,*.mp2;*.mp3)|*.mp1;*.mp2;*.mp3|Waveform Audio Files (*.wav)|*.wav|Ogg Vorbis Audio Files (*.ogg)|*.ogg|Audio Interchange Files (*.aif;*.aiff)|*.aif;*.aiff|";
	string allFormats = "*.mp1;*.mp2;*.mp3;*.wav;*.ogg;*.aif;*.aiff;";

	for ( vector<HPLUGIN>::iterator it = m_pluginHandles.begin(); it != m_pluginHandles.end(); it++ )
	{
		const BASS_PLUGININFO *pluginInfo = BASS_PluginGetInfo( *it );
		for ( size_t i = 0; i < pluginInfo->formatc; ++i )
		{
			const BASS_PLUGINFORM pluginForm = pluginInfo->formats[i];
			fileSpec += string(pluginForm.name) + " (" + string(pluginForm.exts) + ")" + "|" + string(pluginForm.exts) + "|";
			allFormats += string(pluginForm.exts) + ";";
		}
	}

	fileSpec.insert( 0, "All Supported Files|" + allFormats + "|" );
	fileSpec.erase( fileSpec.length() - 1 );

	return fileSpec;
}

/* ====================================================================
 * ==== File Loading ==================================================*/
bool xAudioInterface::LoadFile( const string &path, 
							    bool startPlay )
{
	if ( m_state == STATE_FREE || path.empty() )
		return false;

	m_state = STATE_STOPPED;
	m_currentPath = path;

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
	*newChan = BASS_StreamCreateFile( false, m_currentPath.c_str(), 0, 0, flags );
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
		return false;

	if ( startPlay )
	{
		if ( !BASS_ChannelPlay( s_mixer, false ) )
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

bool xAudioInterface::LoadNext( const string &path )
{
	if ( path.empty() )
		return false;

	m_currentPath = path;

	HSTREAM *oldChan = s_activeChannel == 'A' ? &channelA : &channelB;
	HSTREAM *newChan = s_activeChannel == 'A' ? &channelB : &channelA;

	BASS_ChannelStop( s_mixer );

	BASS_ChannelRemoveSync( *oldChan, chanStartSync );
	BASS_ChannelRemoveSync( *oldChan, chanStallSync );
	BASS_ChannelRemoveSync( *oldChan, chanResumeSync );
	BASS_ChannelRemoveSync( *oldChan, chanFreedSync );
	BASS_ChannelRemoveSync( *oldChan, chanDlDoneSync );

	DWORD flags = m_forceSoftware ? BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT | BASS_SAMPLE_SOFTWARE : BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT;
	*newChan = BASS_StreamCreateFile( false, m_currentPath.c_str(), 0, 0, flags );
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
		return false;
	}
	
	return true;
}

/* ====================================================================
 * ==== Starting Playback =============================================*/
bool xAudioInterface::StartPlayback( void )
{
	if ( m_state == STATE_PAUSED || m_state == STATE_FILELOADED )
	{
		BASS_ChannelPlay( s_mixer, false );
		m_state = STATE_PLAYING;
		return true;
	}
	else if ( m_state == STATE_PLAYING )
	{
		return true;
	}

	return false;
}

bool xAudioInterface::StartWithFadeIn( unsigned long fadeTimeMillis )
{
	if ( m_state == STATE_PLAYING )
		return false;

	SetVolume( 0.0f );

	if ( BASS_ChannelIsSliding( s_mixer, BASS_ATTRIB_VOL ) )
		return false;

	return BASS_ChannelSlideAttribute( s_mixer, BASS_ATTRIB_VOL, 1.0f, fadeTimeMillis ) 
		&& BASS_ChannelPlay( s_mixer, false );
}

/* ====================================================================
 * ==== Stopping Playback =============================================*/
bool xAudioInterface::PausePlayback( void )
{
	if ( m_state == STATE_PLAYING )
	{
		if ( BASS_ChannelPause( s_mixer ) )
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

bool xAudioInterface::StopPlayback( void )
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		if ( BASS_ChannelStop( s_mixer ) )
		{
			m_state = STATE_STOPPED;
			return true;
		}
	}

	return false;
}

bool xAudioInterface::StopWithFadeOut( unsigned long fadeTimeMillis )
{
	if ( m_state != STATE_PLAYING )
		return false;

	if ( BASS_ChannelIsSliding( s_mixer, BASS_ATTRIB_VOL ) )
		return false;

	BOOL result = BASS_ChannelSlideAttribute( s_mixer, BASS_ATTRIB_VOL, 0.0f, fadeTimeMillis );
	if ( result )
		return true;

	return false;
}

/* ====================================================================
 * ==== Set Playback Position / Volume ================================*/
bool xAudioInterface::Seek( float position )
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		QWORD newPos = BASS_ChannelSeconds2Bytes( chan, position );
		if ( BASS_Mixer_ChannelSetPosition( chan, newPos, BASS_POS_BYTE ) &&
			BASS_ChannelSetPosition( s_mixer, 0, BASS_POS_BYTE ) )
		{
			return true;
		}
	}
	
	return false;
}

bool xAudioInterface::SetVolume( float volume )
{
	if ( volume > 1.0 || volume < 0.0 )
		return false;

	BASS_ChannelSetAttribute( s_mixer, BASS_ATTRIB_VOL, volume );
	return true;
}

/* ====================================================================
 * ==== Getters =======================================================*/
double xAudioInterface::GetCurrentPosition( void )
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		QWORD currPos = BASS_Mixer_ChannelGetPosition( chan, BASS_POS_BYTE );
		return BASS_ChannelBytes2Seconds( chan, currPos );
	}

	return -1.0;
}

long long xAudioInterface::GetCurrentPositionBytes( void )
{
	if ( m_state == STATE_PLAYING || m_state == STATE_PAUSED )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		QWORD currPos = BASS_Mixer_ChannelGetPosition( chan, BASS_POS_BYTE );
		return currPos;
	}

	return -1;
}
	
void xAudioInterface::GetCurrentVolume( float *vol )
{
	if ( vol != NULL )
		BASS_ChannelGetAttribute( s_mixer, BASS_ATTRIB_VOL, vol );
}

int xAudioInterface::GetStreamLength( void )
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		return (int) BASS_ChannelBytes2Seconds( chan, GetStreamLengthBytes() );
	}

	return -1;
}

long long xAudioInterface::GetStreamLengthBytes( void )
{
	if ( m_state != STATE_STOPPED && m_state != STATE_FREE )
	{
		DWORD chan = s_activeChannel == 'A' ? channelA : channelB;
		return BASS_ChannelGetLength( chan, BASS_POS_BYTE );
	}

	return -1;
}
const string &xAudioInterface::GetActiveFile( void ) const 
{ 
	return this->m_currentPath; 
}

string &xAudioInterface::GetActiveFileInfoString( void ) 
{ 
	if ( m_infoStale ) 
	{ 
		RecalcInfoString(); 
		m_infoStale = false; 
	} 
	
	return this->m_infoStr; 
}

bool xAudioInterface::GetDeviceInfo( int deviceNum, 
									 BASS_INFO *out )
{
	if ( out == NULL || !BASS_GetInfo( out ) )
		return false;

	return true;
}

int xAudioInterface::GetState( void ) const 
{
	return this->m_state;
}

int xAudioInterface::GetLastError( void ) 
{ 
	return this->m_lastError; 
}

bool xAudioInterface::IsReady( void ) 
{ 
	return this->m_state == STATE_READY; 
}

/* ====================================================================
 * ==== Channel Event Listener ========================================*/
void xAudioInterface::SetCallbackListener( xaiChannelEventListener *listener ) 
{ 
	this->m_callbackListener = listener;
}

void xAudioInterface::NotifyChannelEventListener( int eventId )
{
	if ( m_callbackListener != NULL )
		xAudioInterface::m_callbackListener->OnChannelEvent( eventId );
}

/* ====================================================================
 * ==== BASS Sync Callbacks ===========================================*/
void CALLBACK xAudioInterface::sync_MixerEnd( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	xAudioInterface::NotifyChannelEventListener( EVENT_MIXER_END );
}

void CALLBACK xAudioInterface::sync_MixerEndHeard( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	s_activeChannel = s_activeChannel == 'A' ? 'B' : 'A';
	BASS_ChannelPlay( s_mixer, TRUE );
	xAudioInterface::NotifyChannelEventListener( EVENT_MIXER_END_HEARD );
}

void CALLBACK xAudioInterface::sync_MixerSlideDone( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	xAudioInterface::NotifyChannelEventListener( EVENT_SLIDE_DONE );
}

void CALLBACK xAudioInterface::sync_ChanStart( HSYNC handle, DWORD channel, DWORD data, void *user )
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

	xAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_START );
}

void CALLBACK xAudioInterface::sync_ChanStall( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	xAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_STALL );
}

void CALLBACK xAudioInterface::sync_ChanResume( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	xAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_RESUME );
}

void CALLBACK xAudioInterface::sync_ChanFreed( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	xAudioInterface::NotifyChannelEventListener( EVENT_CHANNEL_FREED );
}

void CALLBACK xAudioInterface::sync_ChanDLDone( HSYNC handle, DWORD channel, DWORD data, void *user )
{
	xAudioInterface::NotifyChannelEventListener( EVENT_DOWNLOAD_DONE );
}
