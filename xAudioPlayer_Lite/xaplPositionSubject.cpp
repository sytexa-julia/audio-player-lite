#include "xaplPositionSubject.h"

void xaplPositionSubject::AddPositionObserver( xaplPositionObserver *observer )
{
	if ( observer == NULL )
		return;
	
	m_positionObservers.push_back( observer );
}

void xaplPositionSubject::RemovePositionObserver( xaplPositionObserver *observer )
{
	if ( observer == NULL )
		return;

	m_positionObservers.remove( observer );
}

bool xaplPositionSubject::NotifyPositionChanged( float newPos )
{
	if ( newPos < 0.0f )
		return false;
	
	list<xaplPositionObserver *>::iterator it = m_positionObservers.begin();
	while ( it != m_positionObservers.end() )
	{
		if ( (*it) != NULL )
			(*it)->OnSongPositionChanged( newPos );

		it++;
	}

	return true;
}
