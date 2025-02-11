#ifndef __MSW_AUDIO_INTERFACE_H
#define __MSW_AUDIO_INTERFACE_H

#include "xAudioInterface.h"
#include "bass.h"

// A copy of the BASS error table, for comparisons 
// against returned error codes
typedef enum
{
	XA_ERROR_UNKNOWN = -1,
	XA_OK = 0,
	XA_ERROR_MEM = 1,
	XA_ERROR_FILEOPEN = 2,
	XA_ERROR_DRIVER = 3,
	XA_ERROR_BUFLOST = 4,
	XA_ERROR_HANDLE = 5,
	XA_ERROR_FORMAT = 6,
	XA_ERROR_POSITION = 7,
	XA_ERROR_INIT = 8,
	XA_ERROR_START = 9,
	XA_ERROR_ALREADY = 14,
	XA_ERROR_NOPAUSE = 16,
	XA_ERROR_NOCHAN = 18,
	XA_ERROR_ILLTYPE = 19,
	XA_ERROR_ILLPARAM = 20,
	XA_ERROR_NO3D = 21,
	XA_ERROR_NOEAX = 22,
	XA_ERROR_DEVICE = 23,
	XA_ERROR_NOPLAY = 24,
	XA_ERROR_FREQ = 25,
	XA_ERROR_NOTFILE = 27,
	XA_ERROR_NOHW = 29,
	XA_ERROR_EMPTY = 31,
	XA_ERROR_NONET = 32,
	XA_ERROR_CREATE = 33,
	XA_ERROR_NOFX = 34,
	XA_ERROR_PLAYING = 35,
	XA_ERROR_NOTAVAIL = 37,
	XA_ERROR_DECODE = 38,
	XA_ERROR_DX = 39,
	XA_ERROR_TIMEOUT = 40,
	XA_ERROR_FILEFORM = 41,
	XA_ERROR_SPEAKER = 42,
	XA_ERROR_VERSION = 43,
	XA_ERROR_CODEC = 44
} mswErrorTable;

/////////////////////////////
// Callback Handling
/////////////////////////////
static HSYNC chanEndSync;
static HSYNC chanStallSync;
static HSYNC chanResumeSync;
static HSYNC chanFreedSync;
static HSYNC chanDlDoneSync;

static void CALLBACK sync_ChanEnd( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanStall( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanResume( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanFreed( HSYNC handle, DWORD channel, DWORD data, void *user );
static void CALLBACK sync_ChanDLDone( HSYNC handle, DWORD channel, DWORD data, void *user );

/////////////////////////////
// Class Definition
/////////////////////////////
class mswAudioInterface :
	public xAudioInterface
{
private:
	DWORD channel;
	HSYNC syncHandle;

public:
	mswAudioInterface( void );
	~mswAudioInterface( void );

	virtual bool LoadFile( const char *fileName, bool startPlay = false );

	virtual bool StartPlayback( void );
	virtual bool PausePlayback( void );
	virtual bool StopPlayback( bool releaseResources = false );
	virtual bool Seek( float position );
	virtual bool SetVolume( float volume );

	virtual double GetCurrentPosition( void );
	virtual void   GetCurrentVolume( float *vol );
	virtual int    GetStreamLength( void );
	virtual int    GetStreamLengthBytes( void );
	static void    NotifyChannelEventListener( int eventID );

	virtual XA_DEVICEINFO GetDeviceInfo( int deviceNum = 0 );
};

#endif
