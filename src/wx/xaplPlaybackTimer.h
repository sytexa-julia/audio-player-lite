#ifndef __XAPL_PLAYBACK_TIMER_H
#define __XAPL_PLAYBACK_TIMER_H

#include <list>
#include <wx/wx.h>
#include "xaplTimeObserver.h"

using std::list;

class xaplPlaybackTimer :
	public wxThread
{
private:
	list<xaplTimeObserver *> timeObservers;

public:
	xaplPlaybackTimer( void );
	~xaplPlaybackTimer( void );

	void AddTimeObserver( xaplTimeObserver *observer );
	void RemoveTimeObserver( xaplTimeObserver *observer );

	virtual void *Entry( void );
	void Notify( void );
};

#endif
