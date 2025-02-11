#ifndef __EVENT_CALLBACK_LISTENER_H
#define __EVENT_CALLBACK_LISTENER_H

class xaiChannelEventListener
{
public:
	virtual void OnChannelEvent( int eventID ) = 0;
};

#endif
