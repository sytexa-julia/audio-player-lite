#ifndef __XAPL_VOLUME_OBSERVER_H
#define __XAPL_VOLUME_OBSERVER_H

class xaplVolumeObserver
{
public:
	virtual void OnVolumeChanged( float newVol ) = 0;
};

#endif
