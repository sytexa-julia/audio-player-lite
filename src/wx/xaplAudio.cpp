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

#include "xaplAudio.h"

xaplAudio* xaplAudio::s_instance = NULL;
wxMutex* xaplAudio::s_mutex = new wxMutex;

xaplAudio::xaplAudio( void ) 
	: xaplAudioEventSubject(), 
	  xaiChannelEventListener()
{ 
	// Get list of plugins to load from /plugins/sound directory
	wxArrayString files;
	wxDir::GetAllFiles( PLUGIN_DIR, &files, PLUGIN_SPEC );
	vector<string> plugins( files.GetCount() );

	for ( size_t i = 0; i < files.GetCount(); ++i )
	{
		plugins[i] = string(files[i].mb_str());
	}

	xaplConfiguration *config = wxGetApp().GetConfig();
	m_audioInterface = new xAudioInterface( plugins, config->bufferSize, config->forceSoftwareMixing );
	if ( m_audioInterface == NULL )
	{
		wxMessageBox( wxT("The audio subsystem could not be initialized. Xapl will now exit."), wxT("Xapl Fatal Error") );
		wxGetApp().Exit();
	}
	else
	{
		m_allPlayableExtensions = GetPlayableFileExtensions();

		m_audioInterface->SetCallbackListener( this );
		m_audioInterface->SetVolume( (float) config->volume / 100.0f );
	}
}

xaplAudio* xaplAudio::GetInstance( void )
{
	wxMutexLocker locker( *s_mutex );

	if ( s_instance == NULL )
		s_instance = new xaplAudio;

	return s_instance;
}

void xaplAudio::SetPlaylist( xaplPlaylist* playlist )
{
	m_playlist = playlist;
}

void xaplAudio::OnChannelEvent( int eventID )
{
	if ( m_audioInterface->GetLastError() != 0 || m_audioInterface->GetState() != STATE_PLAYING ) 
		return;

	if ( eventID == EVENT_MIXER_END )
	{
		// Get next song and load it (but don't play it until the end of the current track 
		// is heard).
		wxMutexGuiEnter();
			m_nextTrackId = m_playlist->Next( false );
			xaplPlaylistItem *data = m_playlist->GetItemById( m_nextTrackId );
			if ( data != NULL )
				m_audioInterface->LoadNext( string( data->path.mb_str() ) );
			else
				wxMessageBox( wxT("The next playlist item could not be played."), wxT("Xapl"), wxICON_ERROR | wxOK );
		wxMutexGuiLeave();
	}
	else if ( eventID == EVENT_MIXER_END_HEARD )
	{
		wxMutexGuiEnter();
			xaplPlaylistItem *oldTrack = m_playlist->GetPlayingItemData();
			m_playlist->SetPlayingItem( m_nextTrackId );
			NotifyTrackChanged( oldTrack, m_playlist->GetPlayingItemData() );
		wxMutexGuiLeave();
	}
}

const wxArrayString xaplAudio::GetPlayableFileExtensions( void )
{
	vector<string> exts = m_audioInterface->GetPlayableFileExtensions();
	wxArrayString retExts;

	for ( vector<string>::iterator it = exts.begin(); it != exts.end(); it++ )
	{
		retExts.Add( wxString( (*it).c_str(), wxConvUTF8 ) );
	}

	return retExts;
}

const wxString xaplAudio::GetCompletePlayableFileSpec( void )
{
	wxString ret = wxString( m_audioInterface->GetCompletePlayableFileSpec().c_str(), wxConvUTF8 );
	wxMessageBox( ret );
	return ret;
}

bool xaplAudio::IsExtensionPlayable( const wxString &ext )
{
	return m_allPlayableExtensions.Index( ext ) != wxNOT_FOUND;
}

bool xaplAudio::Play( int fadeInTime )
{
	if ( m_playlist == NULL || m_audioInterface == NULL )
		return false;

	// Just unpause if paused and the playing track hasn't changed
	int oldState = GetPlaybackState();
	if ( oldState == xaplPS_PAUSED && m_currentTrackId == m_playlist->GetPlayingItem() )
	{
		m_audioInterface->StartPlayback();
		NotifyPlayingStatusChanged( oldState, xaplPS_PLAYING, m_playlist->GetPlayingItemData() );
		return true;
	}

	// Find a playable song
	wxProgressDialog *dlgProgress = NULL;
	m_currentTrackId = m_playlist->GetPlayingItem();
	wxString audioFile = m_playlist->GetPlayingItemData()->path;
	bool check = false;
	size_t badCnt = 0;
	
	check = m_audioInterface->LoadFile( string( audioFile.mb_str(wxConvLibc) ), true );

	// If the song is invalid, display a dialog notifying the user that we are seaching
	// for a valid song... since if one track is bad, it may be a disconnected network
	// drive (making many of the songs bad...)
	if ( !check )
	{
		dlgProgress = new wxProgressDialog( wxT("Please wait..."), wxT("Locating next valid track..."), -1 );
		dlgProgress->Show();
	}
	
	// Loop until we find a song that is valid, or all tracks are found to be invalid
	while ( !check && badCnt < m_playlist->GetPlaylistItemCount() )
	{
		// Bad song. Flag it, and skip it.
		m_playlist->SetItemOk( m_playlist->GetPlayingItem(), false );
		++badCnt;

		// Check the next song
		m_currentTrackId = m_playlist->Next();
		if ( m_playlist->GetPlayingItem() == xaplNO_ITEM )
		{
			badCnt = m_playlist->GetPlaylistItemCount();
			break;
		}

		audioFile = m_playlist->GetPlayingItemData()->path;
		check = m_audioInterface->LoadFile( string( audioFile.mb_str(wxConvLibc) ), true );
	}

	// Clean up the dialog, if it was used
	if ( dlgProgress != NULL )
	{
		delete dlgProgress;
	}

	// All items are invalid...
	if ( badCnt == m_playlist->GetPlaylistItemCount() )
	{
		m_currentTrackId = xaplNO_TRACK;
		wxMessageBox( wxT("This playlist does not contain any valid items!\n\nIf your files are on the network, consider checking your connection."), wxT("Xapl") );
		return false;
	}

	// Notify audio event listeners
	NotifyPlayingStatusChanged( oldState, xaplPS_PLAYING, m_playlist->GetPlayingItemData() );
	NotifyTrackChanged( NULL, m_playlist->GetItemById( m_playlist->GetPlayingItem() ) );
	
	return true;
}

