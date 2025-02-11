#ifndef __X_AUDIO_INTERFACE_H
#define __X_AUDIO_INTERFACE_H

#include <string>
#include "xaiChannelEventListener.h"
using std::string;

typedef int XA_ERROR;
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
	EVENT_CHANNEL_END = 717,
	EVENT_CHANNEL_STALL,
	EVENT_CHANNEL_RESUME,
	EVENT_CHANNEL_FREED, 
	EVENT_DOWNLOAD_DONE
} XA_CALLBACK_EVENTS;

// Important: Not all values will be available on all platforms!
struct XA_DEVICEINFO
{
	int speakers;
	bool eax;
	int hardMem;
	int freeMem;
	int freeSamples;
	int free3DSamples;
	int minSampleRate;
	int maxSampleRate;
};

class xAudioInterface
{
protected:
	char *m_currentPath;
	static xaiChannelEventListener *m_callbackListener;
	XA_STATE m_state;
	XA_ERROR m_lastError;

public:
	virtual bool LoadFile( const char *fileName, bool startPlay = false ) = 0;

	virtual bool StartPlayback( void ) = 0;
	virtual bool PausePlayback( void ) = 0;
	virtual bool StopPlayback( bool releaseResources = false ) = 0;
	virtual bool Seek( float position ) = 0;
	virtual bool SetVolume( float volume ) = 0;

	virtual int    GetState( void ) const { return this->m_state; };
	virtual char  *GetActiveFile( void ) const { return this->m_currentPath; };
	virtual double GetCurrentPosition( void ) = 0;
	virtual void   GetCurrentVolume( float *vol ) = 0;
	virtual int    GetStreamLength( void ) = 0;
	virtual int    GetStreamLengthBytes( void ) = 0;
	virtual void   SetCallbackListener( xaiChannelEventListener *listener ) { this->m_callbackListener = listener; };

	virtual XA_DEVICEINFO GetDeviceInfo( int deviceNum = 0 ) = 0;

	bool Ready( void ) { return this->m_state == STATE_READY; } ;
	XA_ERROR GetLastError( void ) { return this->m_lastError; };
};

#endif
