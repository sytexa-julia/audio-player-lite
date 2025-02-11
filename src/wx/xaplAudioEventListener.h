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

#ifndef __XAPL_AUDIO_EVENT_LISTENER_H
#define __XAPL_AUDIO_EVENT_LISTENER_H

#include "xaplPlaylist.h"

typedef enum
{
	xaplPS_STOPPED,
	xaplPS_PLAYING,
	xaplPS_PLAYING_REPEAT,
	xaplPS_PAUSED
} xaplPlayingStatus;

class xaplAudioEventListener
{
public:
	virtual void OnTrackChanged( xaplPlaylistItem *oldTrack, xaplPlaylistItem *newTrack ) {};
	virtual void OnPlayingStatusChanged( int prevStatus, int status, xaplPlaylistItem *currentTrack = NULL ) {};
	virtual void OnVolumeChanged( unsigned char oldVolume, unsigned char newVolume ) {};
	virtual void OnPositionChanged( float oldPosition, float newPosition) {};
};

#endif