bool xaplAudio::Pause( void )
{
	if ( m_audioInterface == NULL )
		return false;

	int oldState = GetPlaybackState();
	bool result = m_audioInterface->PausePlayback();
	NotifyPlayingStatusChanged( oldState, xaplPS_PAUSED );
	return result;
}

bool xaplAudio::Stop( int fadeOutTime )
{
	if ( m_audioInterface == NULL )
		return false;

	int oldState = GetPlaybackState();
	bool result = false;
	if ( fadeOutTime > 0 )
		result = m_audioInterface->StopWithFadeOut( fadeOutTime );
	else
		result = m_audioInterface->StopPlayback();

	m_currentTrackId = xaplNO_TRACK;
	NotifyPlayingStatusChanged( oldState, xaplPS_STOPPED );
	return result;
}

bool xaplAudio::Next( void )
{
	if ( m_playlist->GetPlaylistItemCount() == 0 ) 
		return false;
	
	m_playlist->Next();
	return Play();
}

bool xaplAudio::Previous( void )
{
	if ( m_playlist->GetPlaylistItemCount() == 0 ) 
		return false;
	
	m_playlist->Previous();
	return Play();
}

bool xaplAudio::Seek( float position )
{
	if ( m_audioInterface == NULL )
		return false;

	double oldPos = m_audioInterface->GetCurrentPosition();
	bool result = m_audioInterface->Seek( position );
	NotifyPositionChanged( oldPos, position );
	return result;
}

bool xaplAudio::Seek( int direction, int amount )
{
	if ( m_audioInterface == NULL )
		return false;

	int mod;
	switch ( amount )
	{
	case xaplSF_SMALL:
		mod = 2;
		break;
	case xaplSF_LARGE:
		mod = 50;
		break;
	case xaplSF_MEDIUM:
	default:
		mod = 20;
		break;
	}

	double pos = m_audioInterface->GetCurrentPosition();
	bool result = false;
	if ( direction == xaplSF_FORWARD )
		result = m_audioInterface->Seek( (float) (pos + mod) );
	else
		result = m_audioInterface->Seek( (float) (pos - mod) );

	NotifyPositionChanged( pos, m_audioInterface->GetCurrentPosition() );
	
	return result;
}

bool xaplAudio::SetVolume( unsigned char volume )
{
	if ( m_audioInterface == NULL )
		return false;

	float fVol;
	unsigned char oldVol;
	m_audioInterface->GetCurrentVolume( &fVol );
	oldVol = (unsigned char) (fVol * 100.0f);

	fVol = (float) (volume / 100.0f);
	bool result = m_audioInterface->SetVolume( fVol );
	NotifyVolumeChanged( oldVol, volume );
	
	return result;
}

unsigned char xaplAudio::GetVolume( void )
{
	if ( m_audioInterface == NULL )
		return 100;

	float fVol;
	m_audioInterface->GetCurrentVolume( &fVol );
	return (unsigned char) (fVol * 100.0f);
}

double xaplAudio::GetTrackPosition( void )
{
	if ( m_audioInterface == NULL )
		return 0.0;

	return m_audioInterface->GetCurrentPosition();
}

int	xaplAudio::GetTrackLength( void )
{
	if ( m_audioInterface == NULL )
		return 0;

	return m_audioInterface->GetStreamLength();
}

wxString xaplAudio::GetTrackInfo( void )
{
	if ( m_audioInterface == NULL )
		return wxEmptyString;

	return wxString( m_audioInterface->GetActiveFileInfoString().c_str(), wxConvUTF8 );
}

int xaplAudio::GetPlaybackState( void )
{
	if ( m_audioInterface == NULL )
		return xaplPS_STOPPED;

	switch ( m_audioInterface->GetState() )
	{
	case STATE_PLAYING:
		return xaplPS_PLAYING;
	case STATE_PAUSED:
		return xaplPS_PAUSED;
	}

	return xaplPS_STOPPED;
}

xaplAudio::~xaplAudio( void )
{
	delete m_audioInterface;
	delete s_instance;
	delete s_mutex;
}
