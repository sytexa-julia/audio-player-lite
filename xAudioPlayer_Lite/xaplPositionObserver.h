#ifndef __XAPL_POSITION_OBSERVER_H
#define __XAPL_POSITION_OBSERVER_H

class xaplPositionObserver
{
public:
	virtual void OnSongPositionChanged( float newPos ) = 0;
};

#endif
