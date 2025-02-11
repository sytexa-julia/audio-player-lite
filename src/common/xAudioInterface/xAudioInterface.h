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

#ifndef __X_AUDIO_INTERFACE_H
#define __X_AUDIO_INTERFACE_H

typedef int XA_STATE;

typedef enum
{
	STATE_PLAYING = 391,
	STATE_PAUSED,
	STATE_STOPPED,
	STATE_FILELOADED,
	STATE_READY,
	STATE_FREE
} XA_STATES;

typedef enum
{
	EVENT_MIXER_END = 716,
	EVENT_MIXER_END_HEARD,
	EVENT_SLIDE_DONE,
	EVENT_CHANNEL_START,
	EVENT_CHANNEL_STALL,
	EVENT_CHANNEL_RESUME,
	EVENT_CHANNEL_FREED, 
	EVENT_DOWNLOAD_DONE
} XA_CALLBACK_EVENTS;

#ifdef __OBJC__ // Objective-C class
	#import "../../../lib/include/bass/bass.h"
	#import "../../../lib/include/bass/bassmix.h"

	#import "xaiChannelEventListener.h"

	@interface XAudioInterface : NSObject
	{
		@private
		NSArray* m_pluginHandles;
		DWORD m_buffer;
		BOOL m_forceSoftware;

		HSTREAM channelA;
		HSTREAM channelB;
		HSYNC syncHandle;
		
		@protected
		NSString* m_currentPath;
		NSString* m_infoStr;
		XA_STATE m_state;
		int m_lastError;
		BOOL m_infoStale;
	}

	- (id) init;
	- (id) initWithBufferSize:(int) buffer forceSoftwareMixing:(BOOL)forceSoftwareMixing;
	- (id) initWithPlugins:(NSArray*)plugins buffer: (int)buffer forceSoftwareMixing: (BOOL)forceSoftwareMixing;
	- (void) dealloc;	
	- (BOOL) _initializeWithPlugins:(NSArray*)plugins buffer: (int)buffer forceSoftwareMixing: (BOOL)forceSoftwareMixing;	

	- (NSArray*) allPlayableFileExtensions;

	- (BOOL) loadFile: (NSString*)fileName;
	- (BOOL) loadFile: (NSString*)fileName andStartPlaying: (BOOL) startPlay;
	- (BOOL) loadNext: (NSString*)fileName;

	- (BOOL) startPlayback;
	- (BOOL) startPlaybackWithFadeIn: (int)fadeInTimeMillis;

	- (BOOL) pausePlayback;
	- (BOOL) stopPlayback;
	- (BOOL) stopPlaybackWithFadeOut: (int)fadeOutTimeMillis;

	- (BOOL) seekToPosition: (float) position;
	- (BOOL) setVolume: (float) volume;
	
	- (double) currentPosition;
	- (long long) currentPositionInBytes;
	- (void) volume: (float*) volume;
	- (int) streamLength;
	- (long long) streamLengthInBytes;
	- (NSString*) activeFile;
	- (NSString*) activeFileInfoString;
	- (BOOL) getDeviceInfo: (BASS_INFO*) info;
	- (XA_STATE) state;
	- (int) lastError;
	- (BOOL) isReady;
	
	- (void) setChannelEventListener: (id <XChannelEventListener>) listener;
	- (void) _recalcInfoString;
	@end

#else // C++ class
	#include "../../../lib/include/bass/bass.h"
	#include "../../../lib/include/bass/bassmix.h"

	#include <iterator>
	#include <vector>
	#include <iostream>
	#include <sstream>
	#include <string>

	#include <stdio.h>
	#include <stdlib.h>
	#include <time.h>
	#include <string.h>
	#include <limits.h>

	#include "xaiChannelEventListener.h"

	using namespace std;

	class xAudioInterface
	{
	private:
		static xaiChannelEventListener *m_callbackListener;
	
		static HSTREAM s_mixer;
		HSTREAM channelA;
		HSTREAM channelB;

		static HSYNC mixerEndSync;
		static HSYNC mixerEndHeardSync;
		static HSYNC mixerSlideDoneSync;
		static HSYNC chanStartSync;
		static HSYNC chanStallSync;
		static HSYNC chanResumeSync;
		static HSYNC chanFreedSync;
		static HSYNC chanDlDoneSync;
		static char s_activeChannel;
		static bool s_randSeeded;

		vector<HPLUGIN> m_pluginHandles;
		DWORD m_buffer;
		bool m_forceSoftware;

		bool Initialize( vector<string> plugins, DWORD buffer, bool forceSoftwareMixing );
		void Shutdown( void );
		
	protected:
		string m_currentPath;
		string m_infoStr;
		
		XA_STATE m_state;
		int		 m_lastError;
		bool	 m_infoStale;

		void RecalcInfoString( void );

	public:
		xAudioInterface( DWORD buffer = 3000, bool forceSoftwareMixing = false );
		xAudioInterface( vector<string> plugins, DWORD buffer = 3000, bool forceSoftwareMixing = false );
		~xAudioInterface( void );

		vector<string> GetPlayableFileExtensions( void );
		const string GetCompletePlayableFileSpec( void );

		bool LoadFile( const string &path, bool startPlay = false );
		bool LoadNext( const string &path );

		bool StartPlayback( void );
		bool StartWithFadeIn( unsigned long fadeTimeMillis = 2500 );

		bool PausePlayback( void );
		bool StopPlayback( void );
		bool StopWithFadeOut( unsigned long fadeTimeMillis = 2500 );

		bool Seek( float position );
		bool SetVolume( float volume );

		double		  GetCurrentPosition( void );
		long long	  GetCurrentPositionBytes( void );
		void		  GetCurrentVolume( float *vol );
		int			  GetStreamLength( void );
		long long	  GetStreamLengthBytes( void );
		const string &GetActiveFile( void ) const;
		string &	  GetActiveFileInfoString( void );
		bool		  GetDeviceInfo( int deviceNum, BASS_INFO *out );
		int			  GetState( void ) const;
		int			  GetLastError( void );
		bool		  IsReady( void );

		static void	 NotifyChannelEventListener( int eventID );
		void		 SetCallbackListener( xaiChannelEventListener *listener );
		
		static void CALLBACK sync_MixerEnd( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_MixerEndHeard( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_MixerSlideDone( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_ChanStart( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_ChanStall( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_ChanResume( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_ChanFreed( HSYNC handle, DWORD channel, DWORD data, void *user );
		static void CALLBACK sync_ChanDLDone( HSYNC handle, DWORD channel, DWORD data, void *user );
	};

#endif

#endif
