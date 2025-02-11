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

#ifndef __XAPL_AUDIO_EVENT_SUBJECT_H
#define __XAPL_AUDIO_EVENT_SUBJECT_H

#include <list>

#include "xaplAudioEventListener.h"

using std::list;

class xaplAudioEventSubject
{
private:
	list<xaplAudioEventListener *> m_audioEventListeners;

public:
	void AddAudioEventListener( xaplAudioEventListener *observer );
	void RemoveAudioEventListener( xaplAudioEventListener *observer );

	virtual bool NotifyTrackChanged( xaplPlaylistItem *oldTrack, xaplPlaylistItem *newTrack );
	virtual bool NotifyPlayingStatusChanged( int prevStatus, int status, xaplPlaylistItem *currentTrack = NULL );
	virtual bool NotifyVolumeChanged( unsigned char oldVolume, unsigned char newVolume );
	virtual bool NotifyPositionChanged( float oldPosition, float newPosition);
};

#endif
