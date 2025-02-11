#include "xaplPositionSubject.h"

#ifdef _DEBUG
#include <crtdbg.h>
#define DEBUG_NEW new(_NORMAL_BLOCK ,__FILE__, __LINE__)
#else
#define DEBUG_NEW new
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif 

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
