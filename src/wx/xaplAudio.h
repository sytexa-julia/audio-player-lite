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

#ifndef __XAPL_AUDIO_H
#define __XAPL_AUDIO_H

#include <wx/thread.h>
#include <wx/string.h>
#include <wx/progdlg.h>

#include "../common/xAudioInterface/xaiChannelEventListener.h"
#include "../common/xAudioInterface/xAudioInterface.h"
#include <wx/msw/winundef.h>	// Fix conflict between wxWidgets and windows.h include in xAudioInterface

#include "xaplAudioEventSubject.h"
#include "xaplApplication.h"
#include "xaplPlaylist.h"

typedef enum
{
	xaplSF_FORWARD, 
	xaplSF_BACKWARD,
	xaplSF_SMALL,
	xaplSF_MEDIUM,
	xaplSF_LARGE
} xaplSeekFlags;

#define xaplNO_FADE -1
#define xaplNO_TRACK -1

class xaplAudio
	: public xaplAudioEventSubject,
	  public xaiChannelEventListener
{
private:
	static xaplAudio* s_instance;
	static wxMutex*	  s_mutex;
	xAudioInterface*  m_audioInterface;
	xaplPlaylist*	  m_playlist;

	long			  m_currentTrackId;
	long			  m_nextTrackId;

	wxArrayString	  m_allPlayableExtensions;

protected:
	// Constructor.
	xaplAudio( void );

public:
	// Gets the global instance of xaplAudio.
	static xaplAudio* GetInstance( void );

	// Set the playlist to get track information from.
	void SetPlaylist( xaplPlaylist* playlist );
	
	// Handles channel events from the audio library. Don't call this manually.
	void OnChannelEvent( int eventID );

	// Important Information
	// Gets a list of extensions that can be played. Each entry is of the form *.ext
	const wxArrayString GetPlayableFileExtensions( void );
	// Gets a complete filespec for the list of playable file extensions
	const wxString GetCompletePlayableFileSpec( void );
	// Is this extension playable? ext should be of the form *.ext
	bool IsExtensionPlayable( const wxString &ext );

	// Operations
	// Start playing. Use SetPlayingItem() on the playlist to set the track to play.
	bool Play( int fadeInTime = xaplNO_FADE );
	// Pause playback.
	bool Pause( void );
	// Stop playback.
	bool Stop( int fadeOutTime = xaplNO_FADE );
	// Skip to the next song in the playlist.
	bool Next( void );
	// Skip to the previous song in the playlist.
	bool Previous( void );
	// Move to the specified position in the playing track. 
	bool Seek( float position );
	// Seek by a pre-defined amount
	bool Seek( int direction = xaplSF_FORWARD, int amount = xaplSF_MEDIUM );
	// Set the output volume.
	bool SetVolume( unsigned char volume );

	// Getters
	// Get the output volume.
	unsigned char GetVolume( void );
	// Get the current playback position in the track in seconds.
	double		  GetTrackPosition( void );
	// Get the length of the playing track in seconds.
	int			  GetTrackLength( void );
	// Get a string containing the format, bitrate, and # of channels of the playing track.
	wxString	  GetTrackInfo( void );
	// Get the playback state.
	int			  GetPlaybackState( void );

	// Destructor
	~xaplAudio( void );
};

#endif
