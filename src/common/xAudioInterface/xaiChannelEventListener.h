#ifndef __EVENT_CALLBACK_LISTENER_H
#define __EVENT_CALLBACK_LISTENER_H

#ifdef __OBJC__
	@protocol XChannelEventListener

	- (void) onChannelEvent:(int) eventID;

	@end
#else
	class xaiChannelEventListener
	{
	public:
		virtual void OnChannelEvent( int eventID ) = 0;
	};

#endif

#endif
