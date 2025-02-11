#include "xaplVolumeSubject.h"

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
