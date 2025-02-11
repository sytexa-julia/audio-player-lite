#ifndef __XAPL_POSITION_SUBJECT_H
#define __XAPL_POSITION_SUBJECT_H

#include <list>
#include "xaplPositionObserver.h"

using std::list;

class xaplPositionSubject
{
private:
	list<xaplPositionObserver *> m_positionObservers;

public:
	void AddPositionObserver( xaplPositionObserver *observer );
	void RemovePositionObserver( xaplPositionObserver *observer );
	virtual bool NotifyPositionChanged( float newPos );
};

#endif
