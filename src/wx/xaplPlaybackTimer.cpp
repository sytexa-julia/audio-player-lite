#include "xaplPlaybackTimer.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

xaplPlaybackTimer::xaplPlaybackTimer( void ) : wxThread()
{ }

xaplPlaybackTimer::~xaplPlaybackTimer( void )
{ }

void *xaplPlaybackTimer::Entry( void )
{
	while ( IsAlive() && !TestDestroy() && !IsPaused() )
	{
		Sleep( 250 );
		Notify();
	}

	return NULL;
}

void xaplPlaybackTimer::Notify( void )
{
	list<xaplTimeObserver *>::iterator it = timeObservers.begin();
	while ( it != timeObservers.end() )
	{
		if ( (*it) != NULL )
			(*it)->OnTimeUpdated();

		it++;
	}
}

void xaplPlaybackTimer::AddTimeObserver( xaplTimeObserver *observer )
{
	if ( observer == NULL )
		return;
	
	timeObservers.push_back( observer );
}

void xaplPlaybackTimer::RemoveTimeObserver( xaplTimeObserver *observer )
{
	if ( observer == NULL )
		return;

	timeObservers.remove( observer );
}
