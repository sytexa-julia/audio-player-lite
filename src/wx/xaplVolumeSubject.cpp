#include "xaplVolumeSubject.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

void xaplVolumeSubject::AddVolumeObserver( xaplVolumeObserver *observer )
{
	if ( observer == NULL )
		return;
	
	volumeObservers.push_back( observer );
}

void xaplVolumeSubject::RemoveVolumeObserver( xaplVolumeObserver *observer )
{
	if ( observer == NULL )
		return;

	volumeObservers.remove( observer );
}

bool xaplVolumeSubject::NotifyVolumeChanged( float newVol )
{
	if ( newVol > 1.0 || newVol < 0.0 )
		return false;
	
	list<xaplVolumeObserver *>::iterator it = volumeObservers.begin();
	while ( it != volumeObservers.end() )
	{
		if ( (*it) != NULL )
			(*it)->OnVolumeChanged( newVol );

		it++;
	}

	return true;
}
