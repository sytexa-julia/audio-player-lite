#ifndef __XAPL_VOLUME_SUBJECT_H
#define __XAPL_VOLUME_SUBJECT_H

#include <list>
#include "xaplVolumeObserver.h"

using std::list;

class xaplVolumeSubject
{
private:
	list<xaplVolumeObserver *> volumeObservers;

public:
	void AddVolumeObserver( xaplVolumeObserver *observer );
	void RemoveVolumeObserver( xaplVolumeObserver *observer );
	virtual bool NotifyVolumeChanged( float newVol );
};

#endif
